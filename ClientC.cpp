#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#pragma warning(disable:4996)
#define MSG_MAX_SIZE 256
using namespace std;

//создание сокета
SOCKET Connection;

//функция приема сообщений от сервера
void ClientHandler() {
	char msg[MSG_MAX_SIZE];
	while (true) {
		recv(Connection, msg, sizeof(msg), NULL);
		cout << msg << endl;
	}
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

	//отправка сообщения серверу
	char sendmsg[MSG_MAX_SIZE];
	while (true) {
		//cout << "Your message: ";
		cin.getline(sendmsg, sizeof(sendmsg));
		send(Connection, sendmsg, sizeof(sendmsg), NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}