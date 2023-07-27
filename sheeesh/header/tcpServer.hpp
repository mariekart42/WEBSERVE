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
#include <netdb.h>		// for addrinfo struct
#include <fstream>      // for infile stream

#define FAILURE -69
#define SUCCESS 0

// Colour shit
# define RED   "\x1B[31m"
# define GRN   "\x1B[32m"
# define YEL   "\x1B[33m"
# define BLU   "\x1B[34m"
# define MAG   "\x1B[35m"
# define CYN   "\x1B[36m"
# define RESET "\x1B[0m"

class tcpServer
{
	private:
		std::string myIpAddress;
		int myPort;
		int mySocket;		// fd that represents the socket
		int myNewSocket;
//		long myIncomingMessage;
		// struct sockaddr_in mySocketAddress; // fuggu we change to addrinfo struct lol
		struct addrinfo mySocketAddress;

        struct addrinfo *bindAddress;

        // unsigned int mySocketAddressLen;
        std::string myServerMessage;

		static std::string buildResponse();
        int initMySocket();

	public:
		tcpServer();
		tcpServer(std::string &, int);
		~tcpServer();

        static std::string readFile(const std::string&);

		int startServer();
		void startServerError();

		int startListen();
};

void logg(const std::string &);
void exitWithError(const std::string &);
std::string ipv4ToString(uint32_t addr);

#endif