#include <thread>
#include <mutex>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#else // On Windows
#include <stdlib.h>
#include "../network/psAPI.hpp"
#endif // On Linux

#include "plate.hpp"
#include "svm.hpp"
#include "ocr.hpp"
#include "process.hpp"
#include "tools.hpp"

using namespace cv;
using namespace std;

#ifndef PSAPI_HPP_
namespace ps {

	// Database Server API
	class API {

		// API Program 경로
		string path = "Network/http_test";

	public:

		// 차량 진입
		void enter(string str) {
			system((path + " enter " + str).c_str());
		}

		// 차량 퇴장
		void exit(string str) {
			system((path + " exit " + str).c_str());
		}

		// 차량 주차
		void parking(int floor, string zoneName, int i, string str) {
			system((path + " parking " + to_string(floor) + " " + zoneName + " " + to_string(i) + " " + str).c_str());
		}

		// Server 응답
		void resopnse() {
		}
	};
}
#endif

void process::send2Server(const ParkingInfo &info, Table table[SEGMENTSIZE]) {
	cout << "send to server" << endl;

	for (int i = 0; i < SEGMENTSIZE; i++) {
		/* 이미 Server에 보낸 경우*/
		if (table[i].sended)
			continue;

		/* 이미 Server에 보낸 경우*/
		if (table[i].match < tools::Dicider::LEASTMATCH)
			continue;

		ps::API api("13.124.74.249", 3000);

		if (!info.floor) {
			if (info.way == ENTER)
				api.enter(table[i].plateStr);
			else if (info.way == EXIT)
				api.exit(table[i].plateStr);
		}
		else
			api.parking(info.floor, info.zoneName, i, table[i].plateStr);

		api.resopnse();

		/* 주차 차량 정보 갱신 */

		table[i].sended = true;
	}

	cout << "sending complete" << endl;
}

bool process::deductIndex(const cv::Rect area[SEGMENTSIZE], const cv::Point &position, int *zoneIndex) {
	for (int j = 0; j < SEGMENTSIZE; j++)
		if (area[j].contains(position)) {
			cout << "\t\tIt's " << (*zoneIndex = 4 - j) << "th Section." << endl;
			return true;
		}

	return false;
}

int process::startOpencv(int width, int height, int mode, ParkingInfo info, std::string answer) {

#if FROM == CAMERA
	VideoCapture camera;
	Mat cameraFrame;

	/** Camera 열기 */
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

	Mat image;

	/** 주차 영역 */
	Rect area[SEGMENTSIZE];
	Scalar blue(255, 0, 0), red(0, 0, 255), white(255, 255, 255);

	cout << "Loading Machine learning Modules." << endl;

	OCR *ocrChar, *ocrNum;
	Svm *svm;

	/** OpenMP Thread 생성 */
#pragma omp parallel
#pragma omp sections
	{
#pragma omp section
		if (!(mode & NOTUSEML))
			ocrChar = new OCR(CHARACTER, OCR::READDT);
#pragma omp section
		if (!(mode & NOTUSEML))
			ocrNum = new OCR(NUMBER, OCR::READDT);
#pragma omp section
		svm = new Svm(Svm::READDT);
	}

	cout << "Loading was Complete." << endl;

	tools::Analyzer analyzer(answer);
	tools::Dicider dicider;
	OCRTrainer ocrtrainer(answer);
	SVMTrainer svmtrainer;

	mutex m;

	bool runing = true;

	/** Camera image 불러오는 Thread */
	thread camThread([&] {
		cout << "camThread start" << endl;
		while (runing) {
			m.try_lock();
			camera >> cameraFrame;
			m.unlock();
		}
	});

	/* image 처리 Thread */
	thread procThread([&] {
		cout << "procThread start" << endl;
		Table table[SEGMENTSIZE];

		/* ESC 입력 시 종료 */
		while (runing = (waitKey(50) != ESC)) {

#if FROM == CAMERA

			/* 하나의 Cycle 당 소요 시간*/
			double cycle_t = (double)getTickCount();

			m.try_lock();
			cameraFrame.copyTo(image);
			m.unlock();

			if (image.empty())
				continue;

			/* 각각의 주차 영역 계산 */
			for (int i = 0; i < SEGMENTSIZE; i++)
				area[i] = Rect(image.cols * i / SEGMENTSIZE, 0, image.cols / SEGMENTSIZE, image.rows);

#elif FROM == FILESYSTEM

			if (waitKey(0) == 27)
				break;

			int img_num;
			cout << "img_num : " << endl;
			cin >> img_num;
			if (!tools::readImage("InputImage/" + to_string(img_num) + ".jpg", image)) {
				cerr << "File No Exist." << endl;
				exit(1);
			}

#endif // File System 으로부터 입력받는 경우

			vector<Plate> possiblePlates;

			Plate::find(image, &possiblePlates);

			vector<Mat> sample;
			int k = 0;

			for (auto &plate : possiblePlates) {
				plate.setDebug(mode & WINDOWON);
				int response = (int)svm->predict(plate.canonical());

				if (mode & WINDOWON)
					imshow("plate", plate.img);
				/*
				if (mode & TRAIN)
					svmtrainer.train(plate.img);
				*/

				/** Svm을 통해 번호판 여부 확인 */
				if (response != 1)
					continue;

				/* 주차 영역 번호 */
				int zoneIndex = NULL;

				Plate &foundPlate = plate;

				/* 텍스트 추출 시 소요 시간 */
				double findText_t = (double)getTickCount();

				/* 추출된 텍스트가 TEXTSIZE 일치 여부 */
				bool isText = foundPlate.findTexts(TEXTSIZE);

				findText_t = (double)getTickCount() - findText_t;

#if FROM == CAMERA
				if (!isText)
					continue;
#endif // CAMERA로터 입력받는 경우

				if (mode & COSTTIME)
					cout << "\t\tCost Time In the FindTexts : " << findText_t * 1000 / getTickFrequency() << "ms" << endl;

				if (mode & POSITION)
					if (deductIndex(area, plate.position, &zoneIndex))
						circle(image, plate.position, 2, Scalar(0, 0, 255), 2);

				int textsSize = (int)foundPlate.texts.size();

				/* text 모음 - Debug 용 */
				Mat textCollection(Size(SAMPLESIZE*textsSize, SAMPLESIZE), CV_8UC3, white);

				/* 번호판의 텍스트 */
				string str;

				/* 추출된 텍스트를 OCR를 통해 검사 */
				for (int j = 0; j < textsSize; j++) {
					Plate::Text& text = foundPlate.texts[j];

					OCR *ocr;
					/*
						* 문자 OCR과 숫자 OCR을 할당
						* j 번째를 문자로 설정 - 한글 : { 2 } 영문 : {4 ,5, 6}
					*/
					if ((j == 4) || (j == 5) || (j == 6))
						ocr = ocrChar;
					else
						ocr = ocrNum;

					Mat canonical = text.canonical(SAMPLESIZE);

					if (mode & TRAIN)
						sample.push_back(canonical);
					if (!(mode & NOTUSEML)) {
						/* 정규화된 Text의 특징 */
						Mat feature = OCR::features(canonical, SAMPLESIZE);

						/* OCR에 의해 분석된 결과 */
						Mat output(1, ocr->numCharacters, CV_32FC1);

						ocr->predict(feature, &output);
						str += ocr->classify(&output);
					}

					Rect textArea = Rect(SAMPLESIZE*j, 0, SAMPLESIZE, SAMPLESIZE);
					cvtColor(canonical, textCollection(textArea), CV_GRAY2BGR);

					/* 붉은 사각형 hightlight */
					rectangle(textCollection, textArea, red);
				}

				/* 주차 차량 정보 갱신 */
				if (table[zoneIndex].plateStr == str)
					table[zoneIndex].match++;
				else {
					table[zoneIndex].plateStr = str;
					table[zoneIndex].match = 0;
					table[zoneIndex].sended = false;
				}

				/* Plate Text 문자열을 image에 넣기 */
				putText(image, str, plate.position, cv::FONT_HERSHEY_SIMPLEX, 1, blue);

				if (mode & PLATESTR)
					cout << "\t\t" << str << endl;
				/*
				if (dicider.decide(str)) {
					std::cout << "\t\t\tThe answer is " << str << "  " << rand() % 256 << std::endl;
				}
				*/
				if (mode & ANALYSIS)
					analyzer.analyze(str);
				if (mode & WINDOWON)
					imshow("Text", textCollection);
				if (mode & WINDOWON)
					imshow("warp" /*+ to_string(i)*/, foundPlate.img);

				k++;
			}

			cycle_t = (double)getTickCount() - cycle_t;

			if (mode & NETWORK)
				send2Server(info, table);
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