#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include "opencv2/opencv.hpp"

namespace tools{
	int readImage(std::string fn, cv::Mat& image, int mode = 1);
	int writeImage(std::string fn, cv::Mat& image, int mode = 1);
};

#endif
