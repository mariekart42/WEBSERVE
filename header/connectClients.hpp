#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"
#include "marieConfigParser.hpp"


#include <netdb.h>		// addrinfo struct
#include <poll.h>       // pollfd struct




#define MAX_USERS 10
#define DATA_TO_READ (_fdList[i].revents & POLLIN)
//#define CURRENT_FD (_fdList[i].fd)


class ConnectClients
{
    private:
        struct addrinfo _clientAddress;
        socklen_t  _clientAddressLen;
        std::vector<pollfd> _fdList;
        char _clientData[MAX_REQUESTSIZE];
        std::vector<uint8_t> _byteVector;
        std::vector<int> _serverSockets;
        std::map<int, clientInfo> _clientInfo;

    public:
        ConnectClients(const std::vector<int>&);
        ~ConnectClients();


        void clientConnected();
        void initFdList();
        void connectClients();

        void initNewConnection(int);

        void initClientInfo(int);
        int receiveData(int);

        void closeConnection(int*);

        bool newConnection(int);
        void handleData(int);
};

#endif
