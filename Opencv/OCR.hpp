#ifndef OCR_HPP_
#define OCR_HPP_

#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>

#define SAMPLESIZE 20
#define VERTICAL 0
#define HORIZONTAL 1
#define CHARACTER 13
#define NUMBER 10
#define NLAYERS 30

class OCR {
private:
	
	cv::Ptr<cv::ml::ANN_MLP> ann;
	cv::Mat classes;
	cv::Mat trainingData;
	std::string strCharacters = "0123456789BCDEFGNSTVWXY";

public:
	OCR();
	OCR(int format);
	int numCharacters;	
	cv::Mat getHistogram(cv::Mat &img, int t);
	cv::Mat features(cv::Mat &numbers, int sizeData);
	void train(int nlayers);
	void collectTrainImages();
	float predict(cv::Mat &img);
	float predict(cv::Mat &img, cv::Mat &out);
	void readTraindata(std::string fn);
	void readTraindata(std::string fn, int format);
	void writeTraindata(std::string fn);
	char classify(cv::Mat &output);
};

#endif