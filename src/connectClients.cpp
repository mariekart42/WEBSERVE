#include "../header/connectClients.hpp"


ConnectClients::ConnectClients():
    _clientSocket(), _clientAddress(),
    _clientAddressLen(sizeof(_clientAddress)), _fdList()
{}


ConnectClients::~ConnectClients()
{
    printf("Closing client connection...\n");
}


//void ConnectClients::initFdList(int serverSocket)
//{
//    int i = 0;
//
//    for (; i < MAX_USERS; i++) {
//        _fdList[i].fd = -1;         // File descriptor
//        _fdList[i].events = 0;      // Set of events to monitor
//        _fdList[i].revents = 0;     // Ready Event Set of Concerned Descriptors
//    }
//    i = 0;
//    for (; i < MAX_USERS; i++) {
//        if (_fdList[i].fd == -1)
//        {
//            _fdList[i].fd = serverSocket;
//            _fdList[i].events = POLLIN;     // Concern about Read-Only Events
//            break;
//        }
//    }
//}



void ConnectClients::clientConnected(int serverSocket)
{
    for (size_t i = 0; i < _fdList.size(); ++i)
    {
        if (_fdList[i].revents & POLLIN)
        {
            if (_fdList[i].fd == serverSocket)
            {
                std::cout << YEL " . . . Accepting Connection from Client" RESET << std::endl;
                _clientSocket = accept(serverSocket, (struct sockaddr *) &_clientAddress, &_clientAddressLen);
                if (_clientSocket < 0)
                    exitWithError("Failed to init client Socket [EXIT]");
                pollfd newClientSocket;
                newClientSocket.fd = _clientSocket;
                newClientSocket.events = POLLIN;
                newClientSocket.revents = 0;
                _fdList.push_back(newClientSocket);
            }
            else
            {

                char clientData[MAX_REQUESTSIZE];
                memset(clientData, 0, MAX_REQUESTSIZE);
                ssize_t bytesRead = recv(_fdList[i].fd, clientData, sizeof(clientData), O_NONBLOCK);


                std::cout << "Received Data [" << bytesRead << "] \n"<<clientData<<std::endl;

                if (bytesRead > 0)
                {

                    size_t charArraySize = sizeof(clientData) / sizeof(char);

                    std::vector<uint8_t> byteVector;
                    byteVector.reserve(charArraySize); // Reserve space to avoid reallocations

                    for (size_t i = 0; i < charArraySize; ++i) {
                        byteVector.push_back(static_cast<uint8_t>(clientData[i]));
                    }


                    Request request(byteVector);
                    Response response(request, _clientSocket);
                    response.sendResponse();

                    if (strncmp(clientData, "GET", 3) == 0)
                        close(_clientSocket);
                }
                else if (bytesRead == 0)
                {
                    // DO I HAVE TO RESPONSE ANYTHING?
                    std::cout << "Connection closed by client" << std::endl;
                    close(_fdList[i].fd);
                    _fdList.erase(_fdList.begin() + i);
                    --i; // Compensate for erasing
                }
                else
                    exitWithError("unexpected error while reading data from client with read()");
            }
        }
    }
}


void ConnectClients::connectClients(int serverSocket)
{
//    initFdList(serverSocket);
    pollfd initialServerSocket;
    initialServerSocket.fd = serverSocket;
    initialServerSocket.events = POLLIN;
    initialServerSocket.revents = 0;
    _fdList.push_back(initialServerSocket);
    while (69)
    {
        switch (poll(&_fdList[0], _fdList.size(), -1))
        {
            case -1:
                exitWithError("Failed to poll [EXIT]");
                break;
            default:
                clientConnected(serverSocket);
                break;
        }
    }
}