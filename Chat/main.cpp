//main.cpp

#define WIN32_LEAN_AND_MEAN

#include <fstream>
//#include <iostream>
#include "chat.h"

int main()
{
    try {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return 1;
        std::ofstream log("log.txt");

		std::string name;
		char status;
		std::string secretKey;
		bool isHost;
		std::cout << "Enter your name: ";
		std::cin >> name;
		std::cout << "Do you want to connect to another user [c] or wait for him to connect to you [s]: ";
		std::cin >> status;
		std::cout << "Enter the secret key: ";
		std::cin >> secretKey;
		switch (tolower(status))
		{
		case 'c':
			isHost = false;
			break;
		case 's':
			isHost = true;
			break;
		default:
			throw ;
			break;
		}

        Chat newChat(name, isHost);
        newChat.Connect(secretKey);
        newChat.Start();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << '\n';
	}
    WSACleanup();
	int a;
	std::cin >> a;
}
