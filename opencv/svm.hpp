#ifndef SVM_HPP_
#define SVM_HPP_

#include <opencv2/opencv.hpp>

/**
	@brief Support Vector Machines을 다루기 위한 클래스
*/

class Svm {
private:

	cv::Ptr<cv::ml::SVM> svm;		//!< @brief Support Vector Machines
	cv::Mat classes;				//!< @brief trainingData와 연관된 출력될 vectors
	cv::Mat trainingData;			//!< @brief 훈련을 위한 Samples

	/**
		@brief trainimage에서 훈련 데이터 불러오기
		@param index 경계 인덱스 
		'trainimage'에 경로에서 모든 이미지 파일을 읽어들여 훈련 데이터를 생성한다.
	*/
	void collectTrainImages(int index);

	/**
		@brief 훈련 데이터를 File System에 Json 파일로 쓰기
		@param fn Josn 파일의 경로
	*/
	void writeTraindata(const std::string fn);

	/** 
		@brief 훈련 데이터를 File System에서 Json 파일로부터 불러오기
		@param fn Josn 파일의 경로
	*/
	void readTraindata(const std::string fn);

public:

	//! 훈련 데이터를 다룰 방법 
	enum MODE {
		READDT = 0b00,		//!< Json File에서 읽기 @see readTraindata
		COLLECT = 0b01,		//!< trainimage에서 읽기 @see collectTrainImages
		WRITEDT = 0b11		//!< Json File로 쓰기 @see writeTraindata
	};

	/**
		@brief Svm 초기화
		@param mode Svm 모드 설정 @see MODE
	*/
	Svm(const int mode);

	/**
		@brief 입력된 Sample이 훈련된 Support Vector Machines의해 예측된 결과를 출력
		@param img 입력할 Sample
		@return Svm을 통해 0 ~ 1로 분류된 결과
	*/
	float predict(const cv::Mat &img);
};

/**
	@brief Svm::collectTrainImages 실행 시 필요한 image를 생성하는 클래스
*/
class SVMTrainer {
private:

	int fileIndex;		//!< @brief File 검색을 위한 Index 변수

public:

	/**
		@brief SVMTrainer 초기화
	*/
	SVMTrainer();

	/** 
		@brief Svm 클래스에서 참조할 이미지를 File System에 쓰기
		@param sample File System에 쓸 이미지
		입력받은 이미지를 'TrainImage'경로에 File System로 쓴다.
	*/
	void train(const cv::Mat &sample);
};

#endif
