// sock.cpp

#include "sock.h"
#include <Ws2tcpip.h>
#include <fstream>
#include <iostream>

#define DEFAULT_PROTOCOL IPPROTO_TCP

static const std::string MyIP = "192.168.0.103";
static const std::string InterlocutorIP = "192.168.0.103";
static const int DefaultPort = 1029;

static std::wstring getIP(bool isServer) {
    std::string ip = isServer ? MyIP : InterlocutorIP;

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, ip.c_str(), -1, NULL, 0);
    std::wstring wip(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, ip.c_str(), -1, &wip[0], size_needed);

    return wip;
}

static sockaddr_in createService(bool isServer) {
    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_port = htons(DefaultPort);

    std::wstring wip = getIP(isServer);
    if (InetPton(AF_INET, wip.c_str(), &service.sin_addr) != 1) {
        std::cerr << "InetPton failed with error: " << WSAGetLastError() << "\n";
    }

    return service;
}

static const sockaddr_in ClientService = createService(false);
static const sockaddr_in ServerService = createService(true);

// ---- Socket ----

Socket::Socket()
    : sock(socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL))
{
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << "\n";
    }
}

Socket::Socket(const SOCKET& other)
    : sock(other)
{
}

Socket::~Socket()
{
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

// ---- ListeningSocket ----

ListeningSocket::ListeningSocket(const SOCKET& other)
    : Socket(other)
{
}

bool ListeningSocket::Bind() {
    int result = bind(sock, reinterpret_cast<const sockaddr*>(&ServerService), sizeof(ServerService));
    if (result == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << "\n";
    }
    return result == 0;
}

bool ListeningSocket::Listen(int backlog) {
    int result = listen(sock, backlog);
    if (result == SOCKET_ERROR) {
        std::cerr << "Listen failed: " << WSAGetLastError() << "\n";
    }
    return result == 0;
}

SOCKET ListeningSocket::Accept() {
    SOCKET accepted = accept(sock, NULL, NULL);
    if (accepted == INVALID_SOCKET) {
        std::cerr << "Accept failed: " << WSAGetLastError() << "\n";
    }
    return accepted;
}

// ---- ConnectionSocket ----

ConnectionSocket::ConnectionSocket(const SOCKET& other)
    : Socket(other)
{
}

bool ConnectionSocket::Connect() {
    int result = connect(sock, reinterpret_cast<const sockaddr*>(&ClientService), sizeof(ClientService));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << "\n";
    }
    return result == 0;
}

bool ConnectionSocket::ReSock(SOCKET other) {
    if (sock != INVALID_SOCKET) {
        if (closesocket(sock) == SOCKET_ERROR) {
            std::cerr << "Closing socket failed: " << WSAGetLastError() << "\n";
            return false;
        }
    }
    sock = other;
    return true;
}

int ConnectionSocket::Send(const char* buf) const {
    int result = send(sock, buf, SOCCKET_BUF_SIZE, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << "\n";
    }
    return result;
}

int ConnectionSocket::Recv(char* buf) const {
    int result = recv(sock, buf, SOCCKET_BUF_SIZE, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Recv failed: " << WSAGetLastError() << "\n";
    }
    return result;
}
