#include "ocr.hpp"
#include "tools.hpp"

using namespace cv::ml;
using namespace cv;

OCR::OCR(const FORMAT format, const int mode) {
	this->numCharacters = format;

	if ((format != NUMBER) && (format != CHARACTER) && (format != (NUMBER + CHARACTER))) {
		std::cerr << "Long Format Was Inputed!" << std::endl;
		exit(1);
	}

	// Json File 경로
	std::string jsonPath = "Opencv/OCR.json";
	if (mode & COLLECT) {
		collectTrainImages();
		if (mode & WRITEDT)
			writeTraindata(jsonPath);
	}
	else
		readTraindata(jsonPath, format);

	// layer 크기
	Mat layerSizes(3, 1, CV_32SC1);

	layerSizes.at<int>(0) = trainingData.cols;
	layerSizes.at<int>(1) = NLAYERS;
	layerSizes.at<int>(2) = numCharacters;

	ann = ANN_MLP::create();

	ann->setLayerSizes(layerSizes);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	ann->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);

	Mat trainClasses;
	trainClasses.create(trainingData.rows, numCharacters, CV_32FC1);

	for (int i = 0; i < trainClasses.rows; i++) {
		for (int k = 0; k < trainClasses.cols; k++) {
			if (k == classes.at<int>(i))
				trainClasses.at<float>(i, k) = 1;
			else
				trainClasses.at<float>(i, k) = 0;
		}
	}

	ann->train(trainingData, ROW_SAMPLE, trainClasses);

}

void OCR::readTraindata(const std::string fn) {
	FileStorage fs(fn, cv::FileStorage::READ | FileStorage::FORMAT_JSON);

	if (!fs.isOpened()) {
		std::cerr << "File Open Fail." << std::endl;
		exit(1);
	}

	fs["TrainingData"] >> trainingData;
	fs["classes"] >> classes;
	fs.release();

}

void OCR::readTraindata(const std::string fn, const FORMAT format) {
	readTraindata(fn);

	if (format == CHARACTER + NUMBER)
		return;

	Mat _trainingData;
	Mat _classes;

	for (int i = 0; i < classes.rows; i++) {
		// 문자 OCR의 경우
		if (format == CHARACTER) {
			if (classes.at<int>(i, 0) >= NUMBER) {
				_classes.push_back(classes.at<int>(i, 0) - NUMBER);
				_trainingData.push_back(trainingData.row(i));
			}
		}
		// 숫자 OCR의 경우
		else if (format == NUMBER) {
			if (classes.at<int>(i, 0) < NUMBER) {
				_classes.push_back(classes.at<int>(i, 0));
				_trainingData.push_back(trainingData.row(i));
			}
		}
	}

	_trainingData.copyTo(trainingData);
	_classes.copyTo(classes);

}

void OCR::writeTraindata(const std::string fn) {
	FileStorage fs(fn, FileStorage::WRITE | FileStorage::FORMAT_JSON);

	if (!fs.isOpened()) {
		std::cerr << "File Write Fail." << std::endl;
		exit(1);
	}

	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();

}

void OCR::collectTrainImages() {
	for (int i = 0; i < CHARACTER + NUMBER; i++) {
		int j = 0;
		while (1) {
			std::string path = "TrainNumber/" + std::string(1, strCharacters[i]) + "/" + std::to_string(j) + ".png";
			Mat img;
			std::cout << path << std::endl;
			if (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE)) {
				break;
			}

			Mat tmp = img.reshape(1);
			resize(tmp, tmp, Size(SAMPLESIZE, SAMPLESIZE));
			trainingData.push_back(features(tmp, SAMPLESIZE));
			classes.push_back(i);
			j++;
		}
	}

	trainingData.convertTo(trainingData, CV_32FC1);
}

char OCR::classify(Mat *output) {
	Point maxLoc;
	double maxVal;

	minMaxLoc(*output, 0, &maxVal, 0, &maxLoc);

	if (numCharacters == CHARACTER)
		return strCharacters[maxLoc.x + 10];
	else
		return strCharacters[maxLoc.x];
}

float OCR::predict(const Mat &img, Mat *out) {
	return ann->predict(img, *out);
}

Mat OCR::getHistogram(const Mat &img, const ORIENTATION t) {
	int sz = (t == HORIZONTAL) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F);

	for (int j = 0; j < sz; j++) {
		Mat data = (t == HORIZONTAL) ? img.row(j) : img.col(j);

		mhist.at<float>(j) = (float)countNonZero(data);
	}

	double min, max;
	minMaxLoc(mhist, &min, &max);

	if (max > 0)
		mhist.convertTo(mhist, -1, 1.0f / max, 0);

	return mhist;
}

Mat OCR::features(const Mat &texts, const int sizeData) {

	Mat vhist = getHistogram(texts, VERTICAL);
	Mat hhist = getHistogram(texts, HORIZONTAL);
	Mat lowData;

	if (texts.size() == Size(sizeData, sizeData))
		texts.copyTo(lowData);
	else
		resize(texts, lowData, Size(sizeData, sizeData));

	int numCols = vhist.cols + hhist.cols + lowData.cols * lowData.cols;
	Mat out = Mat::zeros(1, numCols, CV_32F);

	int k = 0;
	for (int i = 0; i < vhist.cols; i++) {
		out.at<float>(k) = vhist.at<float>(i);
		k++;
	}
	for (int i = 0; i < hhist.cols; i++) {
		out.at<float>(k) = hhist.at<float>(i);
		k++;
	}
	for (int x = 0; x < lowData.cols; x++) {
		for (int y = 0; y < lowData.rows; y++) {
			out.at<float>(k) = (float)lowData.at<unsigned char>(x, y);
			k++;
		}
	}

	return out;
}

OCRTrainer::OCRTrainer(const std::string answer) {
	for (int index : fileIndexs)
		index = 0;
	this->answer = answer;
}

void OCRTrainer::train(const std::vector<cv::Mat> &sample) {
	if (sample.size() == answer.size()) {
		for (int i = 0; i < answer.size(); i++) {
			// TrainNumber들의 경로
			std::string path;
			// 저장할 image
			cv::Mat img;
			do {
				path = "TrainNumber/" + std::string(1, answer[i]) + "/" + std::to_string(fileIndexs[i]) + ".png";
				std::cout << path << std::endl;
				fileIndexs[i]++;
			} while (tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

			std::cout << path << std::endl;

			if (!tools::writeImage(path, sample[i])) {
				std::cerr << "Fail To Write." << std::endl;
				exit(1);
			}
		}
	}
}

