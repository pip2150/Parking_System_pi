#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#include "OCR.hpp"
#include "Plate.hpp"
#include "Svm.hpp"
#include "Utils.hpp"
#include <thread>
#include <ctime>

#define CAMERA 0
#define FILESYSTEM 1
#define TRUE 1
#define FALSE 0

/* ----- Debug Setting ----- */
#define SEGMENTSIZE 1
#define MAXMATCH 10
#define FROM CAMERA
//#define CAMERA		0x01
#define NETWORK		0x01
#define TRAIN		0x02
#define POSITION 	0x04
#define COSTTIME 	0x08
#define PLATESTR 	0x10
#define WINDOWON 	0x20
#define ANALYSIS	0x40
#define FINALDCS	0x80
/* ------------------------- */

int startOpencv(int mode);

#endif