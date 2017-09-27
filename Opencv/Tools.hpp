#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <opencv2/opencv.hpp>
#include "OCR.hpp"

//* �ּ����� ��ġ Ƚ��
#define LEASTMATCH 5

namespace tools {

	/** File System���� ���� Image �ҷ�����
		���� �� 0, ���� �� 1�� ��ȯ
	*/
	int readImage(const std::string fn, cv::Mat& image, int mode = 1);

	/** File System���� Image�� ����
		���� �� 0, ���� �� 1�� ��ȯ
	*/
	int writeImage(const std::string fn, const cv::Mat& image, int mode = 1);

	/** ���� ����� �����ϴ� Ŭ����
	*/
	class Dicider {
	private:

		/** ���� ���ڿ�
		*/
		std::string keyStr;

		/** ��ġ�� Ƚ��
		*/
		int match;

	public:

		/** Dicider �ʱ�ȭ
		*/
		Dicider();

		/** �������� ���� Ƚ������ ���� ���ڿ��� �Է¹����� ���� ����� ���
		*/
		bool decide(std::string str);
	};

	/** ����� ����� ����ϴ� Ŭ����
	*/
	class Analyzer {
	private:

		/** �� ���� Ƚ��
		*/
		int totalCorrect;

		/** �� �õ��� Ƚ��
		*/
		int totalTry;

		/** ���� ����
		*/
		std::string answer;

	public:

		/** Analyzer �ʱ�ȭ
		*/
		Analyzer(const std::string answer);

		/** ����� ���ϸ� ����� ��� ����� ���
		*/
		void analyze(const std::string str);
	};

	/** SVM Ŭ������ collectTrainImages �� �ʿ��� image�� �����ϴ� Ŭ����
	*/
	class SVMTrainer {
	private:

		/** File �˻��� ���� Index ����
		*/
		int fileIndex;

	public:

		/** SVMTrainer �ʱ�ȭ
		*/
		SVMTrainer();

		/** �Է¹��� Mat ������ TrainImage�ν� File System�� ���
		*/
		void train(const cv::Mat &sample);
	};

	/** OCR Ŭ������ collectTrainImages �� �ʿ��� image�� �����ϴ� Ŭ����
	*/
	class OCRTrainer {
	private:

		/** File �˻��� ���� Index ����
		*/
		int fileIndexs[NUMBER + CHARACTER];

		/** �����н��� ���� ����
		*/
		std::string answer;

	public:

		/** OCRTrainer �ʱ�ȭ
		*/
		OCRTrainer(const std::string answer);

		/** �Է¹��� Mat �������� TrainImage�ν� File System�� ���
		*/
		void train(const std::vector<cv::Mat> &sample);
	};
};


#endif
