#include "../header/client.hpp"

Client::Client(int serverSocket): clientAddress(),
clientAddressLen(sizeof(clientAddress)), clientSocket(), fdList()
{
    initFdList(serverSocket);
    loop();
    initClientSocket(serverSocket);
}


Client::~Client()
{
    printf("Closing client connection...\n");
    close(clientSocket);
}


void Client::initFdList(int serverSocket)
{
    for (int i = 0; i < MAX_USERS; i++) {
        fdList[i].fd = -1;    // File descriptor
        fdList[i].events = 0; // Set of events to monitor
        fdList[i].revents = 0;// Ready Event Set of Concerned Descriptors
    }

    // Add the listen_socket to the fd_list for monitoring incoming data.
    fdList[0].fd = serverSocket;
    fdList[0].events = POLLIN;
}

void Client::loop()
{
    while (true)
    {
        switch (poll(fdList, MAX_USERS, -1))
        {
            case -1:


        }
    }
}

void Client::initClientSocket(int serverSocket)
{
    std::cout << "server Socket: " << serverSocket << std::endl;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
        exitWithError("Failed to create socket for the client");

    struct addrinfo hints, *serverInfo;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_protocol = 0;          // Any protocol

    if (getaddrinfo("127.0.0.1", "8090", &hints, &serverInfo) != 0)
        exitWithError("Error getting address information");

    // Try to connect to the first available address from the serverInfo list
    for (struct addrinfo *p = serverInfo; p != nullptr; p = p->ai_next)
    {
        std::cout << YEL " . . . Connecting client" RESET<< std::endl;
        if (connect(clientSocket, p->ai_addr, p->ai_addrlen) == 0)
        {

            std::cout << YEL " . . . Client connected" RESET<< std::endl;
            break; // Connection successful
        }
    }

    // Close the socket when done
    close(clientSocket);

}



//
//void Client::initClientSocket(int serverSocket)
//{
//
//    std::cout << YEL " . . . Waiting for connection" RESET << std::endl;
//
//    // accept() makes program sleep/stop until it finds connection
//    // on success, accept() creates new socket that can receive and send data
//    //	(old socket waits for new connections)
//    // also: accept() fills data about clientAddress
//    clientSocket = accept(serverSocket, &clientAddress, &clientAddressLen);
//    if (clientSocket < 0)
//        exitWithError("accept() failed");
//
//
//
//    printf(YEL" . . . Configuring remote address\n"RESET);
//    struct addrinfo hints;
//    memset(&hints, 0, sizeof(hints));
//    hints.ai_socktype = SOCK_STREAM;
//    struct addrinfo *peer_address;
//    if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address))
//    {
//        exitWithError("getaddrinfo() for client failed");
//    }
//    std::cout << serverSocket << std::endl;
//
//
//    printf("Remote address is: ");
//    char address_buffer[100];
//    char service_buffer[100];
//    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
//                address_buffer, sizeof(address_buffer),
//                service_buffer, sizeof(service_buffer),
//                NI_NUMERICHOST);
//    printf("%s %s\n", address_buffer, service_buffer);
//
//    printf(YEL" . . . Creating client socket\n"RESET);
//    clientSocket = socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);
//    if (clientSocket < 0)
//        exitWithError("client socket() failed");
//
//    // connecting
//    printf(YEL" . . . Connecting\n"RESET);
//    if (connect(clientSocket, peer_address->ai_addr, peer_address->ai_addrlen))
//        exitWithError("Failed to connect client");
//    freeaddrinfo(peer_address);
//
////
////    std::cout << YEL " . . . Waiting for connection" RESET << std::endl;
////    char address_buffer[100];
////
////    // writes hostname output to clientAddress
////    // writes service name to address_buffer
////    // NI_NUMERICHOST let us see hostname as IP address
////    getnameinfo((struct sockaddr*)&clientAddress,
////                clientAddressLen, address_buffer, sizeof(address_buffer), nullptr, 0, NI_NUMERICHOST);
////
////    std::cout << YEL " . . . Reading Request" RESET << std::endl;
////    char request[1024];
////    int bytes_received = recv(clientSocket, request, 1024, 0);
////    if (bytes_received <= 0)
////        exitWithError("unable to read client request!");
////
//////    std::cout << "Received " << bytes_received << " bytes" << std::endl;
////// with recv we get HTTP Request Line  (HTTP method (GET, POST, ..), requested URL and HTTP version)
//////    std::cout << "Received data:\n";
//////    printf(GRN"%s\n", request);
////
////
////    std::cout << YEL " . . . Sending Response" RESET << std::endl;
////
////    TESTWEBSITE(clientSocket);
//
//}

void TESTWEBSITE(int clientSocket)
{

    std::string responsePre = readFile("/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html");

    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nLocal time is: ";
    int bytes_sent = send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
    std::cout << "Sent " << bytes_sent << " of " << strlen(response.c_str()) << " bytes" << std::endl;
//	std::string response2 = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    send(clientSocket, responsePre.c_str(), strlen(responsePre.c_str()), 0);

}