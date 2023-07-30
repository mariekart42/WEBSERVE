#ifndef SETSERVER_HPP
#define SETSERVER_HPP

#include "main.hpp"
#include "client.hpp"

#include <netdb.h>		// addrinfo struct

//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <stdlib.h>
//#include <string>

#define IP "127.0.0.1"
#define PORT 8090
#define PORT_STR "8090"

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
//    setServer(std::string &, int);
    ~setServer();

};

#endif
