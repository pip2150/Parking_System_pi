#include <iostream>
#include <cstdlib>
#include <netdb.h>
#include <stdio.h>
#include <sstream>
#include "Socket.h"
#include "Http.h"

#define SERVER 1
#define CLIENT 0

#define SOCKET CLIENT

using namespace std;

#if SOCKET == SERVER
int main(){
	ServerSocket ssock = ServerSocket();
	int port = 80;

	ssock.create();
	ssock.bind(port);
	ssock.listen();

	while(true){
		HttpParser hp;
		HttpMessage hm;
		string recvbuff = "";
		string sendbuff = "";
		ClientSocket csock;

		ssock.accept(csock);

		csock.recv(recvbuff);

		hp.setMsg(recvbuff);
		cout << recvbuff <<endl;

		vector<vector<string> > result;
		hp.parserLine(result);
		for(size_t i=0;i<result.size();i++){
			for(size_t j=0;j<result[i].size();j++){
				cout << "<" << result[i].at(j) <<">";
			}
			cout <<endl;
		}

		hm.getMetaData("Date", hm.getCurrentTime());
		hm.getMetaData("Last-Modified", hm.getCurrentTime());
		string jsondata = "{\"car\":{\"numbering\":\"88Çã1234\",\"started_at\":1501217913},\"place\":{\"zonename\":\"A\",\"zone_index\":1,\"floor\":2}}\r\n";
		hm.setContent(jsondata);

		//sendbuff = hm.getHeader(csock);
		cout << sendbuff<<endl;
		csock.send(sendbuff);
	}
}
#elif SOCKET == CLIENT
int main(){
	HttpMessage hm;
	string sendbuff ="";
	string recvbuff ="";
	ClientSocket csock = ClientSocket();

	string host = "13.124.74.249";
	int port = 3000;

	hm.setRequest("POST", "/car");
	string jsondata =
			"{"
				"\"id\":\"88Çã1234\""
			"}";
	hm.setContent(jsondata);
	hm.setHeader(host);
	sendbuff = hm.getMessage();

	cout << csock.connect(host, port) <<endl;

	cout << sendbuff <<endl;

		csock.send(sendbuff);
		csock.recv(recvbuff);

	cout << recvbuff <<endl;
}
#endif
