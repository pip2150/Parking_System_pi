#include "Plate.hpp"
#include <omp.h>

using namespace cv;
using namespace std;

Plate::Plate() {}
Plate::Plate(const Mat &img) { this->img = img; }
Plate::Plate(const Mat &img, const Point &position) {
	this->img = img;
	this->position = position;
}
Plate::Number::Number(const Mat &src) { this->img = src; }
void Plate::setDebug(bool debug) { this->debug = debug; }
static int lowThreshold = 50, highThreshold = 100;

/*		���� ���� ����ȭ		*/
void Plate::Number::canonicalize(int sampleSize) {
	resize(img, canonical, Size2d(sampleSize, sampleSize));
}

RotatedRect Plate::minApproxRect(const vector<Point> &contour) {
	vector<Point> approxCurve;

	double eps = contour.size() * 0.1;
	approxPolyDP(contour, approxCurve, eps, true);

	if (approxCurve.size() < 4)
		return RotatedRect();

	/*	if (!isContourConvex(approxCurve))
	return RotatedRect();*/

	return minAreaRect(contour);
}

/*		��ȣ�� ���� ����		*/
void Plate::find(const Mat &image, vector<Plate> *PossiblePlates) {
	srand((int)time(NULL));

	Scalar green(0, 255, 0);

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);

	Mat blr;
	int maxSize = 960 * 720;
	int graySize = gray.size().area();
	float redRatio = 1.0;

	Mat lowRes;
	if (graySize > maxSize) {
		redRatio = sqrtf((float)maxSize / graySize);
		resize(gray, lowRes, Size2f(redRatio*gray.cols, redRatio*gray.rows));
	}
	else
		lowRes = gray;

	blur(lowRes, blr, Size(3, 3));

	createTrackbar("min threahold", "bin", &lowThreshold, 300);
	createTrackbar("max threahold", "bin", &highThreshold, 300);

	Mat sobel;
	Canny(blr, sobel, lowThreshold, highThreshold, 3);
	//	Sobel(blr, sobel, CV_8U, 1, 0, 3);

	Mat thImg;
	threshold(sobel, thImg, 0, 255, THRESH_OTSU + THRESH_BINARY);

	Mat morph;
	//	Mat kernel(3, 17, CV_8UC1, Scalar(1));
	//	morphologyEx(thImg, morph, MORPH_CLOSE, kernel);

	imshow("bin", thImg);

	vector < vector< Point> > contours;
	findContours(sobel, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

#pragma omp parallel
#pragma omp for nowait
	for (int i = 0; i < contours.size();i++) {
		RotatedRect mr = minApproxRect(contours[i]);

		/*		ũ�� �˻�		*/
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

		/*		��ȣ�ǿ� floodfill ����		*/
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

		vector<vector<Point> > plateContours;
		findContours(mask(ccomp + Point(1, 1)), plateContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		for (auto plateContour : plateContours) {
			RotatedRect minRect = minApproxRect(plateContour);

			if (!verifySizes(minRect))
				continue;

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

			Point position = minRect.center + (Point2f)ccomp.tl();
			Plate plate(imgCrop, position);

#pragma	omp	critical
			PossiblePlates->push_back(plate);

		}
	}

}

/*		��ȣ�� ����ȭ		*/
void Plate::canonicalize() {
	resize(img, canonical, Size(144, 33), 0, 0, INTER_CUBIC);
	equalizeHist(canonical, canonical);
	canonical = canonical.reshape(1, 1);
	canonical.convertTo(canonical, CV_32FC1);
}

/*		ũ�� �˻�		*/
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

/*		��ȣ�ǿ��� ���ڿ��� ����		*/
bool Plate::findNumbers(const int number) {

	Mat thresholded;
	threshold(img, thresholded, 0, 255, THRESH_OTSU + THRESH_BINARY);
	thresholded = ~thresholded;
	//adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

	vector<vector<Point> > contours;
	findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int contoursSize = (int)contours.size();
	if (contoursSize < number)
		return false;

	Size flatSize = thresholded.size();

	Mat contoursfound(flatSize, CV_8UC4, Scalar(255, 255, 255));
	line(contoursfound,
		Point2d(0, flatSize.height * 0.5),
		Point2d(flatSize.width, flatSize.height * 0.5),
		Scalar(255, 0, 255)
	);

	srand((int)time(0));		// ���� �� seed ��

	/* ----- ���� ���� ���� �� ��ġ �˻� ----- */

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

		double ratio = ((double)contourRect.width / (double)contourRect.height)
			* ((double)flatSize.width / (double)flatSize.height);

		if ((ratio > 0.5) && (ratio < 5)) {
			if ((dy > flatSize.height * 0.5) && (uy < flatSize.height * 0.5)) {
				/* ���� ��� �� ���� �������� ��� */
				vector<vector<Point> > tmp;
				tmp.push_back(contour);
				drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, 0, rand() % 255), 1);
				rectPQ.push(contourRect);
			}
		}
	}

	if (debug) {
		imshow("thresholded", thresholded);
		imshow("contoursfound", contoursfound);
	}

	if (rectPQ.size() < number)
		return false;

	/* ----- x��ǥ ���� �� ��ħ �˻� ----- */
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

/*		��ħ �˻�	*/
inline bool Plate::isOverlap(const Rect &A, const Rect &B) {
	return (A & B).area() > 0;
}

/*		RotatedRect �׸���		*/
void Plate::drawRotatedRect(const Mat &img, const RotatedRect &roRec, const Scalar color, int thickness, int lineType, int shift) {

	Point2f vertices[4];
	roRec.points(vertices);
	for (int i = 0; i < 4; i++)
		line(img, vertices[i], vertices[(i + 1) % 4], color, thickness, lineType, shift);
}
