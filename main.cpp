#include "Opencv/OCR.hpp"
#include "Opencv/Plate.hpp"
#include "Opencv/Svm.hpp"
#include "Opencv/utils.hpp"

using namespace cv;
using namespace std;
using namespace utils;

int main(int argc, char* argv[]) {

	OCR ocr;
	Svm svm;
	Mat image;
	Mat templ;
	VideoCapture camera;

	if (readImage("divArea2.png", templ)) {
		cerr << "File No Exist." << endl;
		exit(1);
	}

	camera.open(0);

	while (1) {
		/*int img_num;
		cin >> img_num;
		readImage("InputImage/" + to_string(img_num) + ".jpg", image);*/
		camera >> image;
		Rect area[4];

		for (int j = 0; j < 1; j++) {
			area[j] = Rect(image.cols * j / 4, 0, image.cols / 4, image.rows);
			Mat divArea;
			Mat result;
			double maxVal= 0;
			Point maxLoc;

			divArea = image(area[j]);
			//matchTemplate(divArea, templ, result, TM_CCOEFF_NORMED);
			//minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);

			//rectangle(divArea, maxLoc,
			//Point(maxLoc.x + templ.cols, maxLoc.y+templ.rows), Scalar(255, 0, 255), 2);
			if (maxVal >= 0.9) {

				// DataBase로 비어있는 구역이라고 보내야됨
				// DataBase로부터 값을 불러와서 보내려는값과 동일한 경우 보내지 않음
				//cout << j << " 구역 차량 없음" << endl;
				cout << j << "No Vehicle In Section" << endl;

			}
			else {
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

						/*imshow("image" + to_string(i), image);
						imshow("warp" + to_string(i), foundPlate.img);
						moveWindow("warp" + to_string(i), 1200, 500);*/
						k++;
					}
				}
			}
			Point location = Point(1, 1);
			imshow("divArea" + to_string(j), divArea);
			moveWindow("divArea" + to_string(j), location.x + divArea.size().width*j, location.y);
		}

		int key = waitKey(0);
		if (key == 0)
			break;
	}
}