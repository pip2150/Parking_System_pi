#ifndef PROCESS_HPP_
#define PROCESS_HPP_

/** @brief Process
	�������� ���� namespace
*/
namespace process {

	//! image�� �Է¹��� ��
	enum FROM {
		CAMERA = 0,		//!< Camera�� ����
		FILESYSTEM = 1	//!< File System���� ����
	};

	//! ī�޶� ����
	enum WAY {
		NONE = -1,		//!< NONE
		ENTER = 0,		//!< �Ա�
		EXIT = 1		//!< �ⱸ
	};

	//! ���α׷� Mode
	enum MODE {
		NETWORK = 0x01,			//!< Network Ȱ��ȭ
		OCRTRAIN = 0x02,		//!< OCRTrain Ȱ��ȭ
		SVMTRAIN = 0x04,		//!< SVMTrain Ȱ��ȭ
		COSTTIME = 0x08,		//!< �ҿ�ð� ǥ��
		PLATESTR = 0x10,		//!< �ҿ�ð� ǥ��
		WINDOWON = 0x20,		//!< �� ������ Windows�� ����
		ANALYSIS = 0x40,		//!< ����� ��� ����
		NOTUSEML = 0x80,		//!< �ΰ��Ű�� ������� �ʱ�
	};

#define SEGMENTSIZE 4	//!< ������ ����	
#define FROM CAMERA		
#define ESC 27

	/**
		@brief ������ ����
	*/
	struct ParkingInfo {
		WAY way;					//!< ������ ���Ա� ����
		int floor;					//!< ������ ��
		std::string zoneName;		//!< ���� ���� �̸�
	};

	/**
		@brief ���� ���� ����
	*/
	struct Table {
		std::string plateStr = "";		//!< ��ȣ�� �ؽ�Ʈ
		int match = 0;				//!< ��ġ�� Ƚ��
		bool sended = false;		//!< Sever ���� ����
	};

	/**
		@brief Server ���� ������
		@param info ������ ����
		@param table ���� ���� ����
	*/
	void send2Server(const ParkingInfo &info, Table table[SEGMENTSIZE + 1]);

	/**
		@brief ���� ��ġ ����
		@param area SEGMENTSIZE ������ �������� �簢�� ����
		@param position ��ȣ���� ��ǥ
		@return zoneIndex ����� ���� ��ġ
		position�� ������ area �߿� ���ԵǴ��� �˻��Ѵ�.
	*/
	int deductIndex(const cv::Rect area[SEGMENTSIZE], const cv::Point &position);

	/**
		@brief Debug�� ���� ���� ���� ���̺� ���
		@param table ����� ���̺� �迭 @see Table
	*/
	void printTable(Table table[SEGMENTSIZE + 1]);

	/**
		@brief Opencv �۾� ����
		@param width ���� ó���� �̹����� ���� ����
		@param height ���� ó���� �̹����� ���� ����
		@param mode ���� ó���� ���� ���
		@param info ���� ���� ����
		@param answer ��� ���� �Ǵ� ������� �Ʒ� �����Ϳ� �����ϱ� ���� ���� @see OCRTrainer
	*/
	int startOpencv(int width, int height, int mode, process::ParkingInfo info, std::string answer);
}

#endif