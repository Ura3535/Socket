#include "chat.h"
#include <thread>
#include <fstream>
#include <chrono>
#include <functional>
#include <sstream>
#include "security.h"

enum class COMMAND
{
	EXIT,
	SEND,
	UNKNOWN
};

template <typename EnumType>
char enumToChar(EnumType e) {
	return static_cast<int>(e) + '0';
}

template <typename EnumType>
EnumType charToEnum(char c) {
	return static_cast<EnumType>(c - '0');
}

static COMMAND commandProcessing(std::string command)
{
	for (int i = 0; i != command.size(); ++i)
		command[i] = tolower(command[i]);

	if (command == "send") return COMMAND::SEND;
	if (command == "exit") return COMMAND::EXIT;
	return COMMAND::UNKNOWN;
}

inline static std::string pathChat(const char* name) {
	return name + "_chat.txt"s;
}

//struct Chat

Chat::Chat(const std::string& name, bool serverStatus)
	: Name(name.c_str())
	, InterlocutorName("")
	, connection(false)
	, isHost(serverStatus)
	, communicator(nullptr)
{
}

void Chat::Connect(const std::string& secretKey) {
	if (isHost)
	{
		ListeningSocket ListenSocket;
		ListenSocket.Bind();
		ListenSocket.Listen(1);

		std::cout << "Wait for the interlocutor to connect...\n"; 
		communicator = new ChatCommunicator(std::move(ConnectionSocket(ListenSocket.Accept())), false);
		std::cout << "The interlocutor has connected\n";

	}
	else
	{
		communicator = new ChatCommunicator(std::move(ConnectionSocket()), true);
	}

	   bool isVerify = LamportAuthentication(isHost, secretKey, *communicator, sha256);

	   if (isVerify) {
	   	std::cout << "Verify\n";
	   }
	   else
	   {
	   	std::cout << "Not verify\n";
	   	return;
	   }

	log("Connect"s);
	communicator->Send(Name);
	InterlocutorName = communicator->Recv();
	std::cout << "Connect to " << InterlocutorName << '\n';
	connection = true;
	clearChat();
}

void Chat::Start()
{
	if (!connection) {
		return;
	}
	clearChat();
	std::thread RECV([this]() {
		std::string msg;
		while (this->connection) {
			msg = this->communicator->Recv();
			if (msg.empty())
				this->connection = false;
			print(msg, 0);
		}
		});
	std::string command = "";
	std::string msg = "";
	while (connection) {
		std::cin >> command;
		if (!connection)
			break;

		COMMAND processedCommand = commandProcessing(command);

		switch (processedCommand)
		{
		case COMMAND::SEND:
		{
			std::getline(std::cin, msg);
			communicator->Send(msg);
			break;
		}

		case COMMAND::EXIT:
		{
			communicator->Send({ enumToChar<HEAD>(HEAD::EXIT) });
			log("send| exit");
			connection = false;
			break;
		}

		default:
		{
			std::cout << "'" << command << "' is an unknown command\n";
			break;
		}
		}
	}

	RECV.join();
	delete communicator;
}


void Chat::clearChat()
{
	std::ofstream out(pathChat(Name.c_str()));
	out.close();
}

void Chat::print(const std::string& msg, bool who)
{
	std::ofstream out(pathChat(Name.c_str()), std::ios::app);
	out << (who ? Name : InterlocutorName) << ": " << msg << '\n';
	out.close();
}


//struct Comunicator
ChatCommunicator::ChatCommunicator(ConnectionSocket&& interlocutor, bool tryConnect)
	: Interlocutor(std::move(interlocutor))
{
	if(tryConnect)
		log("try to connect to server: "s + std::to_string(Interlocutor.Connect()));
}

void ChatCommunicator::Send(std::string msg) const
{
	char buf[SOCKET_BUF_SIZE];
	while (msg.size() > SOCKET_BUF_SIZE - 1) {
		msg = enumToChar<HEAD>(HEAD::MESS_PIECE) + msg;
		buf[msg.copy(buf, SOCKET_BUF_SIZE - 1, 0)] = '\0';
		msg.erase(0, SOCKET_BUF_SIZE - 1);
		Interlocutor.Send(buf);
		log("send| message (piece): "s + buf);
	}
	msg = enumToChar<HEAD>(HEAD::MESS_LAST_PIECE) + msg;
	buf[msg.copy(buf, msg.size(), 0)] = '\0';
	Interlocutor.Send(buf);
	log("send| message (last piece): "s + buf);
}

std::string ChatCommunicator::Recv() const
{
	std::string msg = "";
	char buf[SOCKET_BUF_SIZE];
	while (true) {
		(Interlocutor.Recv(buf));
		switch (charToEnum<HEAD>(buf[0]))
		{
		case HEAD::EXIT:
		{
			log("recv| exit: "s + buf);
			return "";
		}

		case HEAD::MESS_PIECE:
		{
			log("recv| message (piece): "s + buf);
			msg.append(buf + 1);
			break;
		}

		case HEAD::MESS_LAST_PIECE:
		{
			log("recv| message (last piece): "s + buf);
			msg.append(buf + 1);
			return msg;
		}

		default: return "";
		}
	}
}