#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <string>
#include <cmath>
#include <stdlib.h>
#include <fstream>

#pragma warning(disable: 4996)

using namespace std;

int main(int argc, char* argv[]){
	string UDP_TCP_IP="127.0.0.1";
	string way="test.txt";
	string NameFile = "test";
	string message = "";
	int TCP_Port=5555 ;
	int UDP_Port=6000 ;
	int dl=500;
	int msgSize = 0;
	if (argc > 1){
		UDP_TCP_IP = argv[1];
		TCP_Port = atoi(argv[2]);
		UDP_Port = atoi(argv[3]);
		way = argv[4];
		dl = atoi(argv[5]);
	}
	char msg[1500];
	ifstream fin(way);
	if (!fin.is_open()) { 
		cout << "Error file does not open" << endl; 
		return 0;
	}
	fin >> message;
	fin.close();
	int repit = ceil(message.size() / 1495.0);
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2, 2);
	if (WSAStartup(DLLVersion, &wsaData) != 0){
		cout << "Error lib" << endl;
		return 0;
	}
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(UDP_TCP_IP.c_str());
	addr.sin_port = htons(TCP_Port);
	addr.sin_family = AF_INET;
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	if (bind(sListen, (SOCKADDR*)&addr, sizeofaddr) == SOCKET_ERROR){
		cout << "Error can`t bind socket " << WSAGetLastError() << endl;
		return 0;
	}
	listen(sListen, SOMAXCONN);
	cout << "Waiting for connection" << endl;
	SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);
	if (newConnection == 0) cout << "Error not connection" << endl;
	else {
		 cout << "Client connected" << endl;
		 for (int i = 0;i<NameFile.size();i++)msg[i]=NameFile[i];
		 msg[NameFile.size()] = '\0';
		 send(newConnection, msg, sizeof(msg), NULL);
		 Sleep(dl);
		 send(newConnection, (to_string(UDP_Port)).c_str(), to_string(UDP_Port).size(), NULL);
	}
	SOCKADDR_IN server_udp;
	server_udp.sin_addr.s_addr = inet_addr(UDP_TCP_IP.c_str());
	server_udp.sin_port = htons(UDP_Port);
	server_udp.sin_family = AF_INET;
	SOCKET out = socket(AF_INET, SOCK_DGRAM, NULL);
	cout << "Sending" << endl;
	for (int i = 0; i < repit; i++){
		ZeroMemory(msg, sizeof(msg));
		if ((repit - i - 1) < 10){//Не костыль а КОСТЫЛИЩЕЕЕЕЕЕЕЕЕЕЕЕ
			msg[0] = '0';//4 байта нужно чтобы число посылок для 10мб влезло
			msg[1] = '0';
			msg[2] = '0';
			msg[3] = (char)(((int)'0') + (repit - i - 1));
		}
		else if ((repit - i - 1) < 100){
			 msg[0] = '0';
			 msg[1] = '0';
			 msg[2] = (char)(((int)'0') + (repit - i - 1) / 10);
			 msg[3] = (char)(((int)'0') + (repit - i - 1) % 10);
		}
		else if ((repit - i - 1) < 1000){
			 msg[0] = '0';
			 msg[1] = (char)(((int)'0') + (repit - i - 1) / 100);
			 msg[2] = (char)(((int)'0') + ((repit - i - 1) / 10) % 10);
			 msg[3] = (char)(((int)'0') + (repit - i - 1) % 10);
		}
		else {
			 msg[0] = (char)(((int)'0') + (repit - i - 1) / 1000);
			 msg[1] = (char)(((int)'0') + ((repit - i - 1) / 100) % 10);
			 msg[2] = (char)(((int)'0') + ((repit - i - 1) / 10) % 10);
			 msg[3] = (char)(((int)'0') + (repit - i - 1) % 10);
		}
		msgSize += 4;
		for (; msgSize <= (message.size() + 4*(i+1)) && msgSize < (1495 * (i + 1)+4*(i+1)); msgSize++) msg[msgSize - 1495 * i-4 * i] = message[msgSize - 4*(i+1)];
		msg[msgSize - 1495 * i- 4 * i] = '\0';
		if (sendto(out,msg,sizeof(msg), NULL, (sockaddr*)&server_udp, sizeof(server_udp)) == SOCKET_ERROR){
			cout << "Error sending  " << WSAGetLastError() << endl;
			i--;
			continue;
		}
		Sleep(dl);
		ZeroMemory(msg, sizeof(msg));
		recv(newConnection, msg, sizeof(msg), NULL);
		if (atoi(msg) != (repit - i - 1)){
			cout << "Error delivery" << endl;
			i--;
			continue;
		}
	}
	send(newConnection, "File has been sent", 19, NULL);
	cout << "File has been sent" << endl;
	closesocket(sListen);
	closesocket(out);
	WSACleanup();
	system("pause");
    return 0;
}