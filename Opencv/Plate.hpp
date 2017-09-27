#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>

class Plate {
private:
	bool isOverlap(const cv::Rect &A, const cv::Rect &B);
	bool debug;
	static cv::RotatedRect minApproxRect(const std::vector<cv::Point> &contour);
	static bool verifySizes(const cv::RotatedRect &mr);

public:
	Plate();
	Plate(const cv::Mat& binarizatied);
	Plate(const cv::Mat &img, const cv::Point &position);
	void setDebug(bool debug);

	cv::Mat img;
	cv::Mat binarizatied;
	cv::Mat canonical;
	cv::Point position;

	class Number {
	public:
		Number(const cv::Mat &src);
		cv::Mat img;
		cv::Mat canonical;
		void canonicalize(int sampleSize);
	};
	std::vector<Number> numbers;
	bool findNumbers(const int number);

	static void find(const cv::Mat &input, std::vector<Plate> *PossiblePlates);
	static void drawRotatedRect(const cv::Mat &img, const cv::RotatedRect &roRec, const cv::Scalar color, int thickness = 1, int lineType = cv::LINE_8, int shift = 0);
	void canonicalize();

};

#endif
