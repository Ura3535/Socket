#pragma once
#include <string>
#include <WinSock2.h>
#include <iostream>
#include "shared.h"

struct Socket {
public:
	Socket();
	explicit Socket(const SOCKET&);
	Socket(const Socket&) = default;
	Socket(Socket&&) = default;
	~Socket();
protected:
	SOCKET sock;
};

struct ListeningSocket : public Socket {
	ListeningSocket() = default;
	explicit ListeningSocket(const SOCKET&);
	bool Bind();
	bool Listen(int i);
	SOCKET Accept();
};

struct ConnectionSocket : public Socket {
	ConnectionSocket() = default;
	explicit ConnectionSocket(const SOCKET&);
	ConnectionSocket(const ConnectionSocket&) = default;
	ConnectionSocket(ConnectionSocket&&) = default;
	bool Connect();
	bool ReSock(SOCKET);
	int Send(const char*) const;
	int Recv(char*) const;
};
