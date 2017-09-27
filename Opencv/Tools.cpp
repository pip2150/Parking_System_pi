#include "Tools.hpp"
#include "OCR.hpp"

using namespace cv;
using namespace std;

int tools::readImage(const string fn, Mat& image, int mode) {
	image = imread(fn, mode);

	if (image.empty()) {
		return 1;
	}
	return 0;
}

int tools::writeImage(const string fn, const Mat &image, int mode) {
	if (!imwrite(fn, image)) {
		return 1;
	}
	return 0;
}

tools::Dicider::Dicider() {
	keyStr = "";
	match = -1;
}

bool tools::Dicider::decide(const std::string str) {
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

tools::Analyzer::Analyzer(const std::string answer) {
	totalCorrect = 0;
	totalTry = 0;
	this->answer = answer;
}

void tools::Analyzer::analyze(const std::string str) {
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

tools::SVMTrainer::SVMTrainer() {
	fileIndex = 0;
}

void tools::SVMTrainer::train(const cv::Mat &sample) {
	std::string path;
	cv::Mat img;

	Mat svmdata;
	resize(sample, svmdata, Size(144, 33), 0, 0, INTER_CUBIC);

	do {
		path = "trainimage/" + std::to_string(fileIndex) + ".png";
		std::cout << path << std::endl;
		fileIndex++;
	} while (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

	//std::cout << path << std::endl;
	if (tools::writeImage(path, svmdata)) {
		std::cerr << "Fail To Write." << std::endl;
		exit(1);
	}
}

tools::OCRTrainer::OCRTrainer(const std::string answer) {
	memset(fileIndex, 0, sizeof(fileIndex));
	this->answer = answer;
}

void tools::OCRTrainer::train(const std::vector<cv::Mat> &sample) {
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

