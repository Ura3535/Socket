#pragma once
#include "shared.h"
#include <string>
#include <sstream>

bool Verify(bool isHost, const std::string& secretKey, const ICommunicator& communicator) {
	const int t = 5;
	std::hash<size_t> hash;
	std::hash<std::string> hashStr;
	if (isHost) {
		size_t secret = hashStr(secretKey);
		for (int i = 1; i != t; ++i)
			secret = hash(secret);
		std::string buf;
		for (int i = 1; i != t; ++i)
		{
			buf = communicator.Recv();
			log("recv| key: "s + buf);
			std::istringstream iss(buf);
			size_t key;
			iss >> key;

			log("verify keys: "s + std::to_string(hash(key)) + " "s + std::to_string(secret));
			if (hash(key) != secret) {
				return false;
			}
			secret = key;
		}
	}
	else {
		size_t hashs[t] = { hashStr(secretKey) };
		for (int i = 1; i != t; ++i)
			hashs[i] = hash(hashs[i - 1]);
		for (int i = t - 2; i >= 0; --i)
		{
            std::string key = std::to_string(hashs[i]);
            communicator.Send(key);
            log("send| key: "s + key);
		}
	}

	return true;
}