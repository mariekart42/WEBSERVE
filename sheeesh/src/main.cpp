#include "../header/tcpServer.hpp"

int main()
{
    std::string IPAddress = "0.0.0.0";
	tcpServer obj(IPAddress, 8080);
	obj.startListen();


	std::cout << "main something" << std::endl;
}
