#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "setServer.hpp"
#include "main.hpp"


#include <netdb.h>		// addrinfo struct
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fstream>      // for infile stream
#include <sstream>
#include <poll.h>

#define MAX_USERS 1024

class Client
{
    private:
//        struct sockaddr_storage clientAddress;
        struct addrinfo clientAddress;
        socklen_t  clientAddressLen;
        int clientSocket;
        struct pollfd fdList[MAX_USERS];


    public:
        Client(int);
        ~Client();
        void initClientSocket(int);
        void initFdList(int);
        void loop();
};

void TESTWEBSITE(int);
#endif
