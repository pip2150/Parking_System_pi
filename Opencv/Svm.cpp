#include "Svm.hpp"
#include "Tools.hpp"

using namespace cv::ml;
using namespace cv;
using namespace std;

Svm::Svm(const int flags) {

	if (flags & COLLECT) {
		collectTrainImages();
		if (flags & WRITEDT)
			writeTraindata("Opencv/SVMDATA.json");
	}
	else
		readTraindata("Opencv/SVMDATA.json");
	
	svm = SVM::create();

	svm->setType(SVM::C_SVC);
	svm->setKernel(SVM::LINEAR);
	svm->setDegree(0);
	svm->setGamma(1);
	svm->setCoef0(0);
	svm->setC(1);
	svm->setNu(0);
	svm->setP(0);
	svm->setTermCriteria(TermCriteria(CV_TERMCRIT_ITER, 1000, 0.01));

	svm->train(trainingData, ROW_SAMPLE, classes);
}

void Svm::collectTrainImages() {
	int imagecnt = 262;

	for (int i = 0; i < imagecnt; i++) {
		string path = "trainimage/" + to_string(i) + ".png";
		Mat img;

		if (tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE)) {
			cerr << "File No Exist." << endl;
			exit(1);
		}
		equalizeHist(img, img);
		Mat tmp = img.reshape(1, 1);

		trainingData.push_back(tmp);

		if (i < 233)
			classes.push_back(1);
		else
			classes.push_back(0);
	}

	trainingData.convertTo(trainingData, CV_32FC1);
}

float Svm::predict(const Mat &img) {
	return svm->predict(img);
}

void Svm::readTraindata(const string fn) {
	FileStorage fs(fn, FileStorage::READ | FileStorage::FORMAT_JSON);

	if (!fs.isOpened()) {
		cout << "File Open Fail." << endl;
		exit(1);
	}

	fs["TrainingData"] >> trainingData;
	fs["classes"] >> classes;
	fs.release();

	trainingData.convertTo(trainingData, CV_32FC1);
}

void Svm::writeTraindata(const string fn) {
	FileStorage fs(fn, FileStorage::WRITE | FileStorage::FORMAT_JSON);

	fs << "TrainingData" << trainingData;
	fs << "classes" << classes;
	fs.release();
}
