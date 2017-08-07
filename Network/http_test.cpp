#include "Http.hpp"
#include "Socket.hpp"
#include "psAPI.hpp"

using namespace std;

int main(int argc, char* argv[]){
	string sendbuff ="";
	string recvbuff ="";
	string host= "13.124.74.249";

	ClientSocket csock = ClientSocket();
	csock.connect(host, 3000);

	string plateNum = "88허1253";

	psapi::carEnter(csock, host, plateNum);
//	psapi::carExit(csock, host, plateNum);
//	psapi::carParking(csock, host, 1, "B", 1, "NULL");

	csock.recv(recvbuff);
	cout << "Recv : "<< endl <<recvbuff <<endl;

}
