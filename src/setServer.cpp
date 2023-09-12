#include "../header/setServer.hpp"

SetServer::SetServer()
{

}

SetServer::~SetServer()
{
}


int SetServer::getNewServerSocket(int port)
{
    int newServerSocket;
    struct addrinfo socketAddress;
    struct addrinfo *bindAddress;

    memset(&socketAddress, 0, sizeof(socketAddress));
    socketAddress.ai_family = PF_UNSPEC;        // communicate over IPv4 // !CHANGED
    socketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    socketAddress.ai_flags = AI_PASSIVE;      // any available network interface

    getaddrinfo(0, std::to_string(port).c_str(), &socketAddress, &bindAddress);

    #ifdef DEBUG
        std::cout << YEL " . . . Creating Socket"RESET << std::endl;
    #endif

    newServerSocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 // domain, type, protocol
    if (newServerSocket < 0)
        exitWithError("Socket function returned error [EXIT]");

    #ifdef DEBUG
        std::cout << YEL " . . . Binding socket to local address\nSocket: "<< newServerSocket << "" RESET << std::endl;
    #endif

    // binds specify address and port to "mySocket"
    if (bind(newServerSocket, bindAddress->ai_addr, bindAddress->ai_addrlen) < 0)
        exitWithError("Failed to connect, Port already in use [EXIT]");
    freeaddrinfo(bindAddress);

    return (newServerSocket);
}


void SetServer::initServerSocket(int serverSocket)
{
    // listen function puts created socket into a passive listening state
    // -> allows server to accept() incoming client connections
    //	  (second arg: how many client connections allowed to queue up)
    if (listen(serverSocket, 10) < 0)
        exitWithError("Listen function failed [EXIT]");
}


std::vector<int> SetServer::setUpServer()
{

//    Config config;
//    if (!config.getStartServer())
//    {
//        // error, dont start Server
//    }

    MarieConfigParser config;
    std::vector<int> ports = config.getPortVector();


    std::vector<int> serverSockets;

    for (int i = 0; i < ports.size(); i++)
    {
        int newServerSocket = getNewServerSocket(ports.at(i));
        initServerSocket(newServerSocket);
        serverSockets.push_back(newServerSocket);
    }

    return serverSockets;
}
