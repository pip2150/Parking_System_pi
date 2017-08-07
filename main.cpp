#include "Opencv/OCR.hpp"
#include "Opencv/Plate.hpp"
#include "Opencv/Svm.hpp"
#include "Opencv/Utils.hpp"
#include <thread>
#include <ctime>

using namespace cv;
using namespace std;

#define CAMERA 0
#define FILESYSTEM 1
#define TRUE 1
#define FALSE 0

/* ----- Debug Setting ----- */
#define SEGMENTSIZE 1
#define FROM CAMERA
#if FROM == FILESYSTEM
#define TRAIN FALSE
#endif
#define POSITION FALSE
#define COSTTIME FALSE
#define PLATESTR FALSE
#define WINDOWON TRUE
#define ANALYSIS TRUE
/* ------------------------- */

void send2server(string jsondata) {
	cout << "senddata : " + jsondata << endl;
}

int main(int argc, char* argv[]) {

	Mat image;
	thread *t = NULL;

#if FROM == CAMERA

	int totalCorrect = 0;
	int totalTry = 0;
	double sum = 0;
	Mat templ;
	VideoCapture camera;

	/*if (utils::readImage("divArea2.png", templ)) {
	  cerr << "File No Exist." << endl;
	  exit(1);
	  }*/

	camera.open(0);
	if (!camera.isOpened()) {
		cerr << "Can Not Access The Camera." << endl;
		exit(1);
	}
    camera.set(CV_CAP_PROP_FOURCC, CV_FOURCC('Y','U','Y','V'));

#elif FROM == FILESYSTEM

	int img_num;
	cout << "img_num : " << endl;
	cin >> img_num;
	if (utils::readImage("InputImage/" + to_string(img_num) + ".jpg", image)) {
		cerr << "File No Exist." << endl;
	}

#endif

	OCR ocrChar(CHARACTER);
	OCR ocrNum(NUMBER);
	Svm svm;

#if FROM == CAMERA

	while (waitKey(100) != 27) {
		camera >> image;

#endif

		clock_t cycleCost = clock();
		Rect area[SEGMENTSIZE];
		vector<Mat> sample;

		for (int i = 0; i < SEGMENTSIZE; i++) {
			area[i] = Rect(image.cols * i / SEGMENTSIZE, 0, image.cols / SEGMENTSIZE, image.rows);
			//		Matimage divArea;
			Mat result;
			double maxVal = 0;
			Point maxLoc;

			//	divArea = (area[j]);
				//matchTemplate(divArea, templ, result, TM_CCOEFF_NORMED);
				//minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
				//rectangle(divArea, maxLoc, Point(maxLoc.x + templ.cols, maxLoc.y + templ.rows), Scalar(255, 0, 255), 2);

			if (maxVal >= 0.9) {
				// DataBase로 비어있는 구역이라고 보내야됨
				// DataBase로부터 값을 불러와서 보내려는값과 동일한 경우 보내지 않음
				//cout << j << " 구역 차량 없음" << endl;
				cout << i << "No Vehicle In Section" << endl;
			}
		}

		vector<Plate> PossiblePlates;
		vector<Point> PlatePositions;
		Plate::find(image, PossiblePlates, PlatePositions);

		if (COSTTIME) {
			cout << fixed;
			cout.precision(5);
			cout << "\tCost Time In a Cycle : " << (double)(clock() - cycleCost) / CLOCKS_PER_SEC << "s" << endl;
		}

		int k = 0;

		int PossiblePlatesSize = (int)PossiblePlates.size();
		for (int i = 0; i < PossiblePlatesSize; i++) {
			PossiblePlates[i].canonicalize();
			int response = (int)svm.predict(PossiblePlates[i].canonical);

			if (response != 1)
				continue;

			if (POSITION)
				for (int j = 0; j < SEGMENTSIZE; j++)
					if (area[j].contains(Point(PlatePositions[i].x, PlatePositions[i].y)))
						cout << "\t\tIt's " << j + 1 << "th Section." << endl;

			Plate *foundPlate = &PossiblePlates[i];
			clock_t findNumbers_start = clock();
			foundPlate->findNumbers();

			if (COSTTIME) {
				cout << fixed;
				cout.precision(5);
				cout << "\t\tCost Time In the FindNumbers : " << (double)(clock() - findNumbers_start) / CLOCKS_PER_SEC << "s" << endl;
			}

#if FROM == FILESYSTEM
			if (foundPlate->numbers.size() < 6)
				continue;
#elif FROM == CAMERA
			if (foundPlate->numbers.size() != 7)
				continue;
#endif

			string str = "";
			for (int j = (int)foundPlate->numbers.size() - 1; j >= 0; j--) {
				Plate::Number* number = &foundPlate->numbers[j];
				number->canonicalize(SAMPLESIZE);

#if FROM == FILESYSTEM
#if TRAIN == TRUE
				sample.push_back(foundPlate->numbers[j].canonical);
#endif
#endif

				OCR *ocr;
				/* 마지막에서 4번째를 문자로 설정 - 한글 */
				/* if (j == 4) */
				/* 마지막에서 0,1,2번째를 문자로 설정 - 영문 */
				if ((j == 0) || (j == 1) || (j == 2))
					ocr = &ocrChar;
				else
					ocr = &ocrNum;
				

				Mat feature = ocr->features(number->canonical, SAMPLESIZE);
				Mat output(1, ocr->numCharacters, CV_32FC1);

				ocr->predict(feature, output);

				str += ocr->classify(output);

				if (WINDOWON) {
					int winNo = (int)foundPlate->numbers.size() - j - 1;
					string winName = to_string(winNo);
					imshow(winName, number->canonical);
					moveWindow(winName, WINDOW_X + 20 * winNo, 0 + k * 60);
				}

			}
			//string path = "\"C:\\Users\\dhrco\\OneDrive - pukyong.ac.kr\\Workspace\\CDTWorkspace\\Parking System\\Debug\\Parking System.exe\" ";
			/*string path = "Network/http_test ";
			path += str;
			cout << path << endl;
			system(path.c_str());*/

			if (PLATESTR) {
				cout << "\t\t" << str << endl;
			}

			//	t->joinable();
			//	t = new thread(&send2server, str);

#if FROM == CAMERA
#if ANALYSIS == TRUE
			string answer = "0226FBV";
			int correct = 0;
			for (int j = 0; j < 7; j++)
				if (str[j] == answer[j])
					correct++;

			totalTry++;
			totalCorrect += correct;
			double average = correct / 7.0;
			double totalAverage = totalCorrect / (totalTry * 7.0);
			cout << "\t\tCorrect Answer Rate : " << average * 100 << "\%" << "\tTotal Correct Answer Rate : " << totalAverage * 100 << "\%" << endl;;
#endif
#endif
			if (WINDOWON) {
				imshow("warp" + to_string(i), foundPlate->img);
				moveWindow("warp" + to_string(i), WINDOW_X, WINDOW_Y);
			}

			k++;
		}

		//t->joinable();

		if (WINDOWON)
			for (int i = 0; i < SEGMENTSIZE; i++) {
				Mat divArea = image(area[i]);
				imshow("divArea" + to_string(i), divArea);
				moveWindow("divArea" + to_string(i), 0 + divArea.size().width*i, WINDOW_Y);
			}
	}

#if FROM == FILESYSTEM
	int key = waitKey(0);

#if TRAIN == TRUE

	string answer;
	cin >> answer;

	if (sample.size() == answer.size())
		for (int i = 0; i < answer.size(); i++) {
			string path;
			Mat img;
			int j = 0;

			do {
				path = "TrainNumber/" + string(1, answer[i]) + "/" + to_string(j) + ".jpg";
				cout << path << endl;
				j++;
			} while (!utils::readImage(path, img, 1));

			//cout << path << endl;
			if (utils::writeImage(path, sample[i])) {
				cerr << "Fail To Write." << endl;
				//exit(1);
			}

		}
#endif
#endif

	//	t->joinable();
}
