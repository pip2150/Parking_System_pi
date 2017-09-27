#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <opencv2/opencv.hpp>
#include "OCR.hpp"

//* 최소한의 일치 횟수
#define LEASTMATCH 5

namespace tools {

	/** File System으로 부터 Image 불러오기
		성공 시 0, 실패 시 1을 반환
	*/
	int readImage(const std::string fn, cv::Mat& image, int mode = 1);

	/** File System으로 Image를 쓰기
		성공 시 0, 실패 시 1을 반환
	*/
	int writeImage(const std::string fn, const cv::Mat& image, int mode = 1);

	/** 최종 결과를 도출하는 클래스
	*/
	class Dicider {
	private:

		/** 비교할 문자열
		*/
		std::string keyStr;

		/** 일치한 횟수
		*/
		int match;

	public:

		/** Dicider 초기화
		*/
		Dicider();

		/** 연속으로 일정 횟수까지 같은 문자열을 입력받으면 최종 결과를 출력
		*/
		bool decide(std::string str);
	};

	/** 통계적 계산을 출력하는 클래스
	*/
	class Analyzer {
	private:

		/** 총 맞은 횟수
		*/
		int totalCorrect;

		/** 총 시도한 횟수
		*/
		int totalTry;

		/** 비교할 정답
		*/
		std::string answer;

	public:

		/** Analyzer 초기화
		*/
		Analyzer(const std::string answer);

		/** 정답과 비교하며 통계적 계산 결과를 출력
		*/
		void analyze(const std::string str);
	};

	/** SVM 클래스의 collectTrainImages 시 필요한 image를 생성하는 클래스
	*/
	class SVMTrainer {
	private:

		/** File 검색을 위한 Index 변수
		*/
		int fileIndex;

	public:

		/** SVMTrainer 초기화
		*/
		SVMTrainer();

		/** 입력받은 Mat 변수를 TrainImage로써 File System에 기록
		*/
		void train(const cv::Mat &sample);
	};

	/** OCR 클래스의 collectTrainImages 시 필요한 image를 생성하는 클래스
	*/
	class OCRTrainer {
	private:

		/** File 검색을 위한 Index 변수
		*/
		int fileIndexs[NUMBER + CHARACTER];

		/** 지도학습을 위한 정답
		*/
		std::string answer;

	public:

		/** OCRTrainer 초기화
		*/
		OCRTrainer(const std::string answer);

		/** 입력받은 Mat 변수들을 TrainImage로써 File System에 기록
		*/
		void train(const std::vector<cv::Mat> &sample);
	};
};


#endif
