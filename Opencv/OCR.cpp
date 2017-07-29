
#include "OCR.hpp"
#include "utils.hpp"
#include "Plate.hpp"

using namespace cv::ml;
using namespace cv;
using namespace std;

//const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z' };
const char OCR::strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'E', 'F', 'G', 'N', 'S', 'T', 'V', 'W', 'X', 'Y' };

void OCR::readTraindata(string fn) {
	FileStorage fs(fn, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		cout << "File Open Fail." << endl;
		exit(1);
	}
	fs["TrainingData"] >> trainingData; break;
	fs["classes"] >> classes;

	fs.release();
}

void OCR::writeTraindata(string fn) {
	FileStorage fs(fn, FileStorage::WRITE);

	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();
}
//int OCR::writeTraindata(string fn, vector<Mat> numbers, vector<int> nums) {
//	char strCharacters[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'W', 'X', 'Y', 'Z' };
//
//	string str[] = { "3028BYS", "3154FFY", "3266CNT", "3732FWW", "5445BSX", "7215BGN", "8995CCN", "9588DWV", "9773BNB", "0226FBV", "4898GXY", "06-4444", "FE612SC", "FE612SC", "11-3174", "39-2764" };
//
//	FileStorage fs(fn, FileStorage::WRITE);
//
//	Mat classes;
//	Mat trainingData;
//	Mat trainingDataf5;
//	Mat trainingDataf10;
//	Mat trainingDataf15;
//	Mat trainingDataf20;
//
//	for (int k = 0; k < nums.size(); k++) {
//		/*cout << "k : " << k<<endl;
//		cout << "nums : " << nums[k] << endl;
//		cout << "str : " << str[nums[k]-1] << endl;
//		cout << "str size : " << str[nums[k]-1].size() << endl;
//		cout << "numbers size : " << numbers.size() << endl;*/
//		if (str[nums[k] - 1].size()*nums.size() != numbers.size()) {
//			cerr << "String lengths dosen't match" << endl;
//			return 1;
//		}
//
//		for (int i = 0; i < 7; i++) {
//			Mat plateNum;
//			resize(numbers[i], plateNum, Size(SAMPLESIZE, SAMPLESIZE));
//
//			for (int j = 0; j < strlen(strCharacters); j++) {
//				/* --- isFound ---- */
//				if (str[nums[k] - 1].at(i) == strCharacters[j]) {
//					//cout << str[nums[k] - 1].at(i) << endl;
//					trainingDataf5.push_back(Plate::features(plateNum, 5));
//					trainingDataf10.push_back(Plate::features(plateNum, 10));
//					trainingDataf15.push_back(Plate::features(plateNum, 15));
//					trainingDataf20.push_back(Plate::features(plateNum, 20));
//					classes.push_back(j);
//					goto found;
//				}
//			}
//			/* ---- Not found ---- */
//			{
//				cerr << "Character does not exist." << endl;
//				continue;
//			}
//		found:;
//			/* ---- found ---- */
//		}
//	}
//
//	trainingDataf5.convertTo(trainingDataf5, CV_32FC1);
//	trainingDataf10.convertTo(trainingDataf10, CV_32FC1);
//	trainingDataf15.convertTo(trainingDataf15, CV_32FC1);
//	trainingDataf20.convertTo(trainingDataf20, CV_32FC1);
//
//	fs << "TrainingDataF5" << trainingDataf5;
//	fs << "TrainingDataF10" << trainingDataf10;
//	fs << "TrainingDataF15" << trainingDataf15;
//	fs << "TrainingDataF20" << trainingDataf20;
//	fs << "classes" << classes;
//
//	fs.release();
//	cout << "Train was completed" << endl;
//	return 0;
//};

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
	int imagecnt = 10;
	char text[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'B', 'C', 'D', 'E', 'F', 'G', 'N', 'S', 'T', 'V', 'W', 'X', 'Y' };

	for (int i = 0; i < imagecnt; i++) {
		string path = "trainnumber2/" + Utils::to_string(i) + ".png";
		cout << path << endl;
		Mat img;

		if (Utils::readImage(path, img, 1)) {
			exit(1);
		}

		Mat tmp = img.reshape(1);
		resize(tmp, tmp, Size(SAMPLESIZE, SAMPLESIZE));

		trainingData.push_back(Plate::features(tmp, SAMPLESIZE));
		classes.push_back(i);

	}

	trainingData.convertTo(trainingData, CV_32FC1);
}

void OCR::collectTrainImages2() {

	for (int i = 0; i < recNum; i++) {
		int j=0;
		while (1) {
			string path = "trainnumber2/" + string(1,strCharacters[i]) + "/" + Utils::to_string(j) + ".jpg";
			Mat img;
			cout << path << endl;
			if (Utils::readImage(path, img, 1)) {
				break;
			}
			
			Mat tmp = img.reshape(1);
			resize(tmp, tmp, Size(SAMPLESIZE, SAMPLESIZE));
			trainingData.push_back(Plate::features(tmp, SAMPLESIZE));
			classes.push_back(i);
			j++;
		}
	}

	trainingData.convertTo(trainingData, CV_32FC1);
}

char OCR::classify(Mat output){
	Point maxLoc;
	double maxVal;

	minMaxLoc(output, 0, &maxVal, 0, &maxLoc);
	
	return strCharacters[maxLoc.x];
}

float OCR::predict(Mat img) {
	return ann->predict(img);
}

float OCR::predict(Mat img, Mat out) {
	return ann->predict(img, out);
}
