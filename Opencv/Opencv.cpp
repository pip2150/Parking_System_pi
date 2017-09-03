#include "Plate.hpp"
#include "Svm.hpp"
#include "Opencv.hpp"
#include <thread>
#include <mutex>
#include <unistd.h>
#include "../Network/psAPI.hpp"

using namespace cv;
using namespace std;

int startOpencv(int width, int height, int way, int floor, string zoneName, string answer, int mode) {

#if FROM == CAMERA
	VideoCapture camera;
	Mat cameraFrame;

	camera.open(0);
    while(!camera.isOpened()){
        camera.open(0);
		cerr << "Can Not Access The Camera." << endl;
        sleep(2);
    }
	camera.set(CV_CAP_PROP_FRAME_WIDTH, width);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, height);

#endif

	Mat image;
	Mat templ;
	Rect area[SEGMENTSIZE];

    OCR(CHARACTER+NUMBER);
	OCR *ocrChar;
	OCR *ocrNum;
	Svm *svm;

#pragma omp parallel
#pragma omp sections
	{
#pragma omp section
        if(!(mode & NOTUSEML))
    		ocrChar = new OCR(CHARACTER);
#pragma omp section
        if(!(mode & NOTUSEML))
            ocrNum = new OCR(NUMBER);
#pragma omp section
		svm = new Svm();
	}

#ifdef PSAPI_HPP_
#endif

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
			vector<Point> platePositions;

			Plate::find(image, possiblePlates, platePositions);

			vector<Mat> sample;
			int k = 0;
			int possiblePlatesSize = (int)possiblePlates.size();

			for (int i = 0; i < possiblePlatesSize; i++) {
				possiblePlates[i].setDebug(mode & WINDOWON);
				possiblePlates[i].canonicalize();
				int response = (int)svm->predict(possiblePlates[i].canonical);

				if (mode & WINDOWON)
					imshow("plate", possiblePlates[i].img);

				Mat svmdata;
				resize(possiblePlates[i].img, svmdata, Size(144, 33), 0, 0, INTER_CUBIC);
				if (mode & TRAIN)
					//svmtrainer.train(svmdata);
                    ;
				if (response != 1)
					continue;

				int section = 0;	// NULL

				Plate *foundPlate = &possiblePlates[i];
				double findNum_t = (double)getTickCount();
				bool isNumber = foundPlate->findNumbers(NUMSIZE);

				findNum_t = (double)getTickCount() - findNum_t;

#if FROM == CAMERA
				if (!isNumber)
					continue;
#endif
				if (mode & COSTTIME)
					cout << "\t\tCost Time In the FindNumbers : " << findNum_t * 1000 / getTickFrequency() << "ms" << endl;

				if (mode & POSITION)
					for (int j = 0; j < SEGMENTSIZE; j++)
						if (area[j].contains(platePositions[i])){
							cout << "\t\tIt's " << (section = j + 1) << "th Section." << endl;
                            circle(image, platePositions[i], 2, Scalar(0,0,255),2);
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
					/* j 번째를 문자로 설정 - 한글 : 2 영문 : {4 ,5, 6} */
					if ((j == 4) || (j == 5) || (j == 6))
						ocr = ocrChar;
					else
						ocr = ocrNum;

                    if(!(mode & NOTUSEML)){
                        Mat feature = ocr->features(number->canonical, SAMPLESIZE);
                        Mat output(1, ocr->numCharacters, CV_32FC1);

                        ocr->predict(feature, output);
                        str += ocr->classify(output);
                    }

					Rect numberArea = Rect(SAMPLESIZE*j, 0, SAMPLESIZE, SAMPLESIZE);
					cvtColor(number->canonical, num(numberArea), CV_GRAY2BGR);
					rectangle(num, numberArea, Scalar(0, 0, 255));
				}

				if (mode & WINDOWON)
					imshow("Number", num);
				if (mode & PLATESTR)
					cout << "\t\t" << str << endl;
                putText(image, str, platePositions[i], cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(255,0,0));

                if (dicider.decide(str)) {
#ifdef PSAPI_HPP_
                    if (mode & NETWORK) {
	                    ps::API api;
                        cout << "send to server" <<endl;
                        if (!floor) {
                            if (way == ENTER)
                                api.enter(str);
                            else if (way == EXIT)
                                api.exit(str);
                        }
                        else
                            api.parking(floor, zoneName, section, str);
                        cout << __LINE__ <<endl;
                        api.resopnse();
                    }
#endif
#ifndef PSAPI_HPP_
                    if (mode & NETWORK) {
                        cout << "send to server" <<endl;
                        if (!floor) {
                            if (way == ENTER)
                                system(("http_test enter "+str).c_str());
                            else if (way == EXIT)
                                system(("http_test exit "+str).c_str());
                        }
                        else{
                            cout << "http_test parking "+to_string(floor)+" "+zoneName+" "+to_string(section)+" "+str <<endl;
                            system(("http_test parking "+to_string(floor)+" "+zoneName+" "+to_string(section)+" "+str).c_str());
                        }
                    }
#endif
                    //std::cout << "\t\t\tThe answer is " << str << "  " << rand() % 256 << std::endl;
                }
				if (mode & ANALYSIS)
					analyzer.analyze(str);
				if (mode & WINDOWON)
					imshow("warp" /*+ to_string(i)*/, foundPlate->img);

				k++;
			}

			cycle_t = (double)getTickCount() - cycle_t;

			if (mode & COSTTIME)
				cout << "\tCost Time In a Cycle : " << cycle_t * 1000 / getTickFrequency() << "ms" << endl;
			if (mode & TRAIN)
				trainer.train(sample);

			for (int i = 0; i < SEGMENTSIZE; i++)
				rectangle(image, area[i], Scalar(0, 0, 255), 1);

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
