#ifndef UTILS_HPP_
#define UTILS_HPP_

#include "opencv2/opencv.hpp"

namespace utils{
	int readImage(std::string fn, cv::Mat& image, int mode = 1);
	int writeImage(std::string fn, cv::Mat& image, int mode = 1);
};

#endif