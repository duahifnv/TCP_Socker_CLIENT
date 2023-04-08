#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable:4996)
#define MSG_MAX_SIZE 256
using namespace std;

//�������� ������
SOCKET Connection;

//������� ������ ��������� �� �������
void ClientHandler() {
	int msg_size;
	while (true) {
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';

		recv(Connection, msg, msg_size, NULL);
		cout << msg << endl;

		delete[] msg;
	}
}


int main() {
	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 1);

	//������ ��������� WSAStartup � �������� �������
	if (WSAStartup(DLLVersion, &wsadata) != 0) {
		cerr << "no wsa cry :(" << endl;
		exit(1);
	}

	//��������� ������, ������ ������
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	//����������� ������ ����������� � �������
	Connection = socket(AF_INET, SOCK_STREAM, NULL);

	//�������� ����������� � �������
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cerr << "Failed to connect!\n";
		return 1;
	}
	cout << "Connected to server!\n";

	//�������� ������ ������ ������ ��������� �� �������
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	//�������� ��������� �������
	string sendmsg;
	while (true) {
		//cout << "Your message: ";
		getline(cin, sendmsg);
		int	msg_size = sendmsg.size();
		send(Connection, (char*)&msg_size, sizeof(int), NULL); //������ ����������� ������
		send(Connection, sendmsg.c_str(), msg_size, NULL); 
		Sleep(10);
	}

	system("pause");
	return 0;
}