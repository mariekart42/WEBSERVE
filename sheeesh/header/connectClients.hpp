#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "setServer.hpp"
#include "serverResponse.hpp"
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
        socklen_t  _clientAddressLen;

        struct addrinfo clientAddress;
        pollfd pollFds;

        std::vector<pollfd> v_fdList;


    public:
        ConnectClients();
        ~ConnectClients();

        void clientResponded(int);

//        void initClientSocket(int);
        void initFdList(int);

//        void loop(int);
        void connectClients(int);
};


void TESTWEBSITE(int);
#endif
