#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"

#include <netdb.h>		// addrinfo struct
#include <poll.h>       // pollfd struct


#define DEBUG


#define MAX_USERS 10
// #define BUFFER_SIZE 8000
#define DATA_TO_READ (_fdList[i].revents & POLLIN)


class ConnectClients
{
    private:
        struct addrinfo _clientAddress;
        socklen_t  _clientAddressLen;
        std::vector<pollfd> _fdList;
        int _currClientSocket;
        char _clientData[MAX_REQUESTSIZE];
        std::vector<uint8_t> _byteVector;
        std::vector<int> _serverSockets;

    public:
        ConnectClients(const std::vector<int>&);
        ~ConnectClients();

        void clientConnected();
        void initFdList();
        void connectClients();

        void initNewConnection(int serverSocket);

        void initClientInfo(int, const std::vector<uint8_t>&, int);
        int receiveData(int);

        void closeConnection(int*);

        std::map<int, clientInfo> _clientInfo;

        int socketMatch(int);
};


void TESTWEBSITE(int);
#endif
