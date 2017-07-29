#include "Plate.hpp"

using namespace cv;
using namespace std;

Plate::Number::Number() {}

Plate::Number::Number(Mat &src) {
	this->img = src;
}

void Plate::Number::canonicalize(int sampleSize) {

	double ratio = (img.rows > img.cols) ?
		((double)sampleSize / (double)img.size().height) :
		((double)sampleSize / (double)img.size().width);

	Mat resized;

	resize(img, resized, Size2f(ratio*img.size().width, ratio*img.size().height));

	int width = resized.size().width;
	int height = resized.size().height;
	double x = (double)(sampleSize - width) / 2;
	double y = (double)(sampleSize - height) / 2;

	canonical = Mat(Size(sampleSize, sampleSize), resized.type(), Scalar(255));
	resized.copyTo(canonical(Rect2f(x, y, width, height)));

}

Plate::Plate(Mat &img) {
	this->img = img;
}

void Plate::find(Mat &gray, vector<RotatedRect> &rects) {
	Mat sobel, th_img, morph;

	Sobel(gray, sobel, CV_8U, 1, 0, 3);
	threshold(sobel, th_img, 0, 255, THRESH_OTSU + THRESH_BINARY);

	Mat kernel(3, 17, CV_8UC1, Scalar(1));

	morphologyEx(th_img, morph, MORPH_CLOSE, kernel);

	vector < vector< Point> > contours;

	findContours(morph.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	for (int i = 0; i < (int)contours.size(); i++) {
		RotatedRect mr = minAreaRect(contours[i]);

		if (verifySizes(mr))
			rects.push_back(mr);
		else
			contours[i].clear();
	}

}

RotatedRect Plate::detect(Mat &input, RotatedRect &rect) {
	Size size = rect.size;

	int minSize = (size.width < size.height) ? size.width : size.height;

	minSize = (int)cvRound(minSize*0.3);

	srand((int)time(NULL));

	Scalar IoDiff(40, 40, 40);
	Scalar upDiff(40, 40, 40);
	int connectivity = 4;
	int flags = connectivity + 0xff00 + FLOODFILL_FIXED_RANGE + FLOODFILL_MASK_ONLY;
	Rect ccomp;
	Mat mask(input.size() + Size(2, 2), CV_8UC1, Scalar(0));

	for (int j = 0; j < 10; j++) {
		int radius = rand() % (int)minSize - (minSize / 2);
		Point seed = (Point)rect.center + Point(radius, radius);
		int area = floodFill(input, mask, seed, Scalar(250, 0, 0), &ccomp, IoDiff, IoDiff, flags);
	}

	vector<Point> pointsInterest;
	Mat_<uchar>::iterator itMask = mask.begin<uchar>();
	Mat_<uchar>::iterator end = mask.end<uchar>();

	for (; itMask != end; ++itMask) {
		if (*itMask == 255) {
			pointsInterest.push_back(itMask.pos());
		}
	}

	return minAreaRect(pointsInterest);
}

void Plate::extract(Mat &input, vector<Plate> &PossiblePlates) {
	vector<RotatedRect> rects;
	Plate::find(input, rects);

	for (int i = 0; i < (int)rects.size(); i++) {
		RotatedRect minRect = Plate::detect(input, rects[i]);
		
		if (Plate::verifySizes(minRect)) {
			Mat img_rotated, img_crop;
			Size m_size = minRect.size;
			float aspect = (float)m_size.width / m_size.height;
			float angle = minRect.angle;

			if (aspect < 1) {
				angle += 90;
				swap(m_size.width, m_size.height);
			}

			Mat rotmat = getRotationMatrix2D(minRect.center, angle, 1);

			warpAffine(input, img_rotated, rotmat, input.size(), CV_INTER_CUBIC);
			getRectSubPix(img_rotated, m_size, minRect.center, img_crop);

			PossiblePlates.push_back(Plate(img_crop));
		}
		
	}
}

Mat Plate::canonicalize() {
	Mat resize_img;
	resize(img, resize_img, Size(144, 33), 0, 0, INTER_CUBIC);
	equalizeHist(resize_img, resize_img);
	resize_img = resize_img.reshape(1, 1);
	resize_img.convertTo(resize_img, CV_32FC1);

	return resize_img;
}

bool Plate::verifySizes(RotatedRect &mr) {
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

void Plate::findNumbers() {
	Mat thresholded;
	adaptiveThreshold(img, thresholded, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 255, 0);

	Mat canny;
	Canny(thresholded, canny, 50, 100, 3);

	vector<vector<Point> > contours;
	findContours(thresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	imshow("thresholded", thresholded);
	moveWindow("thresholded", 1200, 500 + thresholded.size().height + 50);

	typedef pair<pair<int, int>, pair<int, int> > Domain;

	priority_queue<Domain> pq;
	Size flatSize = thresholded.size();
	int width = flatSize.width;
	int height = flatSize.height;

	double maxHigh = 0;
	double minLow = height;

	/* ----- 숫자 위치의 범위 측정 ----- */
	Mat contoursfound(flatSize, CV_8UC4, Scalar(255, 255, 255));
	srand((int)time(0));		// 숫자 색 seed 값

	vector<Rect> num;
	for (int i = 0; i < contours.size(); i++) {
		vector<Point> contour = contours[i];

		Point mPoint[4];
		endPoint(contour, mPoint);

		int uy = mPoint[UP].y;
		int dy = mPoint[DOWN].y;
		int lx = mPoint[LEFT].x;
		int rx = mPoint[RIGHT].x;

		int contourWidth = lx - rx;
		int contourHeight = uy - dy;
		//double heightRatio = (double)contourWidth/ (double )contourHeight;
		double ratio = ((double)contourWidth / (double)contourHeight) * ((double)width / (double)height);

		if ((ratio > 0.5) && (ratio < 5)) {
			/*if (heightRatio < 0.9) {*/
				//cout << "ratio ;" << ratio << endl;
				/*if ((widthRatio > 0.015) && (widthRatio < 0.15)) {*/

			vector<vector<Point> > tmp;	tmp.push_back(contour);
			drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, 0, rand() % 255), 1);	// 숫자 출력 시 랜덤 색상으로 출력
			line(contoursfound, Point(0, height / 2), Point(width, height / 2), Scalar(255, 0, 255));
			imshow("contoursfound", contoursfound);
			moveWindow("contoursfound", 1200, 500 + (contoursfound.size().height + 50) * 2);


			if ((uy > height / 2) && (dy < height / 2)) {
				if (rx >= 1)
					rx--;
				if (dy >= 1)
					dy--;
				num.push_back(Rect(Point(lx, uy), Point(rx, dy)));
			}
		}
	}

	typedef pair<int, int> Index;
	priority_queue<Index> index;

	for (int i = 0; i < num.size(); i++) {
		index.push(make_pair(num[i].x, i));
	}

	vector<Rect> alignednum;
	while (!index.empty()) {
		alignednum.push_back(num[index.top().second]);
		index.pop();
	}

	vector<Rect> overlapRemoved;
	for (int i = 0; i < alignednum.size(); i++) {
		if ((i == 0) || ((alignednum[i - 1] & alignednum[i]).area() <= 0)) {
			overlapRemoved.push_back(alignednum[i]);
		}
		else {
			overlapRemoved.push_back(alignednum[i - 1] | alignednum[i]);
			//cout << "overlaped!!" << endl;
		}
	}

	/*vector<Rect> overlapRemoved;
	for (int i = 0; i < num.size(); i++) {
		overlapRemoved.push_back(num[i]);
	}*/

	for (int i = 0; i < overlapRemoved.size(); i++) {
		double ratio = (double)num[i].width / (double)num[i].height;
		/*rectangle(warpedImage, overlapRemoved[i], Scalar(255, 0, 0));*/
		numbers.push_back(Mat(~thresholded, overlapRemoved[i]));
	}

	/*imshow("warpedImage", warpedImage);
	moveWindow("warpedImage", 800, 800);*/


	/*for (int i = 0; i < range.size(); i++) {
		Rect rectNumber = Rect(Point2d(range[i].first, maxHigh), Point2d(range[i].second, minLow));
		numbers.push_back(Mat(warpedImage, rectNumber));
	}*/

}

void Plate::warpingRotatedRect(Mat &srcMat, Mat &dscMat, RotatedRect roPosition) {
	Point2f src[4];

	roPosition.points(src);

	Point v1 = src[1] - src[0];
	Point v2 = src[3] - src[0];

	double o = abs(v1.x * v2.y) - abs(v1.y * v2.x);

	if (o > 0.0) {
		swap(src[0], src[1]);
		swap(src[1], src[2]);
		swap(src[2], src[3]);
	}

	float e1 = sqrt(pow(src[2].x - src[1].x, 2)
		+ pow(src[2].y - src[1].y, 2));
	float e3 = sqrt(pow(src[3].x - src[0].x, 2)
		+ pow(src[3].y - src[0].y, 2));
	float e2 = sqrt(pow(src[3].x - src[2].x, 2)
		+ pow(src[3].y - src[2].y, 2));
	float e4 = sqrt(pow(src[0].x - src[1].x, 2)
		+ pow(src[0].y - src[1].y, 2));
	float maxWidth = (e1 < e3) ? e1 : e3;
	float maxHeight = (e2 < e4) ? e2 : e4;

	Size_<float> markerSize = Size((int)maxWidth, (int)maxHeight);
	Point2f m_markerCorners2d[] = { Point2f(0,maxHeight - 1), Point2f(0,0), Point2f(maxWidth - 1, 0), Point2f(maxWidth - 1,maxHeight - 1) };

	Mat M = getPerspectiveTransform(src, m_markerCorners2d);
	warpPerspective(srcMat, dscMat, M, markerSize);
}

void Plate::endPoint(vector<Point> &contour, Point mPoint[4]) {
	int coordinate[4];

	coordinate[UP] = 0;
	coordinate[DOWN] = numeric_limits<int>::max();
	coordinate[LEFT] = 0;
	coordinate[RIGHT] = numeric_limits<int>::max();

	for (int i = 0; i < contour.size(); i++) {
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