#include "Http.hpp"
#include "Socket.hpp"

using namespace std;

int main(){
	HttpMessage hm;
	string sendbuff ="";
	string recvbuff ="";
	string host= "13.124.74.249";

	ClientSocket csock = ClientSocket();

	hm.setRequest("POST", "/car");
	string jsondata =
			"{"
			"\"id\":\"88허1234\""
			"}";
	hm.setContent(jsondata);
	hm.setHeader(host);

	csock.connect(host, 3000);

	sendbuff = hm.getMessage();

	cout << sendbuff <<endl;
	csock.send(sendbuff);

	csock.recv(recvbuff);
	cout << recvbuff <<endl;
}
