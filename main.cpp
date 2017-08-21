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
			case 'w': mode |= WINDOWON; break;
			case 'a': mode |= ANALYSIS; break;
			case 'f': mode |= FINALDCS; break;
			default: goto err; break;
			}
		}
		return startOpencv(mode);
	err:
		cerr << "invalid input" << endl;
		exit(1);
	}
	else
		return startOpencv(WINDOWON | ANALYSIS);
}