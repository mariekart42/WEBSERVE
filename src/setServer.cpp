#include "../header/setServer.hpp"

SetServer::SetServer(int port):
    _port(port)//, _serverSocket(), _socketAddress(), _bindAddress()
{}

SetServer::~SetServer()
{
//    close(_serverSocket);
}


int SetServer::getNewServerSocket(int port)
{
    int newServerSocket;
    struct addrinfo socketAddress;
    struct addrinfo *bindAddress;

    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.ai_family = AF_INET;        // communicate over IPv4
    socketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    socketAddress.ai_flags = AI_PASSIVE;      // any available network interface

    // getaddrinfo() generates address that's suitable for bind()
    getaddrinfo(0, std::to_string(port).c_str(), &socketAddress, &bindAddress);

    std::cout << YEL " . . . Creating Socket"RESET << std::endl;

    newServerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 // domain, type, protocol
    if (newServerSocket < 0)
        exitWithError("Cannot create socket");
    std::cout << YEL " . . . Binding socket to local address\nSocket: "<< newServerSocket << "" RESET << std::endl;

    // binds specify address and port to "mySocket"
    if (bind(newServerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen) < 0)
        exitWithError("Cannot connect socket to address, Port already in use");
    freeaddrinfo(bindAddress);

    return (newServerSocket);
}


void SetServer::initServerSocket(int serverSocket)
{
    std::cout << YEL " . . . Listening" RESET << std::endl;
    // listen function puts created socket into a passive listening state
    // -> allows server to accept() incoming client connections
    //	  (second arg: how many client connections allowed to queue up)
    if (listen(serverSocket, 10) < 0)
        exitWithError("Socket listen failed");
}


//void SetServer::startListen() const
//{
//}


void SetServer::setUpServer()
{

//    Config config;
//
//    if (!config.getStartServer())
//    {
//        // error, dont start Server
//    }
//
//    int amountPorts = config.getPortVector().size();
    std::vector<int> serverSockets;
//    std::vector<
    int amountPorts = 2;
    int port;
    for (int i = 0; i < amountPorts; i++)
    {
//        int currentPort = config.getPortAt(i);
        if (i == 0)
            port = 2020;
        if (i == 1)
            port = 6060;
        int newServerSocket;
        newServerSocket = getNewServerSocket(port);
        initServerSocket(newServerSocket);
//        startListen();
        // ADD NEW SERVER SOCKET TO THE END OF SERVER SOCKET VECTOR
        serverSockets.push_back(newServerSocket);
    }

    ConnectClients obj(serverSockets);
    obj.connectClients();
}


