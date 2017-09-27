#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>

/** 번호판
*/
class Plate {
private:

	/** debug 변수
	*/
	bool debug;

	/** 겹침 검사
	*/
	bool isOverlap(const cv::Rect &A, const cv::Rect &B);

	/** 근사 최소영역 사각형
	*/
	static cv::RotatedRect minApproxRect(const std::vector<cv::Point> &contour);

	/** 크기 검사
	*/
	static bool verifySizes(const cv::RotatedRect &mr);

public:

	/** 텍스트
	*/
	class Text {
	public:

		/** 숫자를 나타내는 매트릭스
		*/
		cv::Mat img;

		/** Text 초기화
		*/
		Text(const cv::Mat &src);

		/** Text 영역 정규화
		*/
		cv::Mat canonical(int sampleSize);
	};

	/** Plate를 나타내는 매트릭스
	*/
	cv::Mat img;

	/** 원본 이미지에 대한 상대적인 Plate의 위치
	*/
	cv::Point position;

	/** Plate에 포함된 Texts
	*/
	std::vector<Text> texts;

	/** Plate 초기화
	*/
	Plate(const cv::Mat &img, const cv::Point &position);

	/** debug 모드 설정
	*/
	void setDebug(bool debug);

	/** Plate에서 Text 영역 추출
	*/
	bool findTexts(const int numSize);

	/** 입력 image로 부터 Plate 추출
	*/
	static void find(const cv::Mat &input, std::vector<Plate> *PossiblePlates);

	/** 입력 image에 RotatedRect 그리기
	*/
	static void drawRotatedRect(const cv::Mat &img, const cv::RotatedRect &roRec, const cv::Scalar color, int thickness = 1);

	/** Plate 정규화
	*/
	cv::Mat canonical();

};

#endif
