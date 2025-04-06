#pragma once
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

namespace ch = std::chrono;
using namespace std::string_literals;

const size_t SOCCKET_BUF_SIZE = 21;

void log(const std::string& msg);

class ICommunicator
{
public:
	virtual void Send(std::string) const = 0;
	virtual std::string Recv() const = 0;
	virtual ~ICommunicator() = default;
};