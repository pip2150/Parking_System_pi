#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#define TRUE 1
#define FALSE 0

//* Camera�� ����
#define CAMERA 0
//* File System���� ����
#define FILESYSTEM 1

/* ----- Debug Setting ----- */

//* �Ա�
#define ENTER 0
//* �ⱸ
#define EXIT 1

//* ������ ����
#define SEGMENTSIZE 4

//* image�� �Է¹��� ��
#define FROM CAMERA

//* Network Ȱ��ȭ
#define NETWORK		0x01

//* Train Ȱ��ȭ
#define TRAIN		0x02

//* Position ǥ��
#define POSITION 	0x04

//* �ҿ�ð� ǥ��
#define COSTTIME 	0x08

//* Plate�� ����� ���ڿ� ǥ��
#define PLATESTR 	0x10

//* �� ������ Windows�� ����
#define WINDOWON 	0x20

//* ����� ��� ����
#define ANALYSIS	0x40

//* �ΰ��Ű�� ������� �ʱ�
#define NOTUSEML	0x80

/* ------------------------- */

/** �����忡 ���� ����
*/
struct ParkingInfo {
	//* ������ ���Ա� ����
	int way;
	//* ������ ��
	int floor;
	//* ���� ���� �̸�
	std::string zoneName;
};

/** Opencv �۾� ����
*/
int startOpencv(int width, int height, ParkingInfo parkingInfo, std::string answer, int mode);

#endif