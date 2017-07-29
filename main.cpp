#include "Opencv/OCR.hpp"
#include "Opencv/Plate.hpp"
#include "Opencv/Svm.hpp"
#include "Opencv/utils.hpp"

using namespace cv;
using namespace std;
using namespace utils;

int main(int argc, char* argv[]) {
	srand((int)time(NULL));

	OCR ocr;
	ocr.recNum = 23;
	ocr.collectTrainImages();
	ocr.writeTraindata("Opencv/OCR.xml");
	ocr.readTraindata("Opencv/OCR.xml");
	ocr.train(SAMPLESIZE);

	Svm svm;
	//svm.collectTrainImages();
	//svm.writeTraindata("Opencv/SVMDATA.xml");
	svm.readTraindata("Opencv/SVMDATA.xml");
	svm.train();

	string fn = "";

	int img_num;
	if (argc > 1) {
		fn = string(argv[1]);
	}
	else {
		img_num = rand() % 15 + 1;

		//cin >> img_num;
		//img_num = 16;

		fn = fn +"InputImage"+ utils::to_string(img_num) + ".jpg";
	}
	cout << fn << endl;

	Mat image;
	/*if (readImage(fn, image)) {
	  cerr << "File No Exist." << endl;
	  }*/
	Mat templ;
	utils::readImage("temple/templ.jpg", templ);
	VideoCapture camera;
	camera.open(0);
	while (1) {
		camera >> image;
		Rect area[4];

		area[0]=Rect(0, 0, image.cols / 4, image.rows);
		area[1]=Rect(image.cols * 1 / 4, 0, image.cols / 4, image.rows);
		area[2]=Rect(image.cols * 2 / 4, 0, image.cols / 4, image.rows);
		area[3]=Rect(image.cols * 3 / 4, 0, image.cols / 4, image.rows);

		for (int j = 0; j < 4; j++) {
			Mat divArea;
			Mat result;
			double maxVal;
			Point maxLoc;

			divArea = image(area[j]);
			matchTemplate(divArea, templ, result, TM_CCOEFF_NORMED);
			minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);

			//rectangle(divArea, maxLoc,
			//Point(maxLoc.x + templ.cols, maxLoc.y+templ.rows), Scalar(255, 0, 255), 2);
			if (maxVal >= 0.9) {
				// DataBase로 비어있는 구역이라고 보내야됨
				// DataBase로부터 값을 불러와서 보내려는값과 동일한 경우 보내지 않음
				cout << j << " 구역 차량 없음" << endl;
			}
			else{
				Mat gray;
				cvtColor(image, gray, CV_BGR2GRAY);
				blur(gray, gray, Size(3, 3));

				vector<Plate> PossiblePlates;
				Plate::extract(gray, PossiblePlates);

				int k = 0;
				vector<Mat> sample;
				for (int i = 0; i < (int)PossiblePlates.size(); i++) {
					Mat img = PossiblePlates[i].canonicalize();
					int response = (int)svm.predict(img);
					if (response == 1) {
						k++;
						Plate foundPlate = PossiblePlates[i];
						foundPlate.findNumbers();

						for (int j = foundPlate.numbers.size() - 1; j >= 0; j--) {
							foundPlate.numbers[j].canonicalize(SAMPLESIZE);
							Mat feature = ocr.features(foundPlate.numbers[j].canonical, SAMPLESIZE);
	
							Mat output(1, ocr.recNum, CV_32FC1);
							ocr.predict(feature, output);
							cout << ocr.classify(output) << " ";

							unsigned winNo = foundPlate.numbers.size() - j - 1;
							string winName = to_string(winNo) + to_string(i);
							imshow(winName, foundPlate.numbers[j].canonical);
							moveWindow(winName, 1200 + 20 * winNo, 0 + k * 60);
						}
						cout << endl;

						imshow("image"+ to_string(i), image);
						imshow("warp"+ to_string(i), foundPlate.img);
						moveWindow("warp"+ to_string(i), 1200, 500);
					}
				}


			}
		}

		int key = waitKey(10);
		if (key == 0)
			break;
	}
}
