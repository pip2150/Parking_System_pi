#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#include "OCR.hpp"
#include "Tools.hpp"
#include <ctime>

#define CAMERA 0
#define FILESYSTEM 1
#define TRUE 1
#define FALSE 0

/* ----- Debug Setting ----- */
#define ENTER 0
#define EXIT 1
#define SEGMENTSIZE 4
#define MAXMATCH 5
#define NUMSIZE 7
#define FROM CAMERA
#define NETWORK		0x01
#define TRAIN		0x02
#define POSITION 	0x04
#define COSTTIME 	0x08
#define PLATESTR 	0x10
#define WINDOWON 	0x20
#define ANALYSIS	0x40
#define NOTUSEML	0x80
/* ------------------------- */

class Dicider {
private:
	std::string keyStr;
	int match;

public:
	Dicider() {
		keyStr = "";
		match = -1;
	}

	bool decide(std::string str) {
		match++;
		if (match == 0) {
			keyStr = str;
			return false;
		}
		else if (keyStr == str) {
			if (match == MAXMATCH) {
                match = -1;
				return true;
			}
			else
				return false;
		}
		match = -1;
		return false;
	}
};

class Analyzer {
private:
	int totalCorrect;
	int totalTry;
	std::string answer;

public:
	Analyzer(std::string answer) {
		totalCorrect = 0;
		totalTry = 0;
		this->answer = answer;
	}

	void analyze(std::string str) {
		int correct = 0;
		for (int j = 0; j < NUMSIZE; j++)
			if (str[j] == answer[j])
				correct++;

		totalTry++;
		totalCorrect += correct;
		double average = (double)correct / NUMSIZE;
		double totalAverage = (double)totalCorrect / (totalTry * NUMSIZE);
		std::cout << "\t\tCorrect Answer Rate : " << average * 100 << "%";
		std::cout << "\tTotal Correct Answer Rate : " << totalAverage * 100 << "%" << std::endl;
	}
};

class SVMTrainer {
private:
	int fileIndex;
public:
	SVMTrainer() {
		fileIndex = 0;
	}

	void train(cv::Mat &sample) {
		std::string path;
		cv::Mat img;
		do {
			path = "trainimage/" + std::to_string(fileIndex) + ".png";
			std::cout << path << std::endl;
			fileIndex++;
		} while (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

		//std::cout << path << std::endl;
		if (tools::writeImage(path, sample)) {
			std::cerr << "Fail To Write." << std::endl;
			exit(1);
		}
	}
};

class Trainer {
private:
	int fileIndex[NUMBER + CHARACTER];
	std::string answer;
public:
	Trainer(std::string answer) {
		memset(fileIndex, 0, sizeof(fileIndex));
		this->answer = answer;
	}

	void train(std::vector<cv::Mat> &sample) {
		if (sample.size() == answer.size()) {
			for (int i = 0; i < answer.size(); i++) {
				std::string path;
				cv::Mat img;
				do {
					path = "TrainNumber/" + std::string(1, answer[i]) + "/" + std::to_string(fileIndex[i]) + ".png";
					std::cout << path << std::endl;
					fileIndex[i]++;
				} while (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

				std::cout << path << std::endl;
				if (tools::writeImage(path, sample[i])) {
					std::cerr << "Fail To Write." << std::endl;
					exit(1);
				}
			}
		}
	}
};

int startOpencv(int width, int height, int way, int floor, std::string zoneName, std::string answer, int mode);

#endif
