// Opencv.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <opencv/cv.hpp>
#include <opencv/highgui.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <queue>

using namespace cv;
using namespace std;

typedef vector< vector < Point > > ContoursVector;

class Detector
{

private:
	int minContourPointsAllowed;
	int threshold1, threshold2;

public:
	Detector(int minContourPointsAllowed, int threshold1, int threshold2) {
		this->minContourPointsAllowed = minContourPointsAllowed;
		this->threshold1 = threshold1;
		this->threshold2 = threshold2;
	}
	void prepareImage(const Mat &bgraMat, Mat &grayscale) {
		cvtColor(bgraMat, grayscale, COLOR_BGRA2GRAY);
	}

	void performThreshold(const Mat& grayscale, Mat& thresholdImg) {
		/*adaptiveThreshold(grayscale,
			thresholdImg,
			255,
			ADAPTIVE_THRESH_GAUSSIAN_C,
			THRESH_BINARY_INV,
			7,
			7
		);*/
		Canny(grayscale, thresholdImg, threshold1, threshold2, 3);
	}

	void mfindContours(const Mat& thresholdImg, vector<vector<Point> >& contours) {
		vector<vector<Point> > allContours;

		findContours(thresholdImg, allContours, RETR_LIST, CHAIN_APPROX_NONE);

		contours.clear();
		for (size_t i = 0; i < allContours.size(); i++) {
			size_t contourSize = allContours[i].size();
			if (contourSize > minContourPointsAllowed)
			{
				contours.push_back(allContours[i]);
			}

		}
	}

	static void drawMarkerRect(Mat gray, vector<vector <Point >> detectedMarkers) {
		for (int i = 0; i < detectedMarkers.size(); i++) {
			if (detectedMarkers[i].size() == 4) {
				vector<Point> points = detectedMarkers[i];
				int x = min(points[2].x, points[3].x);
				int y = min(points[3].y, points[0].y);
				int width = max(points[0].x, points[1].x) - x;
				int height = max(points[1].y, points[2].y) - y;
				Rect area = Rect(x, y, width, height);
				rectangle(gray, area, Scalar(255), 2);
			}
		}
	}
};

class MarkerDetector : public Detector {
private :
	float minContourLengthAllowd;
	float approxDgree;

public :
	MarkerDetector(float minContourLengthAllowd, int minContourPointsAllowed, int threshold1, int threshold2, float approxDgree)
		: Detector(minContourPointsAllowed, threshold1, threshold2) {
		this->minContourLengthAllowd = minContourLengthAllowd;
		this->approxDgree = approxDgree;
	}
	
	void findCandidates(const ContoursVector& contours, vector<vector <Point>>& detectedMarkers) {
		vector<Point> approxCurve;
		vector<vector <Point> > possibleMarkers;

		for (size_t i = 0; i < contours.size(); i++) {
			/* 다각형 근사화*/
			double eps = contours[i].size() * approxDgree;
			
			approxPolyDP(contours[i], approxCurve, eps, true);

			/* 사각형 검사*/
			if (approxCurve.size() != 4)
				continue;

			/* 볼록 여부 검사*/
			if (!isContourConvex(approxCurve))
				continue;

			/* 최소 거리 측정 및 비교 */
			float minDist = numeric_limits<float>::max();

			for (int i = 0; i < 4; i++)
			{
				Point side = approxCurve[i] - approxCurve[(i + 1) % 4];
				float squaredSideLegth = (float)side.dot(side);
				minDist = min(minDist, squaredSideLegth);
			}

			if (minDist < minContourLengthAllowd)
				continue;

			/* 후보 저장 */
			vector<Point> m;
			for (int i = 0; i < 4; i++)
				m.push_back(Point(approxCurve[i].x, approxCurve[i].y));

			/* 후보 시계 방향 정렬*/
			Point v1 = m[1] - m[0];
			Point v2 = m[2] - m[0];

			double o = (v1.x * v2.y) - (v1.y * v2.x);

			if (o < 0.0)
				swap(m[1], m[3]);

			possibleMarkers.push_back(m);
		}

		for (size_t i = 0; i < possibleMarkers.size(); i++) {
			detectedMarkers.push_back(possibleMarkers[i]);
		}
	}

	void warpingCandidates(Mat gray, vector <vector<Point>> &detectedMarkers, vector<Mat>& canonicalMarkers) {

		for (int i = 0; i < detectedMarkers.size(); i++) {
			if (detectedMarkers[i].size() == 4) {
				Mat canonicalMarker;
				vector<Point>& marker = detectedMarkers[i];
				Point2f src[4];

				for (int j = 0; j < marker.size(); j++) {
					src[j] = marker[j];
				}

				float w1 = sqrt(pow(src[2].x - src[3].x, 2)
					+ pow(src[2].x - src[3].x, 2));
				float w2 = sqrt(pow(src[1].x - src[0].x, 2)
					+ pow(src[1].x - src[0].x, 2));

				float h1 = sqrt(pow(src[1].y - src[2].y, 2)
					+ pow(src[1].y - src[2].y, 2));
				float h2 = sqrt(pow(src[0].y - src[3].y, 2)
					+ pow(src[0].y - src[3].y, 2));

				float maxWidth = (w1 < w2) ? w1 : w2;
				float maxHeight = (h1 < h2) ? h1 : h2;
				
				Size_<float> markerSize = Size((int)maxWidth, (int)maxHeight);
				Point2f m_markerCorners2d[] = { Point2f(0,0), Point2f(maxWidth - 1, 0), Point2f(maxWidth - 1,maxHeight - 1), Point2f(0,maxHeight - 1) };

				Mat M = getPerspectiveTransform(src, m_markerCorners2d);
				warpPerspective(gray, canonicalMarker, M, markerSize);
				canonicalMarkers.push_back(canonicalMarker);
			}
		}
	}

};


class CarNoDetector : public Detector {
private:
	const double margin = 0.35;

public:
	CarNoDetector(int minContourPointsAllowed, int threshold1, int threshold2) 
		: Detector(minContourPointsAllowed, threshold1, threshold2) {
	}

	void function2(vector<Mat>& canonicalMarkers);

	Point highestPoint(vector<Point> contour) {
		int max = 0;
		int index_max;

		for (int i = 0; i < contour.size(); i++) {
			if (max < contour[i].y) {
				max = contour[i].y;
				index_max = i;
			}
		}
		return contour[index_max];
	}

	Point lowestPoint(vector<Point> contour) {
		int min = numeric_limits<int>::max();
		int index_min;

		for (int i = 0; i < contour.size(); i++) {
			if (min > contour[i].y) {
				min = contour[i].y;
				index_min = i;
			}
		}

		return contour[index_min];
	}

	Point leftPoint(vector<Point> contour) {
		int max = 0;
		int index_max;


		for (int i = 0; i < contour.size(); i++) {
			if (max < contour[i].x) {
				max = contour[i].x;
				index_max = i;
			}
		}
		return contour[index_max];
	}

	Point rightPoint(vector<Point> contour) {
		int min = numeric_limits<int>::max();
		int index_min;

		for (int i = 0; i < contour.size(); i++) {
			if (min > contour[i].x) {
				min = contour[i].x;
				index_min = i;
			}
		}

		return contour[index_min];
	}

};

void CarNoDetector::function2(vector<Mat>& canonicalMarkers) {
	for (int i = 0; i < canonicalMarkers.size(); i++) {
		Mat canonicalMarker = canonicalMarkers[i];
		Mat numbers;			// gray로 부터 추출한 숫자들
		performThreshold(canonicalMarker, numbers);

		ContoursVector contours;
		mfindContours(numbers, contours);

		typedef pair<int, int> Domain;
		priority_queue<Domain> pq;

		double height = canonicalMarker.size().height;
		double ratio = height * margin;
		int numbers_highest = numeric_limits<int>::max();
		int numbers_lowest = 0;

		//Mat contoursfound(numbers.size(), CV_8UC4, Scalar(255, 255, 255));
		//srand(time(0));		// 숫자 색 seed 값

		/* ----- 숫자 위치의 범위 측정 ----- */

		for (int j = 0; j < contours.size(); j++) {
			vector<Point> contour = contours[j];
			if (lowestPoint(contour).y > ratio) {
				if (highestPoint(contour).y < height - ratio) {
					//ContoursVector tmp;	tmp.push_back(contour);
					//drawContours(contoursfound, tmp, -1, Scalar(rand() % 255, rand() % 255, rand() % 255), 2);	// 숫자 출력 시 랜덤 색상으로 출력
					numbers_highest = min(highestPoint(contour).y, numbers_highest);
					numbers_lowest = max(lowestPoint(contour).y, numbers_lowest);
					pq.push(make_pair(leftPoint(contour).x, rightPoint(contour).x));
				}
			}
		}

		/* ----- 범위 정렬 ----- */
		vector<Domain> domains;		// 범위의 집합
		Domain pre, own;
		while (!pq.empty()) {
			own = pq.top();
			pq.pop();
			domains.push_back(own);
		}

		for (int j = 0; j < domains.size(); j++) {
			int flag = (int)domains.size();
			int flag2 = 0;
			for (int k = j; k > 0; k--) {
				if (domains[j].first >= domains[k - 1].second) {
					flag2 = 1;
					flag = min(k - 1, flag);
				}
			}
			if (flag2) {
				domains[j].first = domains[flag].first;
				domains[j].second = min(domains[j].second, domains[flag].second);
				domains.erase(domains.begin() + j - 1, domains.begin() + flag + 1);
				j = flag;
			}
		}

		/*  ------  인식된 숫자를 사각형으로 그리기 ----- */
		for (int j = 0; j < domains.size(); j++) {
			//rectangle(numbers, Point2d(domains[j].first, ratio), Point2d(domains[j].second, height - ratio), Scalar(255), 2);
			rectangle(canonicalMarker, Point2d(domains[j].first, ratio), Point2d(domains[j].second, height - ratio), Scalar(255), 2);
		}

		//imshow("canoicalMarker", numbers);
		imshow("canoicalMarker", canonicalMarker);
	}
}

#ifdef _WIN32
namespace raspicam {
	class RaspiCam_Cv {
	private:
		VideoCapture camera;
	public:
		RaspiCam_Cv() {

		}
		void set(int a, int b) {
		}

		void open() {
			camera.open(0);
		}
		void grab() {
		}
		void retrieve(Mat *cameraFrame) {
			camera >> *cameraFrame;
		}
	};
};
#endif

int m3ain1()
{
	raspicam::RaspiCam_Cv Camera;

	Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	Camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	Camera.open();

	MarkerDetector *arg1 = new MarkerDetector(100, 1, 50, 100, 0.05);

	while (true) {
		Mat cameraFrame;
		Camera.grab();
		Camera.retrieve(&cameraFrame);
		if (cameraFrame.empty()) {
			cerr << "ERROR: Couldn't grab a camera frame." << endl;
			exit(1);
		}

		Mat gray;
		arg1->prepareImage(cameraFrame, gray);

		Mat thresholded;
		arg1->performThreshold(gray, thresholded);

		ContoursVector contours;
		arg1->mfindContours(thresholded, contours);

		Mat contoursfound(thresholded.size(), CV_8U, Scalar(255));
		drawContours(contoursfound, contours, -1, Scalar(1), 2);

		vector<vector <Point >> detectedMarkers;
		arg1->findCandidates(contours, detectedMarkers);

		vector<Mat> canonicalMarkers;
		arg1->warpingCandidates(gray, detectedMarkers, canonicalMarkers);

		/*Mat result1(thresholded.size(), CV_8U, Scalar(255));
		drawContours(result1, detectedMarkers, -1, Scalar(1), 2);*/

		/* 표지판에서의 처리 */
		CarNoDetector *cnd = new CarNoDetector(1, 50, 100);
		cnd->function2(canonicalMarkers);

		// grayscale에  감지된 마커를 사각형으로 출력
		Detector::drawMarkerRect(gray, detectedMarkers);
		imshow("Grayscale", gray);

		int key = waitKey(10);
		if (key == 0)
			break;
	}
}