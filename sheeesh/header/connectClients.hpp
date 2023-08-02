#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "setServer.hpp"
#include "HandleClientRequest.hpp"
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
#include <vector>

#define MAX_USERS 1024

class ConnectClients
{
    private:
        int _clientSocket;
        struct addrinfo _clientAddress;
        socklen_t  _clientAddressLen;
        struct pollfd _fdList[MAX_USERS];

    public:
        ConnectClients();
        ~ConnectClients();

        void clientConnected(int);
        void initFdList(int);
        void connectClients(int);
};


void TESTWEBSITE(int);
#endif
