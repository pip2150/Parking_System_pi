#ifndef OCR_HPP_
#define OCR_HPP_

#include <opencv2/opencv.hpp>

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

	void readTraindata(std::string fn);
	void readTraindata(std::string fn, int format);
	void writeTraindata(std::string fn);
	void collectTrainImages();
public:
	OCR(int format, int flags);
	int numCharacters;	
	cv::Mat getHistogram(cv::Mat &img, int t);
	cv::Mat features(cv::Mat &numbers, int sizeData);
	float predict(cv::Mat &img);
	float predict(cv::Mat &img, cv::Mat &out);
	char classify(cv::Mat &output);
};

#endif