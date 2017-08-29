#include "Opencv.hpp"
#include <thread>
#include <mutex>

using namespace cv;
using namespace std;

#define CAMERAWIDTH 640
#define CAMERAHEIGHT 480

int startOpencv(int mode) {

	/*if (utils::readImage("divArea2.png", templ)) {
	cerr << "File No Exist." << endl;
	exit(1);
	}*/

#if FROM == CAMERA
	VideoCapture camera;
	Mat cameraFrame;

	camera.open(0);
	if (!camera.isOpened()) {
		cerr << "Can Not Access The Camera." << endl;
		exit(1);
	}
	camera.set(CV_CAP_PROP_FOURCC, CV_FOURCC('m', 'j', 'p', 'g'));
	camera.set(CV_CAP_PROP_FRAME_WIDTH, CAMERAWIDTH);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, CAMERAHEIGHT);

#endif

	string answer = "0226FBV";

	Mat image;
	Mat templ;
	Rect area[SEGMENTSIZE];

	OCR ocrChar(CHARACTER);
	OCR ocrNum(NUMBER);
	Svm svm;

	Analyzer analyzer(answer);
	Trainer trainer(answer);
	SVMTrainer svmtrainer;
	Dicider dicider;

	mutex m;

	bool runing = true;

	thread camThread([&] {
		while (runing) {
			m.try_lock();
			camera >> cameraFrame;
			m.unlock();			
		}
	});

	thread procThread([&] {
		while (runing = (waitKey(50) != 27)) {

#if FROM == CAMERA
			clock_t cycleCost = clock();
			
			m.try_lock();
			cameraFrame.copyTo(image);			
			m.unlock();

			if (image.empty())
				continue;

			for (int i = 0; i < SEGMENTSIZE; i++)
				area[i] = Rect(image.cols * i / SEGMENTSIZE, 0, image.cols / SEGMENTSIZE, image.rows);

#elif FROM == FILESYSTEM

			if (waitKey(0) == 27)
				break;

			int img_num;
			cout << "img_num : " << endl;
			cin >> img_num;
			if (utils::readImage("InputImage/" + to_string(img_num) + ".jpg", image)) {
				cerr << "File No Exist." << endl;
				exit(1);
			}

#endif

			//for (int i = 0; i < SEGMENTSIZE; i++) {
			//	//		Matimage divArea;
			//	Mat result;
			//	double maxVal = 0;
			//	Point maxLoc;

			//	//	divArea = (area[j]);
			//		//matchTemplate(divArea, templ, result, TM_CCOEFF_NORMED);
			//		//minMaxLoc(result, NULL, &maxVal, NULL, &maxLoc);
			//		//rectangle(divArea, maxLoc, Point(maxLoc.x + templ.cols, maxLoc.y + templ.rows), Scalar(255, 0, 255), 2);

			//	if (maxVal >= 0.9) {
			//		// DataBase로 비어있는 구역이라고 보내야됨
			//		// DataBase로부터 값을 불러와서 보내려는값과 동일한 경우 보내지 않음
			//		//cout << j << " 구역 차량 없음" << endl;
			//		cout << i << "No Vehicle In Section" << endl;
			//	}
			//}

			vector<Plate> PossiblePlates;
			vector<Point> PlatePositions;

			Plate::find(image, PossiblePlates, PlatePositions);

			int k = 0;

			vector<Mat> sample;

			int PossiblePlatesSize = (int)PossiblePlates.size();
			/*int PossiblePlatesSize = 0;*/
			for (int i = 0; i < PossiblePlatesSize; i++) {
				PossiblePlates[i].setDebug(mode & WINDOWON);
				PossiblePlates[i].canonicalize();
				int response = (int)svm.predict(PossiblePlates[i].canonical);

				if (mode & WINDOWON) {
					imshow("plate", PossiblePlates[i].img);
					moveWindow("plate", WINDOW_X, SAMPLESIZE * 5);
				}

				Mat svmdata;
				resize(PossiblePlates[i].img, svmdata, Size(144, 33), 0, 0, INTER_CUBIC);
				//svmtrainer.train(svmdata);

				if (response != 1)
					continue;

				if (mode & POSITION)
					for (int j = 0; j < SEGMENTSIZE; j++)
						if (area[j].contains(PlatePositions[i]))
							cout << "\t\tIt's " << j + 1 << "th Section." << endl;

				Plate *foundPlate = &PossiblePlates[i];
				clock_t findNumbers_start = clock();

				bool isNumber = foundPlate->findNumbers(NUMSIZE);

#if FROM == CAMERA
				if (!isNumber)
					continue;
#endif

				if (mode & COSTTIME) {
					cout << fixed;
					cout.precision(5);
					cout << "\t\tCost Time In the FindNumbers : " << (double)(clock() - findNumbers_start) / CLOCKS_PER_SEC << "s" << endl;
				}

				string str = "";
				int numbersSize = (int)foundPlate->numbers.size();

				Mat num(Size(SAMPLESIZE*numbersSize, SAMPLESIZE), CV_8UC3, Scalar(255, 255, 255));

				for (int j = 0; j < numbersSize; j++) {
					Plate::Number* number = &foundPlate->numbers[j];
					number->canonicalize(SAMPLESIZE);

					if (mode & TRAIN)
						sample.push_back(number->canonical);

					OCR *ocr;
					/* 2번째를 문자로 설정 - 한글 */
					/* if (j == 2) */
					/* 4,5,6번째를 문자로 설정 - 영문 */
					if ((j == 4) || (j == 5) || (j == 6))
						ocr = &ocrChar;
					else
						ocr = &ocrNum;

					Mat feature = ocr->features(number->canonical, SAMPLESIZE);
					Mat output(1, ocr->numCharacters, CV_32FC1);

					ocr->predict(feature, output);
					str += ocr->classify(output);

					Rect numberArea = Rect(SAMPLESIZE*j, 0, SAMPLESIZE, SAMPLESIZE);
					cvtColor(number->canonical, num(numberArea), CV_GRAY2BGR);
					rectangle(num, numberArea, Scalar(0, 0, 255));
				}
				if (numbersSize)
					if (mode & WINDOWON) {
						imshow("Number", num);
						moveWindow("Number", WINDOW_X, 0 + k * SAMPLESIZE);
					}

				if (mode & PLATESTR)
					cout << "\t\t" << str << endl;

				if (mode & FINALDCS)
					dicider.decide(str);

				if (mode & ANALYSIS)
					analyzer.Analyze(str);

				if (mode & WINDOWON) {
					imshow("warp" /*+ to_string(i)*/, foundPlate->img);
					moveWindow("warp" /*+ to_string(i)*/, WINDOW_X, WINDOW_Y);
				}

				k++;
			}

			if (mode & COSTTIME) {
				cout << fixed;
				cout.precision(5);
				cout << "\tCost Time In a Cycle : " << (double)(clock() - cycleCost) / CLOCKS_PER_SEC << "s" << endl;
			}

			for (int i = 0; i < SEGMENTSIZE; i++)
				rectangle(image, area[i], Scalar(0, 0, 255), 1);

			imshow("image", image);
			moveWindow("image", 0, WINDOW_Y);

			if (mode & TRAIN)
				trainer.train(sample);

		}
	});

	camThread.join();
	procThread.join();

	return 0;
}
