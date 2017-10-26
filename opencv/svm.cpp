#include "svm.hpp"
#include "tools.hpp"

using namespace cv::ml;
using namespace cv;
using namespace std;

Svm::Svm(const int mode) {

	// Json File 경로
	string jsonPath = "opencv/SVMDATA.json";

	if (mode & COLLECT) {
        int index;
        cout << "type threshold index. : " << endl;
        cin >> index;
		collectTrainImages(index);
		if (mode & WRITEDT)
			writeTraindata(jsonPath);
	}
	else
		readTraindata(jsonPath);

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

void Svm::collectTrainImages(int index) {
    int i = 0;

    while(1){
		string path = "trainimage/" + to_string(i) + ".png";
		Mat img;

		if (!tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE)) {
            break;
		}
		equalizeHist(img, img);
		Mat tmp = img.reshape(1, 1);

		trainingData.push_back(tmp);

		if (i < index)
			classes.push_back(1);
		else
			classes.push_back(0);
        i++;
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

SVMTrainer::SVMTrainer() {
	fileIndex = 0;
}

void SVMTrainer::train(const cv::Mat &sample) {
	// trainimage들의 경로
	std::string path;
	// 저장할 image
	cv::Mat img;

	// 정규화된 image
	Mat svmdata;
	resize(sample, svmdata, Size(144, 33), 0, 0, INTER_CUBIC);

	do {
		path = "trainimage/" + std::to_string(fileIndex) + ".png";
		std::cout << path << std::endl;
		fileIndex++;
	} while (tools::readImage(path, img, CV_LOAD_IMAGE_GRAYSCALE));

    cout << endl;
	//std::cout << path << std::endl;
	if (!tools::writeImage(path, svmdata)) {
		std::cerr << "Fail To Write." << std::endl;
		exit(1);
	}
}

