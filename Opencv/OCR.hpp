#ifndef OCR_HPP_
#define OCR_HPP_

#include <opencv2/opencv.hpp>

//* 훈련 데이터의 가로, 세로의 픽셀 크기
#define SAMPLESIZE 20

//* 수직
#define VERTICAL 0

//* 수평
#define HORIZONTAL 1

//* OCR에 문자의 갯수
#define CHARACTER 13

//* OCR에 숫자의 갯수
#define NUMBER 10

//* OCR의 은닉 층
#define NLAYERS 30

//* Json File에서 읽기
#define READDT 0b00

//* trainimage에서 읽기
#define COLLECT 0b01

//* Json File로 쓰기
#define WRITEDT 0b11

//* 번호판의 숫자 수
#define TEXTSIZE 7

/** Artificial Neural Networks을 다루기 위한 클래스
*/
class OCR {
private:

	/** Artificial Neural Networks
	*/
	cv::Ptr<cv::ml::ANN_MLP> ann;

	/** trainingData와 연관된 출력될 vector들
	*/
	cv::Mat classes;

	/** 훈련을 위한 Sample들
	*/
	cv::Mat trainingData;

	/** classes와 대응되는 문자열
	*/
	std::string strCharacters = "0123456789BCDEFGNSTVWXY";

	/** trainimage에서 훈련 데이터 불러오기
	*/
	void collectTrainImages();

	/** 훈련 데이터를 File System에 json 파일로 쓰기
	*/
	void writeTraindata(const std::string fn);

	/** 훈련 데이터를 File System에 json 파일로부터 불러오기
	*/
	void readTraindata(const std::string fn);

	/** 훈련 데이터를 File System에서 json 파일로부터 불러오기
	*/
	void readTraindata(const std::string fn, const int format);

	/** Histogram 추출
	*/
	static cv::Mat getHistogram(const cv::Mat &img, const int t);
public:

	/** 출력될 vector의 크기
	*/
	int numCharacters;

	/** OCR 초기화
	*/
	OCR(const int format, const int flags);

	/** 입력된 Sample이 훈련된 Artificial Neural Networks의해 예측된 결과를 출력
	*/
	float predict(const cv::Mat &img, cv::Mat *out);

	/** 예측된 결과 중 가장 가능성이 높은 문자를 추출
	*/
	char classify(cv::Mat *output);

	/** features 추출
	*/
	static cv::Mat features(const cv::Mat &numbers, const int sizeData);
};

#endif