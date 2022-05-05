#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#include <string>
#include <stdlib.h>
#include <fstream>

#pragma warning(disable: 4996)

using namespace std;

int main(int argc, char* argv[]) {
	string UDP_TCP_IP = "127.0.0.1";
	string NameFile = "";
	string catalog = "temp";
	string message = "";
	string str = "";
	int TCP_Port = 5555;
	int UDP_Port;
	char msg[1500];
	if (argc > 1){
		UDP_TCP_IP = argv[1];
		TCP_Port = atoi(argv[2]);
		catalog = argv[3];
	}
	WSADATA wsaData;
	WORD version = MAKEWORD(2, 2);
	if (WSAStartup(version, &wsaData) != 0){
		cout << "Error lib" << endl;
		return 0;
	}
	SOCKADDR_IN server;
	int sizeofaddr = sizeof(server);
	server.sin_addr.s_addr = inet_addr(UDP_TCP_IP.c_str());
	server.sin_port = htons(TCP_Port);
	server.sin_family = AF_INET;
	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	cout << "Waiting for connection" << endl;
	while (connect(Connection, (SOCKADDR*)&server, sizeof(server)) != 0) {}
	cout << "Connected" << endl;
	recv(Connection, msg, sizeof(msg), NULL);
	NameFile = msg;
	recv(Connection, msg, sizeof(msg), NULL);
	UDP_Port = atoi(msg);
	SOCKADDR_IN serverHint;
	serverHint.sin_addr.s_addr = inet_addr(UDP_TCP_IP.c_str());
	serverHint.sin_port = htons(UDP_Port);
	serverHint.sin_family = AF_INET;
	SOCKET in = socket(AF_INET, SOCK_DGRAM, NULL);
	if (bind(in, (sockaddr*)&serverHint, sizeof(serverHint)) == SOCKET_ERROR){
		cout << "Error can`t bind socket" << WSAGetLastError() << endl;
		return 0;
	}
	SOCKADDR_IN client;
	int clientLength = sizeof(client);
	cout << "Receiving" << endl;
	while (true) {
		ZeroMemory(msg, sizeof(msg));
		if (recvfrom(in, msg, sizeof(msg), NULL, (sockaddr*)&client, &clientLength) == SOCKET_ERROR){
			cout << "Error receiving from client " << WSAGetLastError() << endl;
			send(Connection, "-1", 3, NULL);
			continue;
		}
		int id = -1;
		if (msg[0] != '0')id += (msg[0] - 48) * 1000 + 1;
		if (msg[1] != '0')id += (msg[1] - 48) * 100 + 1;
		if (msg[2] != '0')id += (msg[2] - 48) * 10 + 1;
		id += msg[3] - 48 + 1;
		for (int i = 4; msg[i] != '\0'; i++)str += msg[i];
		send(Connection, (to_string(id)).c_str(), (to_string(id)).size(), NULL);
		if (id == 0 || id == -1)break;
	}
	ofstream fout(NameFile + ".txt");
	ofstream fout2(catalog+R"(\)" + NameFile + ".txt");
	if (!fout.is_open()){
		cout << "Error file does not open" << endl;
		return 0;
	}
	if (!fout2.is_open()) {
		cout << "Error file does not open2" << endl;
		return 0;
	}
	fout << str;
	fout2 << str;
	fout.close();
	fout2.close(); 
	recv(Connection, msg, sizeof(msg), NULL);
	str = msg;
	if (str != "File has been sent")cout << "Error file was not sent" << endl;
	cout << "File has been sent" << endl;
	closesocket(Connection);
	closesocket(in);
	WSACleanup();
	system("pause");
	return 0;
}