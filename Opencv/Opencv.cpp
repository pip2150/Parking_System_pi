#include <thread>
#include <mutex>
#include <ctime>
#ifdef _WIN32
#include <windows.h>
#else
#include <stdlib.h>
#include "../Network/psAPI.hpp"
#endif
#include "Plate.hpp"
#include "Svm.hpp"
#include "OCR.hpp"
#include "Opencv.hpp"
#include "Tools.hpp"

using namespace cv;
using namespace std;

#ifndef PSAPI_HPP_
namespace ps {
	class API {
		string path = "Network/http_test";
	public:
		void enter(string str) {
			system((path + " enter " + str).c_str());
		}
		void exit(string str) {
			system((path + " exit " + str).c_str());
		}
		void parking(int floor, string zoneName, int i, string str) {
			system((path + " parking " + to_string(floor) + " " + zoneName + " " + to_string(i) + " " + str).c_str());
		}
		void resopnse() {
		}
	};
}
#endif

struct Table {
	string plateStr = "";
	int match = 0;
	bool sended = false;
};

void send2Server(Table table[SEGMENTSIZE], ParkingInfo info) {
	cout << "send to server" << endl;

	for (int i = 0; i < SEGMENTSIZE; i++) {
		if (table[i].sended)
			continue;

		if (table[i].match < MAXMATCH)
			continue;

		ps::API api;

		if (!info.floor) {
			if (info.way == ENTER)
				api.enter(table[i].plateStr);
			else if (info.way == EXIT)
				api.exit(table[i].plateStr);
		}
		else
			api.parking(info.floor, info.zoneName, i, table[i].plateStr);

		api.resopnse();

		table[i].sended = true;
	}

	cout << "sending complete" << endl;
}

bool deductIndex(Rect area[SEGMENTSIZE], Point &position, int *zoneIndex) {
	for (int j = 0; j < SEGMENTSIZE; j++)
		if (area[j].contains(position)) {
			cout << "\t\tIt's " << (*zoneIndex = 4 - j) << "th Section." << endl;
			return true;
		}
	return false;
}

int startOpencv(int width, int height, ParkingInfo info, string answer, int mode) {

#if FROM == CAMERA
	VideoCapture camera;
	Mat cameraFrame;

	camera.open(0);
	while (!camera.isOpened()) {
		camera.open(0);
		cerr << "Can Not Access The Camera." << endl;
#ifdef _WIN32
		Sleep(2);
#else
		sleep(2);
#endif        
	}
	camera.set(CV_CAP_PROP_FRAME_WIDTH, width);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);

#endif

	Mat image, templ;
	Rect area[SEGMENTSIZE];
	Scalar blue(255, 0, 0), red(0, 0, 255), white(255, 255, 255);

	cout << "Loading Machine learning Modules." << endl;

	/*OCR(CHARACTER + NUMBER, WRITEDT);*/
	OCR *ocrChar, *ocrNum;
	Svm *svm;

#pragma omp parallel
#pragma omp sections
	{
#pragma omp section
		if (!(mode & NOTUSEML))
			ocrChar = new OCR(CHARACTER, READDT);
#pragma omp section
		if (!(mode & NOTUSEML))
			ocrNum = new OCR(NUMBER, READDT);
#pragma omp section
		svm = new Svm(READDT);
	}

	cout << "Loading was Complete." << endl;

	tools::Analyzer analyzer(answer);
	tools::OCRTrainer ocrtrainer(answer);
	tools::SVMTrainer svmtrainer;
	tools::Dicider dicider;

	mutex m;

	bool runing = true;

	thread camThread([&] {
		cout << "camThread start" << endl;
		while (runing) {
			m.try_lock();
			camera >> cameraFrame;
			m.unlock();
		}
	});

	thread procThread([&] {
		cout << "procThread start" << endl;
		Table table[SEGMENTSIZE];

		while (runing = (waitKey(50) != 27)) {

#if FROM == CAMERA
			double cycle_t = (double)getTickCount();

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
			vector<Plate> possiblePlates;

			Plate::find(image, &possiblePlates);

			vector<Mat> sample;
			int k = 0;

			for (auto &plate : possiblePlates) {
				plate.setDebug(mode & WINDOWON);
				plate.canonicalize();
				int response = (int)svm->predict(plate.canonical);

				if (mode & WINDOWON)
					imshow("plate", plate.img);
				/*if (mode & TRAIN)
					svmtrainer.train(plate.img);*/

				if (response != 1)
					continue;

				int zoneIndex = 0;	// NULL

				Plate &foundPlate = plate;
				double findNum_t = (double)getTickCount();
				bool isNumber = foundPlate.findNumbers(NUMSIZE);

				findNum_t = (double)getTickCount() - findNum_t;

#if FROM == CAMERA
				if (!isNumber)
					continue;
#endif
				if (mode & COSTTIME)
					cout << "\t\tCost Time In the FindNumbers : " << findNum_t * 1000 / getTickFrequency() << "ms" << endl;

				if (mode & POSITION)
					if(deductIndex(area, plate.position, &zoneIndex))
						circle(image, plate.position, 2, Scalar(0, 0, 255), 2);

				int numbersSize = (int)foundPlate.numbers.size();
				Mat num(Size(SAMPLESIZE*numbersSize, SAMPLESIZE), CV_8UC3, white);
				string str;

				for (int j = 0; j < numbersSize; j++) {
					Plate::Number& number = foundPlate.numbers[j];
					number.canonicalize(SAMPLESIZE);

					OCR *ocr;
					/* j 번째를 문자로 설정 - 한글 : 2 영문 : {4 ,5, 6} */
					if ((j == 4) || (j == 5) || (j == 6))
						ocr = ocrChar;
					else
						ocr = ocrNum;

					if (mode & TRAIN)
						sample.push_back(number.canonical);
					if (!(mode & NOTUSEML)) {
						Mat feature = ocr->features(number.canonical, SAMPLESIZE);
						Mat output(1, ocr->numCharacters, CV_32FC1);

						ocr->predict(feature, &output);
						str += ocr->classify(&output);
					}

					Rect numberArea = Rect(SAMPLESIZE*j, 0, SAMPLESIZE, SAMPLESIZE);
					cvtColor(number.canonical, num(numberArea), CV_GRAY2BGR);
					rectangle(num, numberArea, red);
				}

				if (table[zoneIndex].plateStr == str)
					table[zoneIndex].match++;
				else {
					table[zoneIndex].plateStr = str;
					table[zoneIndex].match = 0;
					table[zoneIndex].sended = false;
				}

				putText(image, str, plate.position, cv::FONT_HERSHEY_SIMPLEX, 1, blue);

				if (mode & PLATESTR)
					cout << "\t\t" << str << endl;
				/*if (dicider.decide(str)) {
					std::cout << "\t\t\tThe answer is " << str << "  " << rand() % 256 << std::endl;
				}*/
				if (mode & ANALYSIS)
					analyzer.analyze(str);
				if (mode & WINDOWON)
					imshow("Number", num);
				if (mode & WINDOWON)
					imshow("warp" /*+ to_string(i)*/, foundPlate.img);

				k++;
			}

			cycle_t = (double)getTickCount() - cycle_t;

			if (mode & NETWORK)
				send2Server(table, info);
			if (mode & COSTTIME)
				cout << "\tCost Time In a Cycle : " << cycle_t * 1000 / getTickFrequency() << "ms" << endl;
			if (mode & TRAIN)
				ocrtrainer.train(sample);

			for (int i = 0; i < SEGMENTSIZE; i++)
				rectangle(image, area[i], red, 1);

			imshow("image", image);

		}
	});

	camThread.join();
	procThread.join();

	delete ocrChar;
	delete ocrNum;
	delete svm;

	return 0;
}