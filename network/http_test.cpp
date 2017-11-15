#include "serverAPI.hpp"

using namespace std;

void error(){
	cerr << "잘못된 입력입니다." << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	string arg;
	enum MODE { ENTER, EXIT, PARKING, LOAD, RESET };
	MODE mode;

	switch (argc) {
    case 2:
		arg = string(argv[1]);
		if (arg == "load")		mode = LOAD;
        else if (arg == "reset") mode = RESET;
        else error();
        break;
	case 3:
		arg = string(argv[1]);
		if (arg == "enter")		mode = ENTER;
		else if (arg == "exit")	mode = EXIT;
		else error();
		break;
    case 5:
    case 6:
		arg = string(argv[1]);
		if (arg == "parking")	mode = PARKING;
		else error();
		break;
	default:
		error();
	}

	ps::ServerAPI api("13.124.74.249", 3000);

    vector<string> texts;
	switch (mode) {
    case LOAD:
        api.loadAll(texts);
        for(auto text: texts)
            cout << text <<endl;
        return 0;
    case RESET:
        api.loadAll(texts);
        for(auto text: texts) {
            api.exit(text);
            api.resopnse();
        }
        return 0;
	case ENTER:
		api.enter(string(argv[2])); break;
	case EXIT:
		api.exit(string(argv[2])); break;
	case PARKING:
        if(argc == 6)
            api.parking(atoi(argv[2]),string(argv[3]), atoi(argv[4]), string(argv[5]));
        else
            api.parking(atoi(argv[2]),string(argv[3]), atoi(argv[4]), "");
		break;
	}

	return !api.resopnse();
}
