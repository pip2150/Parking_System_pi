#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>

class Svm {
private:
	cv::Ptr<cv::ml::SVM> svm;
	cv::Mat classes;
	cv::Mat trainingData;
public:
	void train();
	void collectTrainImages();
	float predict(cv::Mat img);
	void readTraindata(std::string fn);
	void writeTraindata(std::string fn);
};