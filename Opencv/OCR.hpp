#ifndef OCR_HPP_
#define OCR_HPP_

#include <opencv2/opencv.hpp>

//* �Ʒ� �������� ����, ������ �ȼ� ũ��
#define SAMPLESIZE 20

//* ����
#define VERTICAL 0

//* ����
#define HORIZONTAL 1

//* OCR�� ������ ����
#define CHARACTER 13

//* OCR�� ������ ����
#define NUMBER 10

//* OCR�� ���� ��
#define NLAYERS 30

//* Json File���� �б�
#define READDT 0b00

//* trainimage���� �б�
#define COLLECT 0b01

//* Json File�� ����
#define WRITEDT 0b11

//* ��ȣ���� ���� ��
#define TEXTSIZE 7

/** Artificial Neural Networks�� �ٷ�� ���� Ŭ����
*/
class OCR {
private:

	/** Artificial Neural Networks
	*/
	cv::Ptr<cv::ml::ANN_MLP> ann;

	/** trainingData�� ������ ��µ� vector��
	*/
	cv::Mat classes;

	/** �Ʒ��� ���� Sample��
	*/
	cv::Mat trainingData;

	/** classes�� �����Ǵ� ���ڿ�
	*/
	std::string strCharacters = "0123456789BCDEFGNSTVWXY";

	/** trainimage���� �Ʒ� ������ �ҷ�����
	*/
	void collectTrainImages();

	/** �Ʒ� �����͸� File System�� json ���Ϸ� ����
	*/
	void writeTraindata(const std::string fn);

	/** �Ʒ� �����͸� File System�� json ���Ϸκ��� �ҷ�����
	*/
	void readTraindata(const std::string fn);

	/** �Ʒ� �����͸� File System���� json ���Ϸκ��� �ҷ�����
	*/
	void readTraindata(const std::string fn, const int format);

	/** Histogram ����
	*/
	static cv::Mat getHistogram(const cv::Mat &img, const int t);
public:

	/** ��µ� vector�� ũ��
	*/
	int numCharacters;

	/** OCR �ʱ�ȭ
	*/
	OCR(const int format, const int flags);

	/** �Էµ� Sample�� �Ʒõ� Artificial Neural Networks���� ������ ����� ���
	*/
	float predict(const cv::Mat &img, cv::Mat *out);

	/** ������ ��� �� ���� ���ɼ��� ���� ���ڸ� ����
	*/
	char classify(cv::Mat *output);

	/** features ����
	*/
	static cv::Mat features(const cv::Mat &numbers, const int sizeData);
};

#endif