#include "Opencv/Opencv.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	int mode = 0;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			char *arg = argv[i];
			int len = (int)strlen(arg);

			if ((len != 2) || (arg[0] != '-'))
				goto err;

			switch (arg[1]) {
			case 'n': mode |= NETWORK; break;
			case 'T': mode |= TRAIN; break;
			case 'p': mode |= POSITION; break;
			case 't': mode |= COSTTIME; break;
			case 's': mode |= PLATESTR; break;
			case 'w': mode |= WINDOWON; break;
			case 'a': mode |= ANALYSIS; break;
			case 'f': mode |= FINALDCS; break;
			case 'A': mode |= 0xFF ^ TRAIN; break;
			default: goto err; break;
			}
		}
		return startOpencv(mode);
	}
	else
		return startOpencv(WINDOWON | FINALDCS | PLATESTR);
err:
	cerr << "invalid input" << endl;
	exit(1);
}