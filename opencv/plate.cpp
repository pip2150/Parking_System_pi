#include "plate.hpp"
#include <omp.h>

using namespace cv;
using namespace std;

Plate::Plate(const Mat &img, const Point &position) {
	this->img = img;
	this->position = position;
}
Plate::Text::Text(const Mat &src) { this->img = src; }
void Plate::setDebug(bool debug) { this->debug = debug; }
static int lowThreshold = 45, highThreshold = 175;

Mat Plate::Text::canonical(int sampleSize) {
	Mat canonical;
	resize(img, canonical, Size2d(sampleSize, sampleSize));

	return canonical;
}

RotatedRect Plate::minApproxRect(const vector<Point> &contour) {
	vector<Point> approxCurve;

	/* 근사도 */
	double eps = contour.size() * 0.1;
	approxPolyDP(contour, approxCurve, eps, true);

	/* 사각형 이하의 다각형 제외 */
	if (approxCurve.size() < 4)
		return RotatedRect();

	/*	if (!isContourConvex(approxCurve))
	return RotatedRect();*/

	return minAreaRect(contour);
}

void Plate::find(const Mat &image, vector<Plate> *PossiblePlates) {
	srand((int)time(NULL));

	Scalar green(0, 255, 0);

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);

	Mat blr;

	/* 최고 해상도 */
	int maxSize = 960 * 720;
	int graySize = gray.size().area();
	float redRatio = 1.0;

	Mat lowRes;

	/* 최고 해상도 이상 해상도를 강제로 저하 */
	if (graySize > maxSize) {
		redRatio = sqrtf((float)maxSize / graySize);
		resize(gray, lowRes, Size2f(redRatio*gray.cols, redRatio*gray.rows));
	}
	else
		lowRes = gray;

	blur(lowRes, blr, Size(3, 3));

    /*
	namedWindow("bin");

	createTrackbar("min threahold", "bin", &lowThreshold, 300);
	createTrackbar("max threahold", "bin", &highThreshold, 300);
    */
	Mat sobel;
	Canny(blr, sobel, lowThreshold, highThreshold, 3);
	//	Sobel(blr, sobel, CV_8U, 1, 0, 3);

	Mat thImg;
	//threshold(sobel, thImg, 0, 255, THRESH_OTSU + THRESH_BINARY);

	Mat morph;
	//	Mat kernel(3, 17, CV_8UC1, Scalar(1));
	//	morphologyEx(thImg, morph, MORPH_CLOSE, kernel);

    //imshow("bin", sobel);

	vector < vector< Point> > contours;
	findContours(sobel, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	/* OpenMP Thread 생성 */
#pragma omp parallel
	/* implicit barrier 사용안함 */
#pragma omp for nowait
	for (int i = 0; i < contours.size(); i++) {
		RotatedRect mr = minApproxRect(contours[i]);

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

		/*	------ 번호판에 floodfill 연산 ------ */

		int area = 0;
		for (int j = 0; j < 10; j++) {
			float radius = rand() % (int)minSize - (float)(minSize * 0.5);
			Point2f seed = mr.center + Point2f(radius, radius);
			if (Rect(0, 0, blr.size().width, blr.size().height).contains(seed)) {
				area = floodFill(blr, mask, seed, Scalar(255), &ccomp, loDiff, upDiff, flags);
				break;
			}
		}
		if (!area)
			continue;

		/*	------ /번호판에 floodfill 연산 ------ */

		vector<vector<Point> > plateContours;
		findContours(mask(ccomp + Point(1, 1)), plateContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		for (auto plateContour : plateContours) {
			RotatedRect minRect = minApproxRect(plateContour);

			if (!verifySizes(minRect))
				continue;

			/* ------ image warping ------ */

			minRect = RotatedRect(minRect.center / redRatio, Size2f(minRect.size / redRatio), minRect.angle);

			Point2f src[4];
			minRect.points(src);

			Size m_size = minRect.size;

			if (minRect.size.width < minRect.size.height)
				swap(m_size.width, m_size.height);

			Point2f plateCorner[4] = {
				Point(0, m_size.height), Point(0, 0),
				Point(m_size.width, 0), Point(m_size.width, m_size.height)
			};

			if (minRect.size.width < minRect.size.height)
				for (int k = 1; k < 4; k++)
					swap(plateCorner[0], plateCorner[k]);

			ccomp = Rect(ccomp.tl() / redRatio, ccomp.br() / redRatio);

			drawRotatedRect(image(ccomp), minRect, green);

			Mat imgCrop;
			Mat M = getPerspectiveTransform(src, plateCorner);
			warpPerspective(gray(ccomp), imgCrop, M, m_size);

			/* ------ /image warping ------ */

			Point position = minRect.center + (Point2f)ccomp.tl();
			Plate plate(imgCrop, position);

			/* thread 에 대한 ciritical 영역 생성*/
#pragma	omp	critical
			PossiblePlates->push_back(plate);

		}
	}

}

Mat Plate::canonical() {
	Mat canonical;
	resize(img, canonical, Size(144, 33), 0, 0, INTER_CUBIC);
	equalizeHist(canonical, canonical);
	canonical = canonical.reshape(1, 1);
	canonical.convertTo(canonical, CV_32FC1);

	return canonical;
}

/*
	*	      Size
	*	      ^
	*	      |
	*	      |
	*	rmax _|       _____________
	*	      |      |             |
	*	      |      |     rect    |
	*	rmin _|      |_____________|
	*	      |________________________________> ratio
	*	             |min          |max
*/
inline bool Plate::verifySizes(const RotatedRect &mr) {
	float min = 800;
	float max = 70000;

	float rmin = 2.0;
	float rmax = 5.5;
	Rect2f rect(min, rmin, max, rmax);

	float asp_max = mr.size.width;
	float asp_min = mr.size.height;

	if (asp_min > asp_max)
		swap(asp_max, asp_min);

	Point2f pt(mr.size.area(), asp_max / asp_min);

	return rect.contains(pt);
}

bool Plate::findTexts(const int textSize) {

	Mat thresholded;
	threshold(img, thresholded, 0, 255, THRESH_OTSU + THRESH_BINARY);
	thresholded = ~thresholded;
	//adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

	vector<vector<Point> > contours;
	findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int contoursSize = (int)contours.size();
	if (contoursSize < textSize)
		return false;

	Size plateSize = thresholded.size();

	Mat contoursfound(plateSize, CV_8UC4, Scalar(255, 255, 255));

	line(contoursfound,
		Point2d(0, plateSize.height * 0.5),
		Point2d(plateSize.width, plateSize.height * 0.5),
		Scalar(255, 0, 255)
	);

	srand((int)time(0));		// 숫자 색 seed 값

	/* ----- 숫자 영역 비율 및 위치 검사 ----- */

	struct cmp {
		bool operator()(Rect t, Rect u) { return t.x > u.x; }
	};

	priority_queue<Rect, vector<Rect>, cmp> rectPQ;
	int k = 0;

	for (auto contour : contours) {
		Rect contourRect = boundingRect(contour);

		int dy = contourRect.y + contourRect.height;
		int lx = contourRect.x;
		int uy = contourRect.y;
		int rx = contourRect.x + contourRect.width;

		/* 가로 세로 비율 */
		double ratio = ((double)contourRect.width / (double)contourRect.height)
			* ((double)plateSize.width / (double)plateSize.height);

		/* 0.5 < ratio < 5 */
		if ((ratio > 0.5) && (ratio < 5)) {
			/*
			*	번호판의 중심 횡단부를 차지하는 contour 만 허용
			*	       _____________________________
			*	      |    ____     _      _____    |
			*	      |   |    |     |          |   |
			*	      |___|____|_____|__________|___|
			*	      |   |    |     |     |        |
			*	      |   |____|   __|__   |_____   |
			*	      |_____________________________|
			*/
			if ((dy > plateSize.height * 0.5) && (uy < plateSize.height * 0.5)) {
				/* 숫자 출력 시 랜덤 색상으로 출력 */
				vector<vector<Point> > tmp;
				tmp.push_back(contour);
				drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, 0, rand() % 255), 1);

				/* Priority Queue에 입력 */
				rectPQ.push(contourRect);
			}
		}
	}

	/* ----- /숫자 영역 비율 및 위치 검사 ----- */

	if (debug) {
		imshow("thresholded", thresholded);
		imshow("contoursfound", contoursfound);
	}

	if (rectPQ.size() < textSize)
		return false;

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

		Mat text = ~thresholded(preRect);
		texts.push_back(text);

		preRect = curRect;
	}

	/* ----- /x좌표 정렬 및 겹침 검사 ----- */

	return (textSize == texts.size());
}

inline bool Plate::isOverlap(const Rect &A, const Rect &B) {
	return (A & B).area() > 0;
}

void Plate::drawRotatedRect(const Mat &img, const RotatedRect &roRec, const Scalar color, int thickness) {

	Point2f vertices[4];
	roRec.points(vertices);
	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], color, thickness);
}
