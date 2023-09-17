#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"
#include "configParser.hpp"


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

        void    clientConnected(configParser&);
        void    initFdList();
        void    connectClients(configParser&);
        void    initNewConnection(int);
        void    initClientInfo(int, configParser&);
        int     receiveData(int);
        void    closeConnection(int*);
        bool    newConnection(int);
        void    handleData(int, configParser&);
};

#endif
