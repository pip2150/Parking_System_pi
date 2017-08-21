#include "Opencv.hpp"

using namespace cv;
using namespace std;

int startOpencv(int mode) {
	string answer = "0226FBV";
	int fileIndex[NUMBER + CHARACTER];
	memset(fileIndex, 0, sizeof(fileIndex));

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
	camera.set(CV_CAP_PROP_FOURCC, CV_FOURCC('Y', 'U', 'Y', 'V'));

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

	int cnt = -1;
	string keyStrings;

#if FROM == CAMERA

	while (waitKey(1) != 27) {
		camera >> image;

#endif

		clock_t cycleCost = clock();
		Rect area[SEGMENTSIZE];

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

		if (mode & COSTTIME) {
			cout << fixed;
			cout.precision(5);
			cout << "\tCost Time In a Cycle : " << (double)(clock() - cycleCost) / CLOCKS_PER_SEC << "s" << endl;
		}

		int k = 0;

		vector<Mat> sample;

		int PossiblePlatesSize = (int)PossiblePlates.size();
		for (int i = 0; i < PossiblePlatesSize; i++) {
			PossiblePlates[i].setDebug(mode & WINDOWON);
			PossiblePlates[i].canonicalize();
			int response = (int)svm.predict(PossiblePlates[i].canonical);

			if (response != 1)
				continue;

			if (mode & POSITION)
				for (int j = 0; j < SEGMENTSIZE; j++)
					if (area[j].contains(PlatePositions[i]))
						cout << "\t\tIt's " << j + 1 << "th Section." << endl;

			Plate *foundPlate = &PossiblePlates[i];
			clock_t findNumbers_start = clock();
			foundPlate->findNumbers();

			if (mode & COSTTIME) {
				cout << fixed;
				cout.precision(5);
				cout << "\t\tCost Time In the FindNumbers : " << (double)(clock() - findNumbers_start) / CLOCKS_PER_SEC << "s" << endl;
			}

#if FROM == FILESYSTEM
			if (foundPlate->numbers.size() < 7)
				continue;
#elif FROM == CAMERA
			if (foundPlate->numbers.size() != 7)
				continue;
#endif

			cnt++;

			string str = "";
			for (int j = (int)foundPlate->numbers.size() - 1; j >= 0; j--) {
				Plate::Number* number = &foundPlate->numbers[j];
				number->canonicalize(SAMPLESIZE);

				if (mode & TRAIN)
					sample.push_back(foundPlate->numbers[j].canonical);

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

				if (mode & WINDOWON) {
					int winNo = (int)foundPlate->numbers.size() - j - 1;
					string winName = to_string(winNo);
					imshow(winName, number->canonical);
					moveWindow(winName, WINDOW_X + 20 * winNo, 0 + k * 60);
				}

			}

			if (mode & PLATESTR) {
				cout << "\t\t" << str << endl;
			}

			if (cnt == 0)
				keyStrings = str;
			else if (cnt < MAXMATCH)
				if (keyStrings != str)
					cnt = -1;
				else {
					if (mode & FINALDCS) {
						cout << "\t\tThe answer is " << str << "  " << rand() % 256 << endl;

						string path = "\"Network\\http_test.exe enter " + str + "\"";
						//path += str;
						/*cout << path << endl;*/

						if (mode & NETWORK)
							system(path.c_str());

					}
					cnt = -1;
				}

				//	t->joinable();
				//	t = new thread(&send2server, str);

#if FROM == CAMERA

				if (mode & ANALYSIS) {
					int correct = 0;
					for (int j = 0; j < 7; j++)
						if (str[j] == answer[j])
							correct++;

					totalTry++;
					totalCorrect += correct;
					double average = correct / 7.0;
					double totalAverage = totalCorrect / (totalTry * 7.0);
					cout << "\t\tCorrect Answer Rate : " << average * 100 << "%";
					cout << "\tTotal Correct Answer Rate : " << totalAverage * 100 << "%" << endl;
				}

#endif
				if (mode & WINDOWON) {
					imshow("warp" /*+ to_string(i)*/, foundPlate->img);
					moveWindow("warp" /*+ to_string(i)*/, WINDOW_X, WINDOW_Y);
				}

				k++;
		}

		//t->joinable();

		if (mode & WINDOWON)
			for (int i = 0; i < SEGMENTSIZE; i++) {
				Mat divArea = image(area[i]);
				imshow("divArea" + to_string(i), divArea);
				moveWindow("divArea" + to_string(i), 0 + divArea.size().width*i, WINDOW_Y);
			}


		if (mode & TRAIN)
			if (sample.size() == answer.size())
				for (int i = 0; i < answer.size(); i++) {
					if (i != 4)
						continue;
					string path;
					Mat img;
					do {
						path = "TrainNumber/" + string(1, answer[i]) + "/" + to_string(fileIndex[i]) + ".jpg";
						cout << path << endl;
						fileIndex[i]++;
					} while (!utils::readImage(path, img, 1));

					//cout << path << endl;
					if (utils::writeImage(path, sample[i])) {
						cerr << "Fail To Write." << endl;
						exit(1);
					}
				}

#if FROM == CAMERA
	}
#elif FROM == FILESYSTEM
		int key = waitKey(0);

#endif

		//	t->joinable();
		return 0;
}