#include "../header/connectClients.hpp"

ConnectClients::ConnectClients():
    _clientAddress(),
    _clientAddressLen(sizeof(_clientAddress)), _fdList(), _currClientSocket(), _clientData()
{}


ConnectClients::~ConnectClients()
{
    printf("Closing client connection...\n");
}


void ConnectClients::initFdList(int serverSocket)
{
    for (int i = 0; i < MAX_USERS; i++ )
    {
        pollfd newSocket = {};
        newSocket.fd = -1;
        newSocket.events = 0;
        newSocket.revents = 0;
        _fdList.push_back(newSocket);

    }
    for (int k = 0; k < MAX_USERS; k++ )
    {
        if (_fdList[k].fd == -1)
        {
            _fdList[k].fd = serverSocket;
            _fdList[k].events = POLLIN;     // Concern about Read-Only Events
            break;
        }
    }
}


void ConnectClients::initNewConnection(int serverSocket)
{
    // Server socket has activity, accept new connection
    int newClientSocket = accept(serverSocket, (struct sockaddr *) &_clientAddress, &_clientAddressLen);
    if (newClientSocket != -1)
    {
        // Find an available slot or expand the vector
        bool foundSlot = false;
        for (int j = 0; j < _fdList.size(); j++)
        {
            if (_fdList[j].fd == -1)
            {
                _fdList[j].fd = newClientSocket;
                _fdList[j].events = POLLIN;
                foundSlot = true;
                break;
            }
        }
        if (!foundSlot)
        {
            pollfd newClient = {newClientSocket, POLLIN, 0};
            _fdList.push_back(newClient);
        }
    }
}



void ConnectClients::initClientInfo(int _clientSocket, const std::vector<uint8_t>& input, int bytesRead)
{
    std::map<int, clientInfo>::iterator it1 = _clientInfo.find(_clientSocket);
    if (it1 != _clientInfo.end() && it1->second._postInfo._isMultiPart == false)
    {
        _clientInfo.erase(it1);
    }
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
        initNewInfo._postInfo._contentType = request.getContentType();
        if (initNewInfo._postInfo._contentType == "multipart/form-data")
        {
            initNewInfo._postInfo._isMultiPart = true;
            initNewInfo._postInfo._boundary = request.getBoundary();
            initNewInfo._postInfo._bytesLeft = request.getBytesLeft(initNewInfo._postInfo._contentType, initNewInfo._postInfo._boundary);
            initNewInfo._postInfo._filename = request.getFileName(initNewInfo._postInfo._contentType, initNewInfo._postInfo._filename);
            initNewInfo._postInfo._outfile = new std::ofstream (UPLOAD_FOLDER+initNewInfo._postInfo._filename, std::ofstream::out | std::ofstream::app  | std::ofstream::binary);
        }
        else
        {
            initNewInfo._postInfo._isMultiPart = false;
            initNewInfo._postInfo._bytesLeft = 0;
            initNewInfo._postInfo._filename = "LOL_NO_CLUE.txt";
        }
        initNewInfo._statusCode = 200;
        _clientInfo[_clientSocket] = initNewInfo;
    }
    else if (it->second._postInfo._isMultiPart == true)   // only for multipart!!
    {
        Request request(input);
        std::string oldFilename = it->second._postInfo._filename;
        it->second._input = input;
        it->second._postInfo._bytesLeft -= bytesRead;
        it->second._postInfo._filename = request.getFileName(it->second._postInfo._contentType, it->second._postInfo._filename);
        if (oldFilename.compare(0, 13, "not_found_yet") == 0 && it->second._postInfo._filename.compare(0, 13, "not_found_yet") != 0)
            rename((UPLOAD_FOLDER+oldFilename).c_str(), (UPLOAD_FOLDER+it->second._postInfo._filename).c_str());
        it->second._statusCode = request.getStatusCode();
    }
}

int ConnectClients::receiveData(int i)
{
    
    memset(_clientData, 0, MAX_REQUESTSIZE);
    ssize_t bytesRead = recv(_fdList[i].fd, _clientData, sizeof(_clientData), O_NONBLOCK);

    // converting client data to vector
    size_t charArraySize = MAX_REQUESTSIZE;
    _byteVector.clear();
    _byteVector.reserve(charArraySize); // Reserve space to avoid reallocations
    for (size_t k = 0; k < charArraySize; ++k)
        _byteVector.push_back(static_cast<uint8_t>(_clientData[k]));

    return bytesRead;
}


void ConnectClients::closeConnection(int *i)
{
    close(_fdList[*i].fd);
    _fdList.erase(_fdList.begin() + *i);
    --*i;
}


void ConnectClients::clientConnected(int serverSocket)
{
    for (int i = 0; i < _fdList.size(); ++i)
    {
        if (DATA_TO_READ)   //_fdList[i].revents & POLLIN
        {
            if (_fdList[i].fd == serverSocket)
                initNewConnection(serverSocket);
            else
            {
                int bytesRead = receiveData(i);
                if (bytesRead > 0)
                {
                    initClientInfo(_fdList[i].fd, _byteVector, bytesRead);

                    std::map<int, clientInfo>::iterator it = _clientInfo.find(_fdList[i].fd);
                    Response response(_byteVector, _fdList[i].fd, it->second._url);

                    switch (it->second._myHTTPMethod)
                    {
                        case M_GET:
                            response.sendRequestedFile();
                            break;
                        case M_POST:
                            it->second._postInfo._isMultiPart = response.uploadFile(it->second._postInfo._contentType, it->second._postInfo._boundary, it->second._postInfo._outfile);
                            break;
                        case M_DELETE:
                            response.deleteFile();
                            break;
                        default:
                            std::cout<<RED"unexpected Error: cant detect HTTPMethod"RESET<<std::endl;
                            break;
                    }

                    if (!it->second._postInfo._isMultiPart)
                        closeConnection(&i);

                }
                else if (bytesRead == 0)
                {
                    // DO I HAVE TO RESPONSE ANYTHING?
                    std::cout << "Connection closed by client" << std::endl;
                    closeConnection(&i);
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
            case 0:
                std::cout<<YEL". . . waiting"RESET<<std::endl;
                break;
            default:
                clientConnected(serverSocket);
                break;
        }
    }
    exitWithError("out of poll lop!!!");
}