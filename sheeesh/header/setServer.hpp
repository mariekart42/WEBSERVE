#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "main.hpp"
#include "client.hpp"

#include <netdb.h>		// addrinfo struct

//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <stdlib.h>
//#include <string>


//#include <netinet/in.h>

class setServer
{
private:
    std::string myIpAddress;
    int myPort;
    int mySocket;
    struct addrinfo mySocketAddress;
    struct addrinfo *bindAddress;

    void initServerSocket();
    void bindSocket();
    void startListen();


public:
    setServer();
    setServer(std::string &, int);
    ~setServer();

};

#endif
