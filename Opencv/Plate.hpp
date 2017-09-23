#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>

class Plate {
private:
	bool isOverlap(cv::Rect &A, cv::Rect &B);
	bool debug;
	static cv::RotatedRect minApproxRect(std::vector<cv::Point> &contour);
	static bool verifySizes(cv::RotatedRect &mr);

public:
	Plate();
	Plate(cv::Mat& binarizatied);
	Plate(cv::Mat &img, cv::Point &position);
	void setDebug(bool debug);

	cv::Mat img;
	cv::Mat binarizatied;
	cv::Mat canonical;
	cv::Point position;

	class Number {
	public:
		Number(cv::Mat &src);
		cv::Mat img;
		cv::Mat canonical;
		void canonicalize(int sampleSize);
	};
	std::vector<Number> numbers;
	bool findNumbers(int number);

	static void find(cv::Mat &input, std::vector<Plate> &PossiblePlates);
	static void drawRotatedRect(cv::Mat img, cv::RotatedRect roRec, const cv::Scalar color, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);
	void canonicalize();

};

#endif
