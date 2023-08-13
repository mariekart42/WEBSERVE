#include "../header/connectClients.hpp"


ConnectClients::ConnectClients():
    _clientAddress(),
    _clientAddressLen(sizeof(_clientAddress)), _fdList()
{}


ConnectClients::~ConnectClients()
{
    printf("Closing client connection...\n");
}


void ConnectClients::initFdList(int serverSocket)
{
    pollfd initialServerSocket = {};
    initialServerSocket.fd = serverSocket;
    initialServerSocket.events = POLLIN;
    initialServerSocket.revents = 0;
    _fdList.push_back(initialServerSocket);
}


void ConnectClients::initNewConnection(int serverSocket)
{
    clientInfo tmp;

    std::cout << YEL " . . . Accepting Connection from Client" RESET << std::endl;
    tmp._clientSocket = accept(serverSocket, (struct sockaddr *) &_clientAddress, &_clientAddressLen);
    if (tmp._clientSocket < 0)
        exitWithError("Failed to init client Socket [EXIT]");
    tmp._bytesLeft = 0;
    tmp._statusCode = 200;
    _clientInfo[tmp._clientSocket] = tmp;

    pollfd newClientSocket = {};
    newClientSocket.fd = tmp._clientSocket;
    newClientSocket.events = POLLIN;
    newClientSocket.revents = 0;
    _fdList.push_back(newClientSocket);
}


//httpMethod ConnectClients::getHTTPMethod() const
//{
//    if (_tmp.compare(0, 3, "GET") == 0)
//        return M_GET;
//    else if (_tmp.compare(0, 4, "POST") == 0)
//        return M_POST;
//    else if (_tmp.compare(0, 6, "DELETE") == 0)
//        return M_DELETE;
//    else
//        return M_error;
//}

void ConnectClients::initClientInfo(int _clientSocket, const std::vector<uint8_t>& input)
{
    std::map<int, clientInfo>::iterator it = _clientInfo.find(_clientSocket);
    if (it == _clientInfo.end())   // IF NOT INITTED YET
    {
        clientInfo initNewInfo;
        Request request(input);
        initNewInfo._input = input;
        initNewInfo._clientSocket = _clientSocket;
        initNewInfo._myHTTPMethod = request.getHTTPMethod();
        initNewInfo._url = request.getURL();
        initNewInfo._fileContentType = request.getFileContentType(initNewInfo._url);
        initNewInfo._contentType = request.getContentType();
        initNewInfo._bytesLeft = request.getBytesLeft(initNewInfo._contentType);
        initNewInfo._filename = request.getFileName(initNewInfo._contentType, initNewInfo._filename);
        initNewInfo._statusCode = request.getStatusCode();
        _clientInfo[_clientSocket] = initNewInfo;
    }
    else
    {
        Request request(input);
        it->second._input = input;
        it->second._bytesLeft = request.getBytesLeft(it->second._contentType);
        it->second._filename = request.getFileName(it->second._contentType, it->second._filename);
        it->second._statusCode = request.getStatusCode();
    }
}


void ConnectClients::clientConnected(int serverSocket)
{
    for (size_t i = 0; i < _fdList.size(); ++i)
    {
        if (_fdList[i].revents & POLLIN)
        {
            if (_fdList[i].fd == serverSocket)
            {
                initNewConnection(serverSocket);
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

                    for (i = 0; i < charArraySize; ++i) {
                        byteVector.push_back(static_cast<uint8_t>(clientData[i]));
                    }

                    initClientInfo(_fdList[i].fd, byteVector);
                    std::map<int, clientInfo>::const_iterator it = _clientInfo.find(_fdList[i].fd);
//                    Request request(byteVector);

                    Response response(it->second);
                    response.sendResponse();

                    if (strncmp(clientData, "GET", 3) == 0)
                        close(_fdList[i].fd);
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
    initFdList(serverSocket);

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