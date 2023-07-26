#include "../header/tcpServer.hpp"

void logg(const std::string &message)
{
	std::cout << message << std::endl;
}

void exitWithError(const std::string &msg)
{
	logg(RED"ERROR: " + msg + RESET);
	exit(1);
}

// Function to convert IPv4 address (32-bit integer) to string
std::string ipv4ToString(uint32_t addr) 
{
    std::stringstream ss;
    ss << (addr >> 24 & 0xFF) << "." << (addr >> 16 & 0xFF)
       << "." << (addr >> 8 & 0xFF) << "." << (addr & 0xFF);
    return ss.str();
}