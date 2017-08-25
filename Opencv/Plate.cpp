#include "Plate.hpp"
#include <omp.h>

using namespace cv;
using namespace std;

void Plate::setDebug(bool debug) {
	this->debug = debug;
}

inline Plate::Number::Number(Mat &src) {
	this->img = src;
}

/*		숫자 영역 정규화		*/
void Plate::Number::canonicalize(int sampleSize) {	
	resize(img, canonical, Size2d(sampleSize, sampleSize));
}

inline Plate::Plate() {
}

inline Plate::Plate(Mat &img) {
	this->img = img;
}

/*		번호판 영역 추출		*/
void Plate::find(Mat &image, vector<Plate> &PossiblePlates, vector<Point> &PlatePositions) {
	srand((int)time(NULL));

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);
	blur(gray, gray, Size(3, 3));

	Mat sobel;
	Sobel(gray, sobel, CV_8U, 1, 0, 3);
	
	Mat thImg;
	threshold(sobel, thImg, 0, 255, THRESH_OTSU + THRESH_BINARY);
	
	Mat morph;
	Mat kernel(3, 17, CV_8UC1, Scalar(1));
	morphologyEx(thImg, morph, MORPH_CLOSE, kernel);

	vector < vector< Point> > contours;
	findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int contoursSize = (int)contours.size();
//#pragma omp parallel for
	for (int i = 0; i < contoursSize; i++) {
		RotatedRect mr = minAreaRect(contours[i]);

		/*		크기 검사		*/
		if (!verifySizes(mr))
			continue;

		RotatedRect* rect = &mr;
		Size size = rect->size;

		int minSize = (size.width < size.height) ? size.width : size.height;
		minSize = (int)cvRound(minSize*0.3);

		Scalar loDiff(40, 40, 40);
		Scalar upDiff(40, 40, 40);
		int connectivity = 4;
		int flags = connectivity + 0xff00 + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
		Rect ccomp;
		Mat mask(gray.size() + Size(2, 2), CV_8UC1, Scalar(0));

		/*		번호판에 floodfill 연산		*/
		for (int j = 0; j < 10; j++) {
			int radius = rand() % (int)minSize - (minSize / 2);
			Point seed = (Point)rect->center + Point(radius, radius);
			if (Rect(0, 0, gray.size().width, gray.size().height).contains(seed))
				int area = floodFill(gray, mask, seed, Scalar(250, 0, 0), &ccomp, loDiff, upDiff, flags);
		}

		vector<vector<Point> > plateContours;

		findContours(Mat(mask, Rect(1, 1, mask.size().width - 1, mask.size().height - 1)), plateContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		for (int j = 0; j < plateContours.size(); j++) {
			RotatedRect minRect = minAreaRect(plateContours[j]);

			if (!verifySizes(minRect))
				continue;

			Size m_size = minRect.size;
			float angle = minRect.angle;

			if (m_size.width < m_size.height) {
				angle += 90;
				swap(m_size.width, m_size.height);
			}

			/*		RotateRect 회전		*/
			Mat imgRotated;
			Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
			warpAffine(gray, imgRotated, rotmat, gray.size(), CV_INTER_CUBIC);

			Mat imgCrop;
			getRectSubPix(imgRotated, m_size, minRect.center, imgCrop);
//#pragma	omp	critical (IMG)
			PossiblePlates.push_back(imgCrop);
//#pragma	omp	critical (POSITION)
			PlatePositions.push_back(rect->center);
		}
	}
	
}

/*		번호판 정규화		*/
void Plate::canonicalize() {
	resize(img, canonical, Size(144, 33), 0, 0, INTER_CUBIC);
	equalizeHist(canonical, canonical);
	canonical = canonical.reshape(1, 1);
	canonical.convertTo(canonical, CV_32FC1);
}

/*		크기 검사		*/
inline bool Plate::verifySizes(RotatedRect &mr) {
	float min = 800;
	float max = 70000;

	float rmin = 2.0;
	float rmax = 5.5;
	Rect_<float> rect(min, rmin, max, rmax);

	float asp_max = mr.size.width;
	float asp_min = mr.size.height;

	if (asp_min > asp_max)
		swap(asp_max, asp_min);

	Point2f pt((float)mr.size.area(), asp_max / asp_min);

	return rect.contains(pt);
}

/*		번호판에서 숫자영역 추출		*/
bool Plate::findNumbers(int number) {

	Mat thresholded;
	adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

	vector<vector<Point> > contours;
	findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	Size flatSize = thresholded.size();

	if (debug) {
		imshow("thresholded", thresholded);
		moveWindow("thresholded", WINDOW_X, WINDOW_Y + flatSize.height + 50);
	}

	Mat contoursfound(flatSize, CV_8UC4, Scalar(255, 255, 255));
	srand((int)time(0));		// 숫자 색 seed 값

	/* ----- 숫자 영역 비율 및 위치 검사 ----- */

	struct cmp {
		bool operator()(Rect t, Rect u) { return t.x > u.x; }
	};

	priority_queue<Rect, vector<Rect>, cmp> rectPQ;
	int k = 0;

	int contoursSize = (int)contours.size();
	for (int i = 0; i < contoursSize; i++) {
		vector<Point> *contour = &contours[i];

		Point mPoint[4];
		endPoint(*contour, mPoint);

		int uy = min(mPoint[UP].y + 1, flatSize.height);
		int lx = min(mPoint[LEFT].x + 1, flatSize.width);
		int dy = max(mPoint[DOWN].y - 1, 0);
		int rx = max(mPoint[RIGHT].x - 1, 0);

		int contourWidth = lx - rx;
		int contourHeight = uy - dy;
		double ratio = ((double)contourWidth / (double)contourHeight) * ((double)flatSize.width / (double)flatSize.height);

		if ((ratio > 0.5) && (ratio < 5)) {

			if (debug) {
				vector<vector<Point> > tmp;	tmp.push_back(*contour);
				drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, 0, rand() % 255), 1);	// 숫자 출력 시 랜덤 색상으로 출력
				line(contoursfound, Point2d(0, flatSize.height / 2.0), Point2d(flatSize.width, flatSize.height / 2.0), Scalar(255, 0, 255));
				imshow("contoursfound", contoursfound);
				moveWindow("contoursfound", WINDOW_X, WINDOW_Y + (flatSize.height + 50) * 2);
			}

			if ((uy > flatSize.height / 2.0) && (dy < flatSize.height / 2.0)) {
				rectPQ.push(Rect(Point(lx, uy), Point(rx, dy)));
			}
		}
	}

	/* ----- x좌표 정렬 및 겹침 검사 ----- */
	Rect preRect;
	if(!rectPQ.empty())
		preRect = rectPQ.top();

	while (!rectPQ.empty()) {
		Rect curRect = rectPQ.top();
		rectPQ.pop();

		if (isOverlap(curRect, preRect))
			preRect = preRect | curRect;
		else
			preRect = curRect;
		
		Mat matNum = ~thresholded(preRect);
		numbers.push_back(matNum);
		
		preRect = curRect;
	}

	return (number == numbers.size());
}

/*		Contour 4방향 끝점 추출		*/
inline void Plate::endPoint(vector<Point> &contour, Point mPoint[4]) {
	int coordinate[4];

	coordinate[UP] = 0;
	coordinate[DOWN] = numeric_limits<int>::max();
	coordinate[LEFT] = 0;
	coordinate[RIGHT] = numeric_limits<int>::max();

	int contourSize = (int)contour.size();
	for (int i = 0; i < contourSize; i++) {
		if (coordinate[UP] < contour[i].y) {
			coordinate[UP] = contour[i].y;
			mPoint[UP] = contour[i];
		}
		if (coordinate[DOWN] > contour[i].y) {
			coordinate[DOWN] = contour[i].y;
			mPoint[DOWN] = contour[i];
		}
		if (coordinate[LEFT] < contour[i].x) {
			coordinate[LEFT] = contour[i].x;
			mPoint[LEFT] = contour[i];
		}
		if (coordinate[RIGHT] > contour[i].x) {
			coordinate[RIGHT] = contour[i].x;
			mPoint[RIGHT] = contour[i];
		}
	}
}

/*		겹침 검사	*/
inline bool Plate::isOverlap(Rect &A, Rect &B) {
	return (A & B).area() > 0;
}

/*		RotatedRect 그리기		*/
void Plate::drawRotatedRect(Mat& img, RotatedRect roRec, const Scalar& color, int thickness, int lineType , int shift) {

	Point2f vertices[4];
	roRec.points(vertices);
	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
}