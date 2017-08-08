#include "Http.hpp"
#include "Socket.hpp"
#include "psAPI.hpp"

using namespace std;

int main(int argc, char* argv[]){
	string mod;
	int statusCode;

	if(argc > 1){
		mod = string(argv[1]);
		if(argc == 3){
			if(mod == "enter")
				statusCode =1 ;
			else if(mod == "exit")
				statusCode =2 ;
		}
		else if(argc == 6){
			if(mod == "parking")
				statusCode =3 ;
			else
				goto error;
		}
		else
			goto error;
	}
	else{
		error:
		cerr << "잘못된 입력입니다." << endl;
		exit(1);
	}

	string sendbuff ="";
	string recvbuff ="";
	string host= "13.124.74.249";

	ClientSocket csock = ClientSocket();
	csock.connect(host, 3000);

	switch(statusCode){
	case 1:
		psapi::enter(csock, host, string(argv[2])); break;
	case 2:
		psapi::exit(csock, host, string(argv[2])); break;
	case 3:
		psapi::parking(csock, host, atoi(argv[2]),
				string(argv[3]), atoi(argv[4]), string(argv[5])); break;
	default :
		exit(1);
	}

	csock.recv(recvbuff);
	cout << "Recv : "<< endl <<recvbuff <<endl;

	return 0;
}



