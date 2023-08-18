#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"

#include <netdb.h>		// addrinfo struct
#include <poll.h>       // pollfd struct


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

    public:
        ConnectClients();
        ~ConnectClients();

        void clientConnected(int);
        void initFdList(int);
        void connectClients(int);

        void initNewConnection(int serverSocket);

        void initClientInfo(int, const std::vector<uint8_t>&, int);
        int receiveData(int);

        std::map<int, clientInfo> _clientInfo;
};


void TESTWEBSITE(int);
#endif
