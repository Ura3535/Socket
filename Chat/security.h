#pragma once
#include "shared.h"
#include <string>
#include <sstream>

template<typename HashFunc>
bool LamportAuthentication(bool isHost, const std::string& secretKey, const ICommunicator& communicator, HashFunc hash) {
	if (isHost) {
		std::string secret = hash(secretKey);
		for (int i = 1; i != T; ++i)
			secret = hash(secret);
		std::string msg;
		for (int i = 1; i != T; ++i)
		{
			msg = communicator.Recv();
			log("recv| key: "s + msg);
			std::istringstream iss(msg);
			std::string key;
			iss >> key;

			log("verify keys: "s + hash(key) + " "s + secret);
			if (hash(key) != secret) {
				communicator.Send("0");
				return false;
			}
			secret = key;
		}

		communicator.Send("1");
		return true;
	}
	else {
		std::string hashes[T] = { hash(secretKey) };
		for (int i = 1; i != T; ++i)
			hashes[i] = hash(hashes[i - 1]);
		for (int i = T - 2; i >= 0; --i)
		{
			std::string key = hashes[i];
			communicator.Send(key);
			log("send| key: "s + key);
		}

		std::string msg = communicator.Recv();
		return std::stoi(msg) == 1;
	}
}

std::string sha256(const std::string& input);
