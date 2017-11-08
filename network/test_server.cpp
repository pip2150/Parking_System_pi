#include "list.hpp"
#include "socket.hpp"

using namespace std;

int main(){
	ps::List list;
    cout << "Starting Server" <<endl;
	sock::ServerSocket server;

	int port = 3001;

	server.bind(port);
	server.listen();

	while(1){
		//@todo ---------- thread start
		string recvdata = "";
		string senddata = "";
		sock::ClientSocket client = server.accept();
        cout << "Client was accepted" <<endl;

		client.recv(&recvdata);

		//@todo 주차장 정보 가공
		string header = string(recvdata,0,5);
		string value = string(recvdata,5);

		if(header == "prnt@"){
			cout << "print" <<endl;

			senddata = list.xport();
		}
		else if(header == "entr@"){
			cout << "enter" <<endl;

			list.insert(value);

			senddata = "ok";
		}
		else if(header == "exit@"){
			cout << "Exit" <<endl;

			list.remove(value);

			senddata = "ok";
		}
		else{
			cout << "잘못된 헤더 입니다." << endl;

			senddata = "fail";
		}

		cout << "send : ";
		cout << senddata <<endl;
		//@todo 올바르게 보냈는지  답신
		client.send(senddata);

		//@todo -> atomic
		list.print();
		cout << list.xport() <<endl;
		//@todo -> atomic

		//@todo ---------- thread end
	}
}
