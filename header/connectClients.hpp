#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"
#include "configParser.hpp"


#define MAX_USERS 100
#define DATA_TO_READ (_fdPortList._fds[i].revents & POLLIN)
#define INCOMING_DATA (_fdPortList._fds[_x].revents & (POLLIN | POLLOUT))
#define CLIENTS (static_cast<int>(_fdPortList._fds.size()))
#define CURRENT_FD (_fdPortList._fds[_x].fd)
#define CURRENT_EVENT (_fdPortList._fds[_x].events)

#define DATA_TO_SEND (_fdPortList._fds[_x].revents & POLLOUT)

typedef std::map<int, clientInfo>::iterator ClientInfoMapIter;

struct fdList
{
    std::vector<int> _ports;
    std::vector<pollfd> _fds;
    std::vector<int> _sockets;
};

class ConnectClients
{
    private:
        int _x;
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
        void    initNewConnection();
        void    initClientInfo(configParser&);
        int     receiveData(configParser&);
        void    closeConnection();
        bool    newConnection();
        void    handleData(configParser&);
        void    setPollEvent(int);
};

#endif
