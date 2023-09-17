#include "../header/setServer.hpp"

SetServer::SetServer(){}
SetServer::~SetServer() {}

int SetServer::setNewSocketFd(int port)
{
    int newSocketFd;
    struct addrinfo socketAddress;
    struct addrinfo *bindAddress;

    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.ai_family = AF_UNSPEC;        // communicate over IPv4 // !CHANGED
    socketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    socketAddress.ai_flags = AI_PASSIVE;      // any available network interface

//    getaddrinfo(0, std::to_string(port).c_str(), &socketAddress, &bindAddress);
    getaddrinfo(0, myItoS(port).c_str(), &socketAddress, &bindAddress);

    newSocketFd = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 // domain, type, protocol

    if (newSocketFd < 0)
        exitWithError("Socket function returned error [EXIT]");

    if (bind(newSocketFd, bindAddress->ai_addr, bindAddress->ai_addrlen) < 0)
        exitWithError("Failed to connect, Port already in use [EXIT]");
    freeaddrinfo(bindAddress);

    if (listen(newSocketFd, _backlog) < 0)
        exitWithError("Listen function failed [EXIT]");
    return (newSocketFd);
}

void SetServer::setServer()
{

//    Config config;
//    if (!config.validConfig(ac, av))
//    {
//        // error, dont start Server
//    }

    MarieConfigParser config;
    fdList initList;

//    _backlog = config.getBacklog();
    _backlog = 10;



    std::vector<int> ports = config.getPortVector();

    for (int i = 0; i < ports.size(); i++)
    {
        int newSocketFd = setNewSocketFd(ports.at(i));
        initList._ports.push_back(ports.at(i));
        initList._sockets.push_back(newSocketFd);
    }

//    int timeout = config.getTimeout();
    int timeout = -1;

    ConnectClients connect(initList);
    connect.connectClients(timeout);
}
