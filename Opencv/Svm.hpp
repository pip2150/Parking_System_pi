#ifndef SVM_HPP_
#define SVM_HPP_

#include <opencv2/opencv.hpp>

class Svm {
private:
	cv::Ptr<cv::ml::SVM> svm;
	cv::Mat classes;
	cv::Mat trainingData;

public:
	Svm(const int flags);
	void collectTrainImages();
	float predict(const cv::Mat &img);
	void readTraindata(const std::string fn);
	void writeTraindata(const std::string fn);
};

#endif