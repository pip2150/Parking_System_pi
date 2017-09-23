#ifndef SVM_HPP_
#define SVM_HPP_

#include <opencv2/opencv.hpp>

class Svm {
private:
	cv::Ptr<cv::ml::SVM> svm;
	cv::Mat classes;
	cv::Mat trainingData;

public:
	Svm();
	Svm(int flags);
	void collectTrainImages();
	float predict(cv::Mat &img);
	void readTraindata(std::string fn);
	void writeTraindata(std::string fn);
};

#endif