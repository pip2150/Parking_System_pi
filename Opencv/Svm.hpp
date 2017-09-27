#ifndef SVM_HPP_
#define SVM_HPP_

#include <opencv2/opencv.hpp>

/** Support Vector Machines�� �ٷ�� ���� Ŭ����
*/
class Svm {
private:

	/** Support Vector Machines
	*/
	cv::Ptr<cv::ml::SVM> svm;

	/** trainingData�� ������ ��µ� vectors
	*/
	cv::Mat classes;

	/** �Ʒ��� ���� Samples
	*/
	cv::Mat trainingData;

public:

	/** Svm �ʱ�ȭ
	*/
	Svm(const int flags);

	/** trainimage���� �Ʒ� ������ �ҷ�����
	*/
	void collectTrainImages();

	/** �Ʒ� �����͸� File System�� json ���Ϸ� ����
	*/
	void writeTraindata(const std::string fn);

	/** �Ʒ� �����͸� File System���� json ���Ϸκ��� �ҷ�����
	*/
	void readTraindata(const std::string fn);

	/** �Էµ� Sample�� �Ʒõ� Support Vector Machines���� ������ ����� ���
	*/
	float predict(const cv::Mat &img);
};

#endif