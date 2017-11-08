#ifndef OCR_HPP_
#define OCR_HPP_

#include <opencv2/opencv.hpp>


#define SAMPLESIZE 20		//!< 훈련 데이터의 가로, 세로의 픽셀 크기
#define NLAYERS 30			//!< OCR의 은닉 층
#define TEXTSIZE 7			//!< 번호판의 숫자 수

//! OCR이 인식할 형식
enum FORMAT {
	CHARACTER = 13,		//!< OCR에 문자의 갯수
	NUMBER = 10			//!< OCR에 숫자의 갯수
};

/** 
	@brief Artificial Neural Networks을 다루기 위한 클래스
*/
class OCR {
private:

	//! Histogram 추출 시의 방향
	enum ORIENTATION {
		VERTICAL = 0,		//!< 수직
		HORIZONTAL = 1		//!< 수평
	};

	cv::Ptr<cv::ml::ANN_MLP> ann;		//!< @brief Artificial Neural Networks
	cv::Mat classes;					//!< @brief trainingData와 연관된 출력될 vector들
	cv::Mat trainingData;				//!< @brief 훈련을 위한 Sample들
	static std::string strCharacters; /*= "0123456789BCDEFGNSTVWXY";*/		//!< @brief classes와 대응되는 문자열

	/**
		@brief 출력 문자의 인덱스 번호 계산
		@param c 찾을 문자
		@param format 포맷
		@return 출력 문자의 인덱스
	*/
	static int strIndex(char c, FORMAT format);

	/** 
		@brief TrainNumber에서 훈련 데이터 불러오기
		'TrainNumber'에 경로에서 모든 이미지 파일을 읽어들여 훈련 데이터를 생성한다.
	*/
	void collectTrainImages();

	/** 
		@brief 훈련 데이터를 File System에 Json 파일로 쓰기
		@param fn josn 파일의 경로
	*/
	void writeTraindata(const std::string fn);

	/** 
		@overload
		@brief 훈련 데이터를 File System에서 Json 파일로부터 불러오기
		@param fn Josn 파일의 경로
	*/
	void readTraindata(const std::string fn);

	/**
		@overload
		@brief 훈련 데이터를 File System에서 Json 파일로부터 불러오기
		@param fn Josn 파일의 경로
		@param format Json OCR이 인식할 형식
	*/
	void readTraindata(const std::string fn, const FORMAT format);

	/** 
		@brief Histogram 추출
		@param img Histogram을 추출할 이미지		
		@param t Histogram 추출 시의 방향
		@return 추출된 Histogram의 매트릭스
	*/
	static cv::Mat getHistogram(const cv::Mat &img, const ORIENTATION t);

public:

	//! 훈련 데이터를 다룰 방법 
	enum MODE {
		READDT = 0b00,		//!< Json File에서 읽기 @see readTraindata
		COLLECT = 0b01,		//!< TrainImage에서 읽기 @see collectTrainImages
		WRITEDT = 0b11		//!< Json File로 쓰기 @see writeTraindata
	};

	int numCharacters;		//!< @brief 출력될 vector의 크기

	/** 
		@brief OCR 초기화
		@param format OCR이 인식할 형식
		@param mode 훈련 데이터를 다룰 방법
	*/
	OCR(const FORMAT format, const int mode);

	/** 
		@brief 입력된 Sample이 훈련된 Artificial Neural Networks의해 예측된 결과를 출력
		@param img 입력할 Sample
		@param out 예측된 결과의 매트릭스
		@return OCR을 통해 예측된 결과
	*/
	float predict(const cv::Mat &img, cv::Mat *out);

	/** 
		@brief 예측된 결과 중 가장 가능성이 높은 문자를 추출
		@param output 문자를 추출할 결과 매트릭스
		@return 가능성 높은 문자
	*/
	char classify(cv::Mat &output);

	/**
		@brief 기존의 번호판 중 가장 가능성 높은 번호판의 인덱스를 출력
		@param outputs 가능성을 계산할 매트릭스
		@param list 가능성을 비교할 번호판의 리스트
		@return 가능성 높은 번호판의 인덱스
	*/
	static int maxProb(std::vector<cv::Mat> &outputs, std::vector<std::string> &list);

	/** 
		@brief features 추출
		@param texts features을 추출할 이미지
		@param sizeData 추출될 features의 가로 세로 크기
		@return 입력받은 이미지에 대한 features
	*/
	static cv::Mat features(const cv::Mat &texts, const int sizeData);
};

/** 
	@brief OCR::collectTrainImages 실행 시 필요한 image를 생성하는 클래스
*/
class OCRTrainer {
private:

	int fileIndexs[NUMBER + CHARACTER];		//!< @brief File 검색을 위한 Index 변수
	std::string answer;						//!< @brief 지도학습을 위한 정답

public:

	/**
		@brief OCRTrainer 초기화
		@param answer 지도 학습을 위한 정답
	*/
	OCRTrainer(const std::string answer);

	/**
		@brief OCR 클래스에서 참조할 이미지를 File System에 쓰기
		입력받은 이미지를 'TrainImage'경로에 File System로 쓴다.
	*/
	void train(const std::vector<cv::Mat> &sample);
};

#endif
