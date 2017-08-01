#include "OCR.hpp"
#include "utils.hpp"

using namespace cv::ml;
using namespace cv;
using namespace std;
using namespace utils;

OCR::OCR() {
	recNum = 23;
	/*collectTrainImages();
	writeTraindata("Opencv/OCR.xml");*/
	readTraindata("Opencv/OCR.xml");
	train(SAMPLESIZE);
}

//const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z' };
const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'E', 'F', 'G', 'N', 'S', 'T', 'V', 'W', 'X', 'Y' };

void OCR::readTraindata(string fn) {
	FileStorage fs(fn, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "File Open Fail." << endl;
		exit(1);
	}

	fs["TrainingData"] >> trainingData;

	fs["classes"] >> classes;

	fs.release();
}

void OCR::writeTraindata(string fn) {
	FileStorage fs(fn, FileStorage::WRITE);

	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();
}

void OCR::train(int nlayers) {

	Mat layerSizes(3, 1, CV_32SC1);

	layerSizes.at<int>(0) = trainingData.cols;
	layerSizes.at<int>(1) = nlayers;
	layerSizes.at<int>(2) = recNum;

	/*cout << "layout : " << layerSizes << endl;*/
	ann = ANN_MLP::create();

	ann->setLayerSizes(layerSizes);
	ann->setActivationFunction(ANN_MLP::SIGMOID_SYM, 1, 1);
	ann->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 300, 0.0001));
	ann->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);

	Mat trainClasses;
	trainClasses.create(trainingData.rows, recNum, CV_32FC1);

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
	for (int i = 0; i < recNum; i++) {
		int j = 0;
		while (1) {
			string path = "trainnumber/" + string(1, strCharacters[i]) + "/" + to_string(j) + ".jpg";
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

	return strCharacters[maxLoc.x];
}

float OCR::predict(Mat &img) {
	return ann->predict(img);
}

float OCR::predict(Mat &img, Mat &out) {
	return ann->predict(img, out);
}

Mat OCR::projectedHistogram(Mat &img, int t) {
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

Mat OCR::features(Mat &numbers, int sizeData) {

	Mat vhist = projectedHistogram(numbers, VERTICAL);
	Mat hhist = projectedHistogram(numbers, HORIZONTAL);
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