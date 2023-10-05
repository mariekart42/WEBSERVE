#ifndef CONNECTCLIENTS_HPP
#define CONNECTCLIENTS_HPP

#include "Response.hpp"
#include "configParser.hpp"


#define INCOMING_DATA (_fdPortList._fds[_x].revents & (POLLIN | POLLOUT))
#define SOCKET_ERROR (_fdPortList._fds[_x].revents & (POLLERR | POLLHUP | POLLNVAL))
#define CURRENT_FD (_fdPortList._fds[_x].fd)

struct fdList
{
    std::vector<int>    _ports;
    std::vector<pollfd> _fds;
    std::vector<int>    _sockets;
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

        int     receiveData();
        bool    newConnection();
        void    initFdList();
		void    shutDownServer();
        void    closeConnection();
        void    setPollEvent(int);
        void    initNewConnection();
        void    handleData(configParser&);
		static void    setFileData(clientInfo data);
        void    clientConnected(configParser&);
        void    connectClients(configParser&);
        void    initClientInfo(configParser&);
};

#endif
