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

//глобальные файловые переменные
FILE* fb;
const char* filewrite = "SERVER_OUTPUT.txt";

//типы пакетов
enum Packet {
	P_ChatMessage,
	P_GetFile
};

void Writef(FILE *fb) {
	/*int msg_len; //16Kb
	do {
		char buffer = ' ';
		msg_len = recv(Connection, (char*)buffer, sizeof(buffer), NULL); //9
		fwrite(&buffer, 1, msg_len, fb);	
	} while (msg_len > 0);

	fclose(fb);
	*/
	int Elements;
	BYTE rmessage[4096]{ NULL };
	//while ((Elements = recv(Connection, (char*)rmessage, sizeof(rmessage), 0)) != NULL) { //9
	//	fwrite(rmessage, sizeof(BYTE), Elements, fb);
	//}
	Elements = recv(Connection, (char*)rmessage, sizeof(rmessage), 0);
	fwrite(rmessage, sizeof(BYTE), Elements, fb);
	fclose(fb);
}


bool OpenFile(const char* filename) {
	const char* folder = "./files/";
	string fullname(folder);
	fullname.append(filename);

	if ((fb = fopen(fullname.c_str(), "wb")) == NULL) {
		cout << "Cant open " << filename << endl;
		fclose(fb);
		return false;
	}
	else {
		return true;
	}
}

bool ProccessPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage: {
		/*-----------------RECEIVING MSG FROM SERVER----------------------------------*/
		int msg_size;

		recv(Connection, (char*)&msg_size, sizeof(int), NULL); //2
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';

		recv(Connection, msg, msg_size, NULL); //3
		cout << msg << endl;

		delete[] msg;
		break;
	}
	case P_GetFile: {
		/*------------RECEIVING MSG ABOUT FILE FOUND AND OPENED ON SERVER-------------*/
		int FisF_size;
		bool File_is_found;

		recv(Connection, (char*)&FisF_size, sizeof(int), NULL); //5
		recv(Connection, (char*)&File_is_found, FisF_size, NULL); //6

		if (File_is_found) {
			cout << "File found in system\n";
		}
		else {
			cout << "File isn't found in system\n";
			break;
		}
		/*----------------------RECEIVING A FILE FROM A SERVER-----------------------*/
		bool msg_is_readed = true;
		int mir_size = sizeof(bool);

		recv(Connection, (char*)&mir_size, sizeof(int), NULL); //7
		recv(Connection, (char*)&msg_is_readed, sizeof(bool), NULL); //8

		if (msg_is_readed) {
			cout << "Opening file to write...\n";
			if (!OpenFile(filewrite)) {
				cout << "Can't open file to write!\n";
				break;
			}

			cout << "Reading file from a server...\n";
			Writef(fb);
		}
		else {
			cout << "Can't read file from a server\n";
		}
		
		cout << "Successfully readed file from a server!\n";
		break;
	}
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
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL); //1-/-4

		if (!ProccessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}


int main() {
	setlocale(LC_ALL, "Russian");

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
		Packet packettype;
		
		getline(cin, sendmsg);

		//с-ния, начинающиеся с ! будут определены как чат
		if (sendmsg[0] == '!') {
			packettype = P_ChatMessage;
			sendmsg.erase(0, 1);
		}
		else {
			packettype = P_GetFile;
		}

		int	msg_size = sendmsg.size();

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