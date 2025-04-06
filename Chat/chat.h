#pragma once
#include "sock.h"


enum class HEAD
{
	EXIT,
	MESS_PIECE,
	MESS_LAST_PIECE,
	UNKNOWN
};

class Chat {
public:
	Chat(const std::string& name, bool serverStatus);
	void Connect(const std::string& secretKey);
	void Start();
private:
	void clearChat();
	void print(const std::string&, bool);

	std::string Name;
	std::string InterlocutorName;
	bool connection;
	bool isHost;
	ICommunicator* communicator;
};

struct ChatCommunicator : ICommunicator {
public:
	ChatCommunicator(ConnectionSocket&&, bool tryConnect);
	void Send(std::string) const override;
	std::string Recv() const override;
private:
	ConnectionSocket Interlocutor;
};