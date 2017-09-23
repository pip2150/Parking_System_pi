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
	int readImage(std::string fn, cv::Mat& image, int mode = 1);
	int writeImage(std::string fn, cv::Mat& image, int mode = 1);

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
		Analyzer(std::string answer);
		void analyze(std::string str);
	};

	class SVMTrainer {
	private:
		int fileIndex;

	public:
		SVMTrainer();
		void train(cv::Mat &sample);
	};

	class OCRTrainer {
	private:
		int fileIndex[NUMBER + CHARACTER];
		std::string answer;

	public:
		OCRTrainer(std::string answer);
		void train(std::vector<cv::Mat> &sample);
	};
};


#endif
