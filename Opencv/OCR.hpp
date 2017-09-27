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
	
	void readTraindata(const std::string fn);
	void readTraindata(const std::string fn, const int format);
	void writeTraindata(const std::string fn);
	void collectTrainImages();

	static cv::Mat getHistogram(const cv::Mat &img, const int t);
public:
	OCR(const int format, const int flags);
	int numCharacters;	
	
	float predict(const cv::Mat &img);
	float predict(const cv::Mat &img, cv::Mat *out);
	char classify(cv::Mat *output);
	
	static cv::Mat features(const cv::Mat &numbers, const int sizeData);
};

#endif