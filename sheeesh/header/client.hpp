#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "setServer.hpp"
#include "main.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Client
{
    private:
        struct sockaddr_storage clientAddress;
        socklen_t clientAddressLen;
        int clientSocket;

        void initClientSocket(int);

    public:
        Client(int);
        ~Client();
};


#endif
