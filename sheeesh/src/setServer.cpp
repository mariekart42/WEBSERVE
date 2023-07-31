#include "../header/setServer.hpp"

SetServer::SetServer(int port):
    _port(port), _serverSocket(), _socketAddress(), _bindAddress()
{}

SetServer::~SetServer()
{
    close(_serverSocket);
    exit(0);
}


void SetServer::initServerSocket()
{
    memset(&_socketAddress, 0, sizeof(_socketAddress));
    _socketAddress.ai_family = AF_INET;        // communicate over IPv4
    _socketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    _socketAddress.ai_flags = AI_PASSIVE;      // any available network interface

    // getaddrinfo() generates address that's suitable for bind()
    getaddrinfo(0, std::to_string(_port).c_str(), &_socketAddress, &_bindAddress);

    std::cout << YEL " . . . Creating Socket" RESET << std::endl;

    _serverSocket = socket(_bindAddress->ai_family, _bindAddress->ai_socktype, _bindAddress->ai_protocol);	 // domain, type, protocol
    if (_serverSocket < 0)
        exitWithError("Cannot create socket");
}

void SetServer::bindSocket()
{
    std::cout << YEL " . . . Binding socket to local address" RESET << std::endl;

    // binds specify address and port to "mySocket"
    if (bind(_serverSocket, _bindAddress->ai_addr, _bindAddress->ai_addrlen) < 0)
        exitWithError("Cannot connect socket to address, Port already in use");
    freeaddrinfo(_bindAddress);
}

void SetServer::startListen() const
{
    std::cout << YEL " . . . Listening" RESET << std::endl;
    // listen function puts created socket into a passive listening state
    // -> allows server to accept() incoming client connections
    //	  (second arg: how many client connections allowed to queue up)
    if (listen(_serverSocket, 10) < 0)
        exitWithError("Socket listen failed");
}


void SetServer::setUpServer()
{
    initServerSocket();
    bindSocket();
    startListen();
    ConnectClients obj;
    obj.connectClients(_serverSocket);
}


