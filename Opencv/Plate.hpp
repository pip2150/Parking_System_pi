#ifndef PLATE_HPP_
#define PLATE_HPP_

#include <opencv2/opencv.hpp>

/** ��ȣ��
*/
class Plate {
private:

	/** debug ����
	*/
	bool debug;

	/** ��ħ �˻�
	*/
	bool isOverlap(const cv::Rect &A, const cv::Rect &B);

	/** �ٻ� �ּҿ��� �簢��
	*/
	static cv::RotatedRect minApproxRect(const std::vector<cv::Point> &contour);

	/** ũ�� �˻�
	*/
	static bool verifySizes(const cv::RotatedRect &mr);

public:

	/** �ؽ�Ʈ
	*/
	class Text {
	public:

		/** ���ڸ� ��Ÿ���� ��Ʈ����
		*/
		cv::Mat img;

		/** Text �ʱ�ȭ
		*/
		Text(const cv::Mat &src);

		/** Text ���� ����ȭ
		*/
		cv::Mat canonical(int sampleSize);
	};

	/** Plate�� ��Ÿ���� ��Ʈ����
	*/
	cv::Mat img;

	/** ���� �̹����� ���� ������� Plate�� ��ġ
	*/
	cv::Point position;

	/** Plate�� ���Ե� Texts
	*/
	std::vector<Text> texts;

	/** Plate �ʱ�ȭ
	*/
	Plate(const cv::Mat &img, const cv::Point &position);

	/** debug ��� ����
	*/
	void setDebug(bool debug);

	/** Plate���� Text ���� ����
	*/
	bool findTexts(const int numSize);

	/** �Է� image�� ���� Plate ����
	*/
	static void find(const cv::Mat &input, std::vector<Plate> *PossiblePlates);

	/** �Է� image�� RotatedRect �׸���
	*/
	static void drawRotatedRect(const cv::Mat &img, const cv::RotatedRect &roRec, const cv::Scalar color, int thickness = 1);

	/** Plate ����ȭ
	*/
	cv::Mat canonical();

};

#endif
