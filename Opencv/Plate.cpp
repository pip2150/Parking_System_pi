#include "Plate.hpp"

using namespace cv;
using namespace std;

inline Plate::Number::Number(Mat &src) {
	this->img = src;
}

/*		숫자 영역 정규화		*/
void Plate::Number::canonicalize(int sampleSize) {
	Size imgSize = img.size();

	double ratio = (img.rows > img.cols) ?
		((double)sampleSize / (double)imgSize.height) :
		((double)sampleSize / (double)imgSize.width);

	Mat resized;

	resize(img, resized, Size2d(ratio*imgSize.width, ratio*imgSize.height));

	Size resizedSize = resized.size();
	int width = resizedSize.width;
	int height = resizedSize.height;
	double x = (double)(sampleSize - width) / 2;
	double y = (double)(sampleSize - height) / 2;

	canonical = Mat(Size(sampleSize, sampleSize), CV_8UC1, Scalar(255));
	resized.copyTo(canonical(Rect2d(x, y, width, height)));

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

	Mat sobel, th_img, morph;
	Sobel(gray, sobel, CV_8U, 1, 0, 3);

	threshold(sobel, th_img, 0, 255, THRESH_OTSU + THRESH_BINARY);

	Mat kernel(3, 17, CV_8UC1, Scalar(1));
	morphologyEx(th_img, morph, MORPH_CLOSE, kernel);

	vector < vector< Point> > contours;
	findContours(morph, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	int contoursSize = contours.size();
	for (int i = 0; i < contoursSize; i++) {
		RotatedRect mr = minAreaRect(contours[i]);
		
		if (!verifySizes(mr))
			continue;

		/*		번호판 영역 감지		*/
		RotatedRect* rect = &mr;
		//rectangle(image, rect->boundingRect(), Scalar(0, 255, 0), 1);
		Size size = rect->size;

		int minSize = (size.width < size.height) ? size.width : size.height;
		minSize = (int)cvRound(minSize*0.3);

		Scalar IoDiff(40, 40, 40);
		Scalar upDiff(40, 40, 40);
		int connectivity = 4;
		int flags = connectivity + 0xff00 + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;

		Rect ccomp;

		int new_x = max((int)rect->center.x - 265, 0);
		int new_y = max((int)rect->center.y - 265, 0);
		int new_width = min(gray.size().width - new_x, 530);
		int new_height = min(gray.size().height - new_y, 530);

		Mat ff = gray(Rect(new_x, new_y, new_width, new_height));

		Mat mask(ff.size() + Size(2, 2), CV_8UC1, Scalar(0));
		for (int j = 0; j < 10; j++) {
			int radius = rand() % minSize - (minSize / 2);
			Point seed = (Point)rect->center - Point(new_x, new_y) + Point(radius, radius);
			//(Point)rect->center + Point(radius, radius);
			if (Rect(0, 0, ff.cols, ff.rows).contains(seed))
				int area = floodFill(ff, mask, seed, Scalar(250, 0, 0), &ccomp, IoDiff, IoDiff, flags);
		}

		vector<Point> pointsInterest;
		Mat_<uchar>::iterator itMask = mask.begin<uchar>();
		Mat_<uchar>::iterator end = mask.end<uchar>();
		for (; itMask != end; ++itMask) {
			if (*itMask == 255) {
				pointsInterest.push_back(itMask.pos());
			}
		}

		RotatedRect minRect = minAreaRect(pointsInterest);
		//rectangle(image, minRect.boundingRect(), Scalar(255, 0, 1));

		if (verifySizes(minRect)) {
			
			PlatePositions.push_back(Point(new_x,new_y));

			Mat img_rotated, img_crop;

			Size m_size = minRect.size;
			float aspect = (float)m_size.width / m_size.height;
			float angle = minRect.angle;

			if (aspect < 1) {
				angle += 90;
				swap(m_size.width, m_size.height);
			}

			Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);
			warpAffine(ff, img_rotated, rotmat, ff.size(), CV_INTER_CUBIC);
			getRectSubPix(img_rotated, m_size, minRect.center, img_crop);

			PossiblePlates.push_back(Plate(img_crop));
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
void Plate::findNumbers() {

	Size flatSize = img.size();
	int width = flatSize.width;
	int height = flatSize.height;

	double maxHigh = 0;
	double minLow = height;

	Mat thresholded;
	adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

	Mat canny;
	Canny(thresholded, canny, 50, 100, 3);

	vector<vector<Point> > contours;
	findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	imshow("thresholded", thresholded);
	moveWindow("thresholded", WINDOW_X, WINDOW_Y + flatSize.height + 50);

	/* ----- 숫자 영역 비율 및 위치 검사 ----- */
	Mat contoursfound(flatSize, CV_8UC4, Scalar(255, 255, 255));
	srand((int)time(0));		// 숫자 색 seed 값

	vector<Rect> rectNum;
	int contoursSize = contours.size();
	for (int i = 0; i < contoursSize; i++) {
		vector<Point> *contour = &contours[i];

		Point mPoint[4];
		endPoint(*contour, mPoint);
		/* 
		 = minAreaRect(contour);

		*/
		
		int uy = mPoint[UP].y;
		int dy = mPoint[DOWN].y;
		int lx = mPoint[LEFT].x;
		int rx = mPoint[RIGHT].x;

		int contourWidth = lx - rx;
		int contourHeight = uy - dy;
		double ratio = ((double)contourWidth / (double)contourHeight) * ((double)width / (double)height);

		if ((ratio > 0.5) && (ratio < 5)) {

			vector<vector<Point> > tmp;	tmp.push_back(*contour);
			drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, 0, rand() % 255), 1);	// 숫자 출력 시 랜덤 색상으로 출력
			line(contoursfound, Point(0, height / 2), Point(width, height / 2), Scalar(255, 0, 255));
			imshow("contoursfound", contoursfound);
			moveWindow("contoursfound", WINDOW_X, WINDOW_Y + (flatSize.height + 50) * 2);

			if ((uy > height / 2) && (dy < height / 2)) {
				if (rx >= 1)
					rx--;
				if (dy >= 1)
					dy--;
				rectNum.push_back(Rect(Point(lx, uy), Point(rx, dy)));
			}
		}
	}

	/* ----- x좌표 정렬 ----- */
	typedef pair<int, int> Index;
	priority_queue<Index> index;

	int rectNumSize = rectNum.size();
	for (int i = 0; i < rectNumSize; i++) {
		index.push(make_pair(rectNum[i].x, i));
	}

	vector<Rect> alignedNum;
	while (!index.empty()) {
		alignedNum.push_back(rectNum[index.top().second]);
		index.pop();
	}

	/* ----- 겹침 검사 ----- */
	vector<Rect> overlapRemoved;
	int alignedNumSize = alignedNum.size();
	for (int i = 0; i < alignedNumSize; i++) {
		if (i && !isOverlap(alignedNum[i - 1], alignedNum[i - 1]))
			overlapRemoved.push_back(alignedNum[i - 1] | alignedNum[i]);
		else
			overlapRemoved.push_back(alignedNum[i]);
	}

	int overlapRemovedSize = overlapRemoved.size();
	for (int i = 0; i < overlapRemovedSize; i++) {
		double ratio = (double)overlapRemoved[i].width / (double)overlapRemoved[i].height;
		Mat matNum = img(overlapRemoved[i]);
		Mat thresholdedNum;
		adaptiveThreshold(matNum, thresholdedNum, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 255, 0);
		numbers.push_back(thresholdedNum);
	}
}

/*		Contour 4방향 끝점 추출		*/
inline void Plate::endPoint(vector<Point> &contour, Point mPoint[4]) {
	int coordinate[4];

	coordinate[UP] = 0;
	coordinate[DOWN] = numeric_limits<int>::max();
	coordinate[LEFT] = 0;
	coordinate[RIGHT] = numeric_limits<int>::max();

	int contourSize = contour.size();
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

/*		겹치는 부분 검사	*/
inline bool Plate::isOverlap(Rect &A, Rect &B) {
	return (A & B).area() > 0;
}
