#include "stdafx.h"

#include "OCR.hpp"
#include "Plate.hpp"
#include "Svm.hpp"
#include "utils.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	srand((int)time(NULL));

	OCR ocr;
	ocr.recNum = 23;
	ocr.collectTrainImages2();
	ocr.writeTraindata("OCR.xml");
	ocr.readTraindata("OCR.xml");
	ocr.train(SAMPLESIZE);

	Svm svm;
	//svm.collectTrainImages();
	//svm.writeTraindata("SVMDATA.xml");
	svm.readTraindata("SVMDATA.xml");
	svm.train();

	string fn = "";

	int img_num;
	if (argc > 1) {
		fn = string(argv[1]);
	}
	else {
		img_num = rand() % 15 + 1;
		
		/*int a[] = {13,14 };
		img_num = a[rand() % sizeof(a) / sizeof(int)];*/
		//cin >> img_num;
		//img_num = 16;

		/*img_num = rand() % 37 + 1;
		fn = "C:/Users/dhrco/Desktop/ÀÚµ¿Â÷/plates/";*/

		fn = fn + to_string(img_num) + ".jpg";
	}


	cout << "Car Number (0~16) : ";
	cout << fn << endl;

	vector<Mat> sample;
	Mat image;
	if (Utils::readImage(fn, image)) {
		cerr << "File No Exist." << endl;
	}

	/*VideoCapture camera;
	camera.open(0);
	while (1) {
		camera >> image;*/

	Mat gray;
	cvtColor(image, gray, CV_BGR2GRAY);

	blur(gray, gray, Size(3, 3));

	vector<RotatedRect> rects = Plate::find(gray);
	vector<Plate> PossiblePlates;

	for (int i = 0; i < (int)rects.size(); i++) {
		RotatedRect roRect = Plate::detect(image, rects[i]);

		if (Plate::verifySizes(roRect)) {
			PossiblePlates.push_back(Plate::extract(gray, roRect));
		}
	}
	int k = 0;
	for (int i = 0; i < (int)PossiblePlates.size(); i++) {
		Mat img = PossiblePlates[i].getImg().reshape(1, 1);
		img.convertTo(img, CV_32FC1);

		int response = (int)svm.predict(img);

		if (response == 1) {
			k++;
			Plate foundPlate = PossiblePlates[i];

			vector<Mat> plateNumbers;
			foundPlate.findNumbers(image, plateNumbers);

			/*for (int j = 0; j < plateNumbers.size(); j++) {*/
			for (int j = plateNumbers.size() - 1; j >= 0; j--) {

				Mat plateNum = plateNumbers[j];

				double ratio = (plateNum.rows > plateNum.cols) ?
					((double)SAMPLESIZE / (double)plateNum.size().height) :
					((double)SAMPLESIZE / (double)plateNum.size().width);

				Mat resized;
				resize(plateNum, resized, Size2f(ratio*plateNum.size().width, ratio*plateNum.size().height));

				int width = resized.size().width;
				int height = resized.size().height;
				double x = (double)(SAMPLESIZE - width) / 2;
				double y = (double)(SAMPLESIZE - height) / 2;

				Mat canonical(Size(SAMPLESIZE, SAMPLESIZE), resized.type(), Scalar(255));
				resized.copyTo(canonical(Rect2f(x, y, width, height)));

				Mat feature;
				feature = Plate::features(canonical, SAMPLESIZE);
				//string outfn = "trainnumber2/" + to_string(img_num) + "_" + to_string(rand() % numeric_limits<int>::max()) + ".jpg";
				//cout << outfn << endl;
				//cout << Utils::writeImage(outfn, canonical) << endl;
				sample.push_back(canonical);

				Mat output(1, ocr.recNum, CV_32FC1);
				ocr.predict(feature, output);

				cout << ocr.classify(output) << " ";

				int winNo = plateNumbers.size() - j - 1;
				string winName = to_string(winNo) + to_string(i);
				imshow(winName, canonical);
				moveWindow(winName, 1200 + 20 * winNo, 0 + k * 60);

			}
			cout << endl;
			Mat warpedImage;
			foundPlate.warpingRotatedRect(image, warpedImage);

			imshow("image" /*+ to_string(i)*/, image);
			imshow("warp" /*+ to_string(i)*/, warpedImage);
			moveWindow("warp" /*+ to_string(i)*/, 1200, 500);

		}
	}
	/*	imshow("camera", image);

	int key = waitKey(10);
	if (key == 0)
		break;*/

	cv::waitKey(0);

	string answer;
	cin >> answer;
	if (sample.size() == answer.size())
		for (int i = 0; i < sample.size(); i++) {
			string index;
			int j = -1;
			do {
				j++;
				index = "trainnumber2/" + string(1, answer.at(i)) + "/" + to_string(j) + ".jpg";
				//cout << index << endl;
			} while (!Utils::readImage(index, Mat()));
			if (j < 30) {
				string outfn = "trainnumber2/" + string(1, answer.at(i)) + "/" + to_string(j) + ".jpg";
				cout << outfn << endl;
				Utils::writeImage(outfn, sample[i]);
			}
		}

	/*}*/
}