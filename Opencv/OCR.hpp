#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>

#define SAMPLESIZE 20
#define VERTICAL 0
#define HORIZONTAL 1

class OCR {
public:
	static const int OCR::numCharacters = 23;
	int recNum;
private:
	cv::Ptr<cv::ml::ANN_MLP> ann;
	cv::Mat classes;
	cv::Mat trainingData;
	static const char OCR::strCharacters[numCharacters];

public:
	OCR();
	cv::Mat projectedHistogram(cv::Mat &img, int t);
	cv::Mat features(cv::Mat &numbers, int sizeData);
	void train(int nlayers);
	void collectTrainImages();
	float predict(cv::Mat &img);
	float predict(cv::Mat &img, cv::Mat &out);
	void readTraindata(std::string fn);
	void writeTraindata(std::string fn);
	char classify(cv::Mat &output);
};