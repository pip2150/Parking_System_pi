#ifndef SVM_HPP_
#define SVM_HPP_

#include <opencv2/opencv.hpp>

/** Support Vector Machines을 다루기 위한 클래스
*/
class Svm {
private:

	/** Support Vector Machines
	*/
	cv::Ptr<cv::ml::SVM> svm;

	/** trainingData와 연관된 출력될 vectors
	*/
	cv::Mat classes;

	/** 훈련을 위한 Samples
	*/
	cv::Mat trainingData;

public:

	/** Svm 초기화
	*/
	Svm(const int flags);

	/** trainimage에서 훈련 데이터 불러오기
	*/
	void collectTrainImages();

	/** 훈련 데이터를 File System에 json 파일로 쓰기
	*/
	void writeTraindata(const std::string fn);

	/** 훈련 데이터를 File System에서 json 파일로부터 불러오기
	*/
	void readTraindata(const std::string fn);

	/** 입력된 Sample이 훈련된 Support Vector Machines의해 예측된 결과를 출력
	*/
	float predict(const cv::Mat &img);
};

#endif