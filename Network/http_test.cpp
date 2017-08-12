#include "psAPI.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	string mod;
	int statusCode;

	switch (argc) {
	case 3:
		mod = string(argv[1]);
		if (mod == "enter")		statusCode = 1;
		else if (mod == "exit")	statusCode = 2;
		else goto error;
		break;
	case 6:
		mod = string(argv[1]);
		if (mod == "parking")	statusCode = 3;
		else goto error;
		break;
	default:
	error:
		cerr << "잘못된 입력입니다." << endl;
		exit(1);
		break;
	}

	ps::API api;

	switch (statusCode) {
	case 1:
		api.enter(string(argv[2])); break;
	case 2:
		api.exit(string(argv[2])); break;
	case 3:
		api.parking(atoi(argv[2]),string(argv[3]), atoi(argv[4]), string(argv[5]));
	break;
	default:
		exit(1);
	}

	api.resopnse();
}
