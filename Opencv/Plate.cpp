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

int Plate::minDistance(vector<Point> &approxCurve) {
	int minDist = numeric_limits<int>::max();

	for (int i = 0; i < approxCurve.size(); i++) {
		Point side = approxCurve[i] - approxCurve[(i + 1) % 4];
		int squaredSideLegth = side.dot(side);
		minDist = min(minDist, squaredSideLegth);
	}

	return minDist;
}

/*		번호판 영역 추출		*/
void Plate::find(Mat &image, vector<Plate> &PossiblePlates, vector<Point> &PlatePositions) {
	srand((int)time(NULL));

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);

	Mat blr;
	int maxSize = 640 * 480;
	double redRatio = 1;
	if (gray.size().area() > maxSize) {
		double width = (double)gray.cols;
		double height = (double)gray.rows;
		redRatio = pow(maxSize / (width*height), 0.5);
		resize(gray, blr, Size(redRatio*width, redRatio*height));
		blur(blr, blr, Size(3, 3));
	}
	else {
		blur(gray, blr, Size(3, 3));
	}

	Mat sobel;
	Sobel(blr, sobel, CV_8U, 1, 0, 3);

	Mat thImg;
	threshold(sobel, thImg, 0, 255, THRESH_OTSU + THRESH_BINARY);

	Mat morph;
	Mat kernel(3, 17, CV_8UC1, Scalar(1));
	morphologyEx(thImg, morph, MORPH_CLOSE, kernel);

	vector < vector< Point> > contours;
	findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int contoursSize = (int)contours.size();

#pragma omp parallel
#pragma omp for nowait
	for (int i = 0; i < contoursSize; i++) {
		vector<Point> approxCurve;

		double eps = contours[i].size() * 0.1;
		approxPolyDP(contours[i], approxCurve, eps, true);

		if (approxCurve.size() < 4)
			continue;

		if (!isContourConvex(approxCurve))
			continue;

		RotatedRect mr = minAreaRect(approxCurve);

		/*		크기 검사		*/
		if (!verifySizes(mr))
			continue;

		Size size = mr.size;

		int minSize = (size.width < size.height) ? size.width : size.height;
		minSize = (int)cvRound(minSize*0.3);

		Scalar loDiff(40, 40, 40);
		Scalar upDiff(40, 40, 40);
		int connectivity = 4;
		int flags = connectivity + 0xff00 + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
		Rect ccomp;
		Mat mask(blr.size() + Size(2, 2), CV_8UC1, Scalar(0));

		/*		번호판에 floodfill 연산		*/
		int area = 0;
		for (int j = 0; j < 10; j++) {
			float radius = rand() % (int)minSize - (float)(minSize * 0.5);
			Point2f seed = mr.center + Point2f(radius, radius);
			if (Rect(0, 0, blr.size().width, blr.size().height).contains(seed)) {
				area = floodFill(blr, mask, seed, Scalar(250), &ccomp, loDiff, upDiff, flags);
				break;
			}
		}
		if (!area)
			continue;

		vector<vector<Point> > plateContours;
		findContours(mask(ccomp + Point(1, 1)), plateContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		int plateContoursSize = (int)plateContours.size();
		for (int j = 0; j < plateContoursSize; j++) {
			vector<Point> approxCurve2;

			double eps2 = plateContours[j].size() * 0.1;
			approxPolyDP(plateContours[j], approxCurve2, eps2, true);

			if (approxCurve2.size() < 4)
				continue;

			if (!isContourConvex(approxCurve2))
				continue;

			RotatedRect minRect = minAreaRect(approxCurve2);

			if (!verifySizes(minRect))
				continue;

			/*
			drawRotatedRect(image, mr, Scalar(0, 0, 255));
			drawRotatedRect(image(ccomp), minRect, Scalar(0, 255, 0));
			*/

			Point2f src[4];
			minRect.points(src);

			for (int k = 0; k < 4; k++) {
				src[k] = Point2f(src[k].x / redRatio, src[k].y / redRatio);
			}

			Size m_size = Size(minRect.size.width / redRatio, minRect.size.height / redRatio);

			if (minRect.size.width < minRect.size.height) {
				swap(m_size.width, m_size.height);
			}

			Point2f plateCorner[4] = {
				Point(0, m_size.height), Point(0, 0),
				Point(m_size.width, 0), Point(m_size.width, m_size.height)
			};

			if (minRect.size.width < minRect.size.height) {
				for (int k = 1; k < 4; k++)
					swap(plateCorner[0], plateCorner[k]);
			}

			Rect ccomp_(ccomp.x / redRatio, ccomp.y / redRatio,
				ccomp.width / redRatio, ccomp.height / redRatio);

			Mat imgCrop;
			Mat M = getPerspectiveTransform(src, plateCorner);
			warpPerspective(gray(Rect(ccomp_)), imgCrop, M, m_size);

#pragma	omp	critical
			{
				PossiblePlates.push_back(imgCrop);
				PlatePositions.push_back(minRect.center);
			}
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
			}

			if ((uy > flatSize.height / 2.0) && (dy < flatSize.height / 2.0)) {
				rectPQ.push(Rect(Point(lx, uy), Point(rx, dy)));
			}
		}
	}

	/* ----- x좌표 정렬 및 겹침 검사 ----- */
	Rect preRect;
	if (!rectPQ.empty())
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
void Plate::drawRotatedRect(Mat img, RotatedRect roRec, const Scalar color, int thickness, int lineType, int shift) {

	Point2f vertices[4];
	roRec.points(vertices);
	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], color, thickness, lineType, shift);
}
