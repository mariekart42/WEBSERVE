#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"
#include "marieConfigParser.hpp"


#define MAX_USERS 10
#define DATA_TO_READ (_fdPortList._fds[i].revents & POLLIN)


struct fdList
{
    std::vector<int> _ports;
    std::vector<pollfd> _fds;
    std::vector<int> _sockets;
};

class ConnectClients
{
    private:
        fdList _fdPortList;
        socklen_t _clientAddressLen;
        struct addrinfo _clientAddress;
        std::vector<uint8_t> _byteVector;
        std::map<int, clientInfo> _clientInfo;

    public:
        ConnectClients(const fdList&);
        ~ConnectClients();

        void    clientConnected();
        void    initFdList();
        void    connectClients(int);
        void    initNewConnection(int);
        void    initClientInfo(int);
        int     receiveData(int);
        void    closeConnection(int*);
        bool    newConnection(int);
        void    handleData(int);
};

#endif
