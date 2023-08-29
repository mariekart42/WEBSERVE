#include "../header/setServer.hpp"

SetServer::SetServer()
{

}

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
    socketAddress.ai_family = PF_UNSPEC;        // communicate over IPv4 // !CHANGED
    socketAddress.ai_socktype = SOCK_STREAM;  // TCP socket
    socketAddress.ai_flags = AI_PASSIVE;      // any available network interface

    // getaddrinfo() generates address that's suitable for bind()
    getaddrinfo(0, std::to_string(port).c_str(), &socketAddress, &bindAddress);// worked
//std::string hostname = "newName";
//    int error = getaddrinfo("www.mama.net", "http", &socketAddress, &bindAddress);
//    if (error != 0)
//        exitWithError("unable to getaddrinfo");

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

//    ConnectClients obj(serverSockets);
//    obj.connectClients();
    return serverSockets;
}


