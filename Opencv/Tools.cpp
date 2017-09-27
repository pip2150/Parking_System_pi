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
		if (match == LEASTMATCH) {
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

	//* TEXTSIZE���� �ؽ�Ʈ �� ��ġ�� Ƚ��
	int correct = 0;
	for (int j = 0; j < TEXTSIZE; j++)
		if (str[j] == answer[j])
			correct++;

	totalTry++;
	totalCorrect += correct;

	//* �ϳ��� ��ȣ�ǿ��� ������ �ؽ�Ʈ�� ��������� ���� Ƚ��
	double average = (double)correct / TEXTSIZE;
	//* ��� �õ����� ������ �ؽ�Ʈ�� ��������� ���� Ƚ��
	double totalAverage = (double)totalCorrect / (totalTry * TEXTSIZE);
	std::cout << "\t\tCorrect Answer Rate : " << average * 100 << "%";
	std::cout << "\tTotal Correct Answer Rate : " << totalAverage * 100 << "%" << std::endl;
}

tools::SVMTrainer::SVMTrainer() {
	fileIndex = 0;
}

void tools::SVMTrainer::train(const cv::Mat &sample) {
	//* trainimage���� ���
	std::string path;
	//* ������ image
	cv::Mat img;

	//* ����ȭ�� image
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
	for (int index : fileIndexs)
		index = 0;
	this->answer = answer;
}

void tools::OCRTrainer::train(const std::vector<cv::Mat> &sample) {
	if (sample.size() == answer.size()) {
		for (int i = 0; i < answer.size(); i++) {
			//* TrainNumber���� ���
			std::string path;
			//* ������ image
			cv::Mat img;
			do {
				path = "TrainNumber/" + std::string(1, answer[i]) + "/" + std::to_string(fileIndexs[i]) + ".png";
				std::cout << path << std::endl;
				fileIndexs[i]++;
			} while (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

			std::cout << path << std::endl;

			if (tools::writeImage(path, sample[i])) {
				std::cerr << "Fail To Write." << std::endl;
				exit(1);
			}
		}
	}
}

