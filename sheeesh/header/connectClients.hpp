#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"

#include <netdb.h>		// addrinfo struct
#include <poll.h>       // pollfd struct


#define MAX_USERS 1024
#define BUFFER_SIZE 8000

class ConnectClients
{
    private:
        int _clientSocket;
        struct addrinfo _clientAddress;
        socklen_t  _clientAddressLen;
        struct pollfd _fdList[MAX_USERS];
//        int _statusCode;

    public:
        ConnectClients();
        ~ConnectClients();

        void clientConnected(int);
        void initFdList(int);
        void connectClients(int);
};


void TESTWEBSITE(int);
#endif
