#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma warning(disable:4996)
#define MSG_MAX_SIZE 256
using namespace std;

//создание сокета
SOCKET Connection;

//типы пакетов
enum Packet {
	P_ChatMessage,
	P_Test
};

bool ProccessPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage: {
		int msg_size;

		//прием размера сообщения
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';

		//прием сообщения
		recv(Connection, msg, msg_size, NULL);
		cout << msg << endl;

		delete[] msg;
		break;
	}
	case P_Test:
		std::cout << "Test packet.\n";
		break;

	default: {
		cout << "Unrecognized packet: " << packettype << endl;
		break;
	}
	}

	return true;
}

//функция приема сообщений от сервера
void ClientHandler() {
	Packet packettype;
	int msg_size;
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProccessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}


int main() {
	WSAData wsadata;
	WORD DLLVersion = MAKEWORD(2, 1);

	//запуск протокола WSAStartup и проверка запуска
	if (WSAStartup(DLLVersion, &wsadata) != 0) {
		cerr << "no wsa cry :(" << endl;
		exit(1);
	}

	//настройка сокета, адреса сокета
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	//определение сокета подключения к серверу
	Connection = socket(AF_INET, SOCK_STREAM, NULL);

	//проверка подключения к серверу
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cerr << "Failed to connect!\n";
		return 1;
	}
	cout << "Connected to server!\n";

	//создание нового потока приема сообщений от сервера
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	//команда выхода из сокета(BETA)
	string exit_command = "exit";
	int ex_comm_size = exit_command.size();

	//отправка сообщения серверу
	string sendmsg;
	while (true) {
		getline(cin, sendmsg);

		int	msg_size = sendmsg.size();

		Packet packettype = P_ChatMessage;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);

		send(Connection, (char*)&msg_size, sizeof(int), NULL); //размер принимаемой строки
		send(Connection, sendmsg.c_str(), msg_size, NULL); 
		Sleep(10);

		if (sendmsg == exit_command) {
			cout << "Exiting...";
			break;
		}
	}

	/*closesocket(Connection);
	WSACleanup();*/

	system("pause");
	return 0;
}