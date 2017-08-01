#include "opencv2/opencv.hpp"

namespace utils{
	void drawRotatedRect(cv::Mat &img, cv::RotatedRect &mr, cv::Scalar color = cv::Scalar(100, 100, 200), int thickness = 2);
	bool isOverlap(cv::Rect &A, cv::Rect &B);
	int readImage(std::string fn, cv::Mat& image, int mode = 1);
	int writeImage(std::string fn, cv::Mat& image, int mode = 1);
    template <typename T>
    static std::string to_string(T input);
};
