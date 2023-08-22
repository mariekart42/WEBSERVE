#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "connectClients.hpp"

class SetServer
{
    private:
        int _port;
        int _serverSocket;
        struct addrinfo _socketAddress;
        struct addrinfo *_bindAddress;

        void initServerSocket();
        void bindSocket();
        void startListen() const;

    public:
        SetServer(int);
        ~SetServer();

        void setUpServer(int* );
};

#endif
