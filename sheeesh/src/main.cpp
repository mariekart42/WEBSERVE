#include "../header/tcpServer.hpp"

int main()
{
	tcpServer obj("0.0.0.0", 8080);
	obj.startListen();

	std::cout << "main something" << std::endl;
}
