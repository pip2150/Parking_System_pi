#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <opencv2/opencv.hpp>

/**
	@brief tools
	영상 처리 시 유용한 tool 모음
*/
namespace tools {
	
	/**
		@brief File System으로 부터 이미지 불러오기
		@param fn 이미지 파일 경로
		@param image File System에서 불러온 이미지를 저장할 변수
		@param flags cv::ImreadModes 의 값을 지정
		@return 성공 시 true 그렇지 않으면 false
	*/
	bool readImage(const std::string fn, cv::Mat& image, int flags = 1);

	/**
		@brief File System으로 Image를 쓰기
		@param fn 이미지 파일 경로
		@param image File System에 쓸 이미지 변수
		@return 성공 시 true 그렇지 않으면 false
	*/
	bool writeImage(const std::string fn, const cv::Mat& image);

	/**
		@brief 초기에 입력된 문자열과 비교하여 tools::Dicider::LEASTMATCH 를 넘었을 경우 결과를 도출한다.
	*/
	class Dicider {
	private:
		
		std::string keyStr;		//!< @brief 초기 문자열
		int match;				//!< @brief 일치한 횟수

	public:

		static const int LEASTMATCH = 5;		//!< @brief 최소한의 일치 횟수

		/**
			@brief Dicider 초기화
			비교할 문자열을 설정한다.
		*/
		Dicider();

		/**
			@brief 연속으로 일정 횟수까지 같은 문자열을 입력받으면 최종 결과를 출력
			@param str 비교 문자열
			@return  str과 초기 문자열이 LEASTMATCH 의 횟수 이상 일치할 경우 true 그렇지 않으면 false 을 반환
		*/
		bool decide(std::string str);
	};

	/**
		@brief 통계 분석 결과 출력 클래스
		입력된 초기 정답 문자열를 영상으로부터 도출된 문자열과 통계적 비교된 결과를 출력한다.
	*/
	class Analyzer {
	private:

		int totalCorrect;		//!< @brief 총 맞은 횟수
		int totalTry;			//!< @brief 총 시도한 횟수
		std::string answer;		//!< @brief 비교할 정답

	public:

		/**
			@brief Analyzer 초기화
			@param answer 정답을 설정
		*/
		Analyzer(const std::string answer);

		/**
			@brief 입력받은 문자열을 정답과 비교하며 통계적 계산 결과를 출력
			@param str 정답과 비교할 문자열
		*/
		void analyze(const std::string str);
	};

};

#endif
