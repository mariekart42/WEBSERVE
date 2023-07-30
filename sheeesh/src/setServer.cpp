#include "../header/setServer.hpp"

setServer::setServer() : myPort(), mySocket(), mySocketAddress(), bindAddress()
{
    std::cout << "default constructor" << std::endl;
    initServerSocket();
    bindSocket();
    startListen();
    Client client(mySocket);
}

//setServer::setServer(std::string &ipAddress, int port)
//{
//    initServerSocket();
//    bindSocket();
//    startListen();
//    Client client(mySocket);
//}

setServer::~setServer()
{
    close(mySocket);
//    close(myNewSocket);
    exit(0);
}

void setServer::initServerSocket()
{
    memset(&mySocketAddress, 0, sizeof(mySocketAddress));
    mySocketAddress.ai_family = AF_INET;        // communicate over IPv4
    mySocketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    mySocketAddress.ai_flags = AI_PASSIVE;      // any available network interface

    // getaddrinfo() generates address that's suitable for bind()
    getaddrinfo(0, std::to_string(PORT).c_str(), &mySocketAddress, &bindAddress);

    std::cout << YEL " . . . Creating Socket" RESET << myPort << std::endl;

    mySocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 // domain, type, protocol
    if (mySocket < 0)
        exitWithError("Cannot create socket");
}

void setServer::bindSocket()
{
    std::cout << YEL " . . . Binding socket to local address" RESET << std::endl;

    // binds specify address and port to "mySocket"
    if (bind(mySocket, bindAddress->ai_addr, bindAddress->ai_addrlen) < 0)
        exitWithError("Cannot connect socket to address, Port already in use");
    freeaddrinfo(bindAddress);
}

void setServer::startListen()
{
    std::cout << YEL " . . . Listening" RESET << std::endl;
    // listen function puts created socket into a passive listening state
    // -> allows server to accept() incoming client connections
    //	  (second arg: how many client connections allowed to queue up)
    if (listen(mySocket, 10) < 0)
        exitWithError("Socket listen failed");



}


