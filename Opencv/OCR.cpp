#include "OCR.hpp"
#include "Utils.hpp"

using namespace cv::ml;
using namespace cv;
using namespace std;
using namespace utils;

OCR::OCR(int format = NUMBER + CHARACTER) {

	this->numCharacters = format;

	switch (format) {
	case NUMBER: 
	case CHARACTER:	
		readTraindata("Opencv/OCR.xml", format);
		train(NLAYERS);
		break;

	case NUMBER + CHARACTER: 
		collectTrainImages();
		writeTraindata("Opencv/OCR.xml");
		/*readTraindata("Opencv/OCR.xml");*/
		break;
	default:
		cerr << "Long Format Was Inputed!" << endl;
		exit(1);
	}
}

void OCR::readTraindata(string fn) {
	FileStorage fs(fn, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		cerr << "File Open Fail." << endl;
		exit(1);
	}

	fs["TrainingData"] >> trainingData;
	fs["classes"] >> classes;
	fs.release();
}

void OCR::readTraindata(string fn, int format) {
	readTraindata(fn);
	
	Mat _trainingData;
	Mat _classes;

	for (int i = 0; i < classes.rows; i++) {
		if (format == CHARACTER) {
			if (classes.at<int>(i, 0) >= NUMBER) {
				_classes.push_back(classes.at<int>(i, 0) - NUMBER);
				_trainingData.push_back(trainingData.row(i));
			}
		}
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

void OCR::writeTraindata(string fn) {
	FileStorage fs(fn, FileStorage::WRITE);

	if (!fs.isOpened()) {
		cerr << "File Write Fail." << endl;
		exit(1);
	}

	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();

}

void OCR::train(int nlayers) {

	Mat layerSizes(3, 1, CV_32SC1);

	layerSizes.at<int>(0) = trainingData.cols;
	layerSizes.at<int>(1) = nlayers;
	layerSizes.at<int>(2) = numCharacters;

	/*cout << "layout : " << layerSizes << endl;*/
	ann = ANN_MLP::create();

	ann->setLayerSizes(layerSizes);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	ann->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);

	Mat trainClasses;
	trainClasses.create(trainingData.rows, numCharacters, CV_32FC1);

	for (int i = 0; i < trainClasses.rows; i++)
	{
		for (int k = 0; k < trainClasses.cols; k++)
		{
			if (k == classes.at<int>(i))
				trainClasses.at<float>(i, k) = 1;
			else
				trainClasses.at<float>(i, k) = 0;
		}
	}

	ann->train(trainingData, ROW_SAMPLE, trainClasses);
}

void OCR::collectTrainImages() {
	for (int i = 0; i < numCharacters; i++) {
		int j = 0;
		while (1) {
			string path = "TrainNumber/" + string(1, strCharacters[i]) + "/" + to_string(j) + ".jpg";
			Mat img;
			cout << path << endl;
			if (readImage(path, img, 1)) {
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

char OCR::classify(Mat &output) {
	Point maxLoc;
	double maxVal;

	minMaxLoc(output, 0, &maxVal, 0, &maxLoc);

	if (numCharacters == NUMBER)
		return strCharacters[maxLoc.x];
	if (numCharacters == CHARACTER)
		return strCharacters[maxLoc.x + 10];
}

float OCR::predict(Mat &img) {
	return ann->predict(img);
}

float OCR::predict(Mat &img, Mat &out) {
	return ann->predict(img, out);
}

/*		히스토그램 추출		*/
Mat OCR::getHistogram(Mat &img, int t) {
	int sz = (t) ? img.rows : img.cols;
	Mat mhist = Mat::zeros(1, sz, CV_32F);

	for (int j = 0; j < sz; j++) {
		Mat data = (t) ? img.row(j) : img.col(j);

		mhist.at<float>(j) = (float)countNonZero(data);
	}

	double min, max;
	minMaxLoc(mhist, &min, &max);

	if (max > 0)
		mhist.convertTo(mhist, -1, 1.0f / max, 0);

	return mhist;
}

/*		특징 추출		*/
Mat OCR::features(Mat &numbers, int sizeData) {

	Mat vhist = getHistogram(numbers, VERTICAL);
	Mat hhist = getHistogram(numbers, HORIZONTAL);
	Mat lowData;

	resize(numbers, lowData, Size(sizeData, sizeData));

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