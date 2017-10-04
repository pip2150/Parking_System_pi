#include "Tools.hpp"
#include "OCR.hpp"

bool tools::readImage(const std::string fn, cv::Mat& image, int flags) {
	image = cv::imread(fn, flags);

	return image.empty();
}

bool tools::writeImage(const std::string fn, const cv::Mat &image) {
	return cv::imwrite(fn, image);
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

	// TEXTSIZE개의 텍스트 중 일치한 횟수
	int correct = 0;
	for (int j = 0; j < TEXTSIZE; j++)
		if (str[j] == answer[j])
			correct++;

	totalTry++;
	totalCorrect += correct;

	// 하나의 번호판에서 예측한 텍스트가 평균적으로 맞은 횟수
	double average = (double)correct / TEXTSIZE;
	// 모든 시도에서 예측한 텍스트가 평균적으로 맞은 횟수
	double totalAverage = (double)totalCorrect / (totalTry * TEXTSIZE);
	std::cout << "\t\tCorrect Answer Rate : " << average * 100 << "%";
	std::cout << "\tTotal Correct Answer Rate : " << totalAverage * 100 << "%" << std::endl;
}

