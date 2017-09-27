#ifndef TOOLS_HPP_
#define TOOLS_HPP_

#include <opencv2/opencv.hpp>
#include "OCR.hpp"

#define READDT 0
#define COLLECT 1
#define WRITEDT 3
#define MAXMATCH 5
#define NUMSIZE 7

namespace tools {
	int readImage(const std::string fn, cv::Mat& image, int mode = 1);
	int writeImage(const std::string fn, const cv::Mat& image, int mode = 1);

	class Dicider {
	private:
		std::string keyStr;
		int match;

	public:
		Dicider();
		bool decide(std::string str);
	};


	class Analyzer {
	private:
		int totalCorrect;
		int totalTry;
		std::string answer;

	public:
		Analyzer(const std::string answer);

		/** ¡÷ºÆ
		*/
		void analyze(const std::string str);
	};

	class SVMTrainer {
	private:
		int fileIndex;

	public:
		SVMTrainer();
		void train(const cv::Mat &sample);
	};

	class OCRTrainer {
	private:
		int fileIndex[NUMBER + CHARACTER];
		std::string answer;

	public:
		OCRTrainer(const std::string answer);
		void train(const std::vector<cv::Mat> &sample);
	};
};


#endif
