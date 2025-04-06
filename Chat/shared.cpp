#include "shared.h"

void log(const std::string& msg)
{
	std::time_t time = ch::system_clock::to_time_t(ch::system_clock::now());
	char buffer[26];
	ctime_s(buffer, sizeof(buffer), &time);
	buffer[24] = '\0';
	std::ofstream out("log.txt", std::ios::app);
	out << '[' << buffer << "]: " << msg << '\n';
	out.close();
}