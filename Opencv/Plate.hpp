#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define WINDOW_X 650
#define WINDOW_Y 300

class Plate {
private:
	enum { UP, DOWN, LEFT, RIGHT };
	void endPoint(std::vector<cv::Point> &contour, cv::Point mPoint[4]);
	bool isOverlap(cv::Rect &A, cv::Rect &B);

public:
	Plate();
	Plate(cv::Mat& binarizatied);

	cv::Mat img;
	cv::Mat binarizatied;
	cv::Mat canonical;

	class Number {
	public:
		Number(cv::Mat &src);
		cv::Mat img;
		cv::Mat canonical;
		void canonicalize(int sampleSize);
	};
	std::vector<Number> numbers;
	void findNumbers();

	static void find(cv::Mat &input, std::vector<Plate> &PossiblePlates, std::vector<cv::Point> &PlatePositions);
	static bool verifySizes(cv::RotatedRect &mr);	
	void canonicalize();

};

#endif
