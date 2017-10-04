#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>

/**
	@brief 번호판
	번호판 이미지와 상대적 위치를 포함한 클래스
*/
class Plate {
private:

	bool debug;		//!< @brief debug 변수

	/** 
		@brief 겹침 검사
		@param A 겹침 검사를 할 사각형1
		@param B 겹침 검사를 할 사각형2
		@return A와 B가 겹치는지 여부를 출력. 겹치면 true 그렇지 않으면 false
	*/
	bool isOverlap(const cv::Rect &A, const cv::Rect &B);

	/** 
		@brief 근사 최소영역 사각형
		@param contour 근사 최소영역 사각형을 구하기 위한 원본 Contour
		@return 근사화 후의 Contour 들을 포함하는 최소 넓이의 사각형
	*/
	static cv::RotatedRect minApproxRect(const std::vector<cv::Point> &contour);

	/** 
		@brief 크기 검사
		@param mr 크기 검사를 할 RotatedRect
		@return 적합한 크기의 경우 true 그렇지 않으면 false
	*/
	static bool verifySizes(const cv::RotatedRect &mr);

public:

	/** 
		@brief 텍스트
		번호판을 구성하는 텍스트들 중 하나
	*/
	class Text {
	public:

		cv::Mat img;		//!< @brief 텍스트를 나타내는 이미지

		/** 
			@brief Text의 이미지 초기화
			@param src 초기화 할 이미지
		*/
		Text(const cv::Mat &src);

		/** 
			@brief img를 정규화한 이미지 출력
			@param sampleSize 정규화하기 위한 가로 세로의 크기
			@return 정규화된 이미지
		*/
		cv::Mat canonical(int sampleSize);
	};

	cv::Mat img;					//!< @brief Plate를 나타내는 매트릭스
	cv::Point position;				//!< @brief 원본 이미지에 대한 상대적인 Plate의 위치
	std::vector<Text> texts;		//!< @brief Plate에 포함된 Texts

	/** 
		@brief Plate 초기화		
		@param img Plate를 나타내는 매트릭스
		@param position 원본 이미지에 대한 상대적인 Plate의 위치
		원본 이미지는 Plate::find 의 매개변수 input이다. @see find
	*/
	Plate(const cv::Mat &img, const cv::Point &position);

	/** 
		@brief debug 모드 설정
		@param debug 중이면 true 그렇지 않으면 false
		중간 처리 과정을 창으로 띄울지 여부를 정한다.
	*/
	void setDebug(bool debug);

	/** 
		@brief Plate에서 Text 이미지 추출
		@param textSize Plate를 구성하는 Text 들의 갯수를 설정
		@return Plate에서 추출한 Text의 갯수가 textSize와 같을 시 true 그렇지 않으면 false
	*/
	bool findTexts(const int textSize);

	/** 
		@brief 입력받은 이미지로 부터 Plate 추출
		@param input Plate을 찾을 원본 이미지
		@param PossiblePlates 원본 이미지로 부터 찾은 Plate
	*/
	static void find(const cv::Mat &input, std::vector<Plate> *PossiblePlates);

	/** 
		@brief 입력받은 이미지에 RotatedRect 그리기
		@param img 입력받은 RotatedRect를 그리는 이미지
		@param roRec 그려야하는 RotatedRect
		@param color 그리는 RotatedRect의 색
		@param thickness 그리는 RotatedRect의 두께
	*/
	static void drawRotatedRect(const cv::Mat &img, const cv::RotatedRect &roRec, const cv::Scalar color, int thickness = 1);

	/** 
		@brief img를 정규화한 이미지 출력
		@return 정규화된 이미지
	*/
	cv::Mat canonical();

};

#endif
