#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <netinet/in.h>

#define FAILURE -69
#define SUCCESS 0

class tcpServer
{
	private:
		std::string myIpAddress;
		int myPort;
		int mySocket;		// fd that represents the socket
		int myNewSocket;
		long myIncomingMessage;
		struct sockaddr_in mySocketAddress;
        unsigned int mySocketAddressLen;
        std::string myServerMessage;

		std::string buildResponse();

	public:
		tcpServer();
		tcpServer(std::string, int);
		~tcpServer();

		int startServer();
		void startServerError();


		void startListen();

};

void logg(const std::string &);
void exitWithError(const std::string &);
std::string ipv4ToString(uint32_t addr);

#endif