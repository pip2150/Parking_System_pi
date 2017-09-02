#include "Plate.hpp"
#include <omp.h>

using namespace cv;
using namespace std;

Plate::Plate() {}
Plate::Plate(Mat &img) { this->img = img; }
Plate::Number::Number(Mat &src) { this->img = src; }
void Plate::setDebug(bool debug) { this->debug = debug; }

/*		숫자 영역 정규화		*/
void Plate::Number::canonicalize(int sampleSize) {
	resize(img, canonical, Size2d(sampleSize, sampleSize));
}

RotatedRect Plate::minApproxRect(vector<Point> &contour) {
	vector<Point> approxCurve;

	double eps = contour.size() * 0.1;
	approxPolyDP(contour, approxCurve, eps, true);

	if (approxCurve.size() < 4)
		return RotatedRect();

	if (!isContourConvex(approxCurve))
		return RotatedRect();
	
	return minAreaRect(approxCurve);
}

/*		번호판 영역 추출		*/
void Plate::find(Mat &image, vector<Plate> &PossiblePlates, vector<Point> &PlatePositions) {
	srand((int)time(NULL));

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);

	Mat blr;
	int maxSize = 640 * 480;
	int graySize = gray.size().area();
	float redRatio = (float)pow(maxSize / graySize, 0.5);

	Mat lowRes;
	if (graySize > maxSize)
		resize(gray, lowRes, Size2f(redRatio*gray.cols, redRatio*gray.rows));
	else
		lowRes = gray;

	blur(lowRes, blr, Size(3, 3));

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
		RotatedRect mr = minApproxRect(contours[i]);

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
				area = floodFill(blr, mask, seed, Scalar(255), &ccomp, loDiff, upDiff, flags);
				break;
			}
		}
		if (!area)
			continue;

		vector<vector<Point> > plateContours;
		findContours(mask(ccomp + Point(1, 1)), plateContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		int plateContoursSize = (int)plateContours.size();
		for (int j = 0; j < plateContoursSize; j++) {
			RotatedRect minRect = minApproxRect(plateContours[j]);

			if (!verifySizes(minRect))
				continue;

			minRect = RotatedRect(
				minRect.center / redRatio,
				Size2f(minRect.size.width / redRatio, minRect.size.height / redRatio),
				minRect.angle
			);
			
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

			drawRotatedRect(image, mr, Scalar(0, 0, 255));
			drawRotatedRect(image(ccomp), minRect, Scalar(0, 255, 0));

			Mat imgCrop;
			Mat M = getPerspectiveTransform(src, plateCorner);
			warpPerspective(gray(ccomp), imgCrop, M, m_size);

#pragma	omp	critical
			{
				PossiblePlates.push_back(imgCrop);
				PlatePositions.push_back(minRect.center + (Point2f)ccomp.tl());
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
	Rect2f rect(min, rmin, max, rmax);

	float asp_max = mr.size.width;
	float asp_min = mr.size.height;

	if (asp_min > asp_max)
		swap(asp_max, asp_min);

	Point2f pt(mr.size.area(), asp_max / asp_min);

	return rect.contains(pt);
}

/*		번호판에서 숫자영역 추출		*/
bool Plate::findNumbers(int number) {

	Mat thresholded;
	adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

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

	srand((int)time(0));		// 숫자 색 seed 값

	/* ----- 숫자 영역 비율 및 위치 검사 ----- */

	struct cmp {
		bool operator()(Rect t, Rect u) { return t.x > u.x; }
	};

	priority_queue<Rect, vector<Rect>, cmp> rectPQ;
	int k = 0;

	for (int i = 0; i < contoursSize; i++) {
		Rect contourRect = boundingRect(contours[i]);

		int dy = contourRect.y + contourRect.height;
		int lx = contourRect.x;
		int uy = contourRect.y;
		int rx = contourRect.x + contourRect.width;

		double ratio = ((double)contourRect.width / (double)contourRect.height)
			* ((double)flatSize.width / (double)flatSize.height);

		if ((ratio > 0.5) && (ratio < 5)) {
			if ((dy > flatSize.height * 0.5) && (uy < flatSize.height * 0.5)) {
				/* 숫자 출력 시 랜덤 색상으로 출력 */
				vector<vector<Point> > tmp;
				tmp.push_back(contours[i]);
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