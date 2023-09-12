#include "../header/connectClients.hpp"

ConnectClients::ConnectClients(const std::vector<int>& serverSockets):
    _clientAddress(), _clientAddressLen(sizeof(_clientAddress)),
    _fdList(), _clientData(), _byteVector(),
    _serverSockets(serverSockets), _clientInfo()
{}


ConnectClients::~ConnectClients()
{}


void ConnectClients::initFdList()
{
    for (int x = 0; x < _serverSockets.size(); x++)
    {
        for (int i = 0; i < MAX_USERS; i++) {
            pollfd newSocket = {};
            newSocket.fd = -1;
            newSocket.events = 0;
            newSocket.revents = 0;
            _fdList.push_back(newSocket);

        }
        for (int k = 0 + x; k < MAX_USERS; k++) {
            if (_fdList[k].fd == -1) {
                _fdList[k].fd = _serverSockets.at(x);
                _fdList[k].events = POLLIN;     // Concern about Read-Only Events
                break;
            }
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



void ConnectClients::initClientInfo(int _clientSocket)
{
    std::vector<uint8_t> input = _byteVector;

    std::map<int, clientInfo>::iterator rm = _clientInfo.find(_clientSocket);
    if (rm != _clientInfo.end() && !rm->second._isMultiPart)
        _clientInfo.erase(rm);

    std::map<int, clientInfo>::iterator it = _clientInfo.find(_clientSocket);
    if (it == _clientInfo.end())   // IF NOT INITTED YET
    {
        clientInfo initNewInfo;
        Request request(input);
        MarieConfigParser config;

        int currentPort = request.getPort();
        // httpMethod getHTTP(myHHTTP, Port, Url);
        initNewInfo._myHTTPMethod = request.getHTTPMethod();
        initNewInfo._clientSocket = _clientSocket;
        initNewInfo._url = config.getUrl(currentPort,request.getUrlString());
        initNewInfo._fileContentType = request.getFileContentType(initNewInfo._url);
        initNewInfo._contentType = request.getContentType();
        initNewInfo._isMultiPart = false;
        initNewInfo._configInfo._rootFolder = config.getRootFolder(currentPort);
        initNewInfo._configInfo._autoIndex = config.getAutoIndex(currentPort);
        initNewInfo._configInfo._indexFile = config.getIndexFile(currentPort);
        if (initNewInfo._myHTTPMethod == M_POST)
        {
            initNewInfo._postInfo._input = input;
            initNewInfo._configInfo._postAllowed = config.getPostAllowed(currentPort);// TODO: implement correctly from Valentin
            initNewInfo._postInfo._filename = request.getFileName(initNewInfo._contentType, initNewInfo._postInfo._filename, UPLOAD_FOLDER);
            initNewInfo._postInfo._outfile = new std::ofstream (UPLOAD_FOLDER+initNewInfo._postInfo._filename, std::ofstream::out | std::ofstream::app  | std::ofstream::binary);
            if (initNewInfo._contentType == "multipart/form-data")
            {
                initNewInfo._isMultiPart = true;
                initNewInfo._postInfo._boundary = request.getBoundary();
            }
            else
                initNewInfo._postInfo._filename = "tmpFile.txt";
        }
        if (initNewInfo._myHTTPMethod == M_DELETE)
        {
            if (!Request::checkPathInFolder(initNewInfo._url, UPLOAD_FOLDER))
                initNewInfo._url = FAILURE;
            initNewInfo._configInfo._deleteAllowed = config.getDeleteAllowed(currentPort);
        }
        else
            initNewInfo._configInfo._getAllowed = config.getGetAllowed(currentPort);
        _clientInfo[_clientSocket] = initNewInfo;
    }
    else if (it->second._isMultiPart)   // only for multipart!!
    {
        Request request(input);
        std::string oldFilename = it->second._postInfo._filename;
        it->second._postInfo._input = input;
        it->second._postInfo._filename = request.getFileName(it->second._contentType, it->second._postInfo._filename, UPLOAD_FOLDER);
        if (oldFilename.compare(0, 13, "not_found_yet") == 0 && it->second._postInfo._filename.compare(0, 13, "not_found_yet") != 0)
            rename((UPLOAD_FOLDER+oldFilename).c_str(), (UPLOAD_FOLDER+it->second._postInfo._filename).c_str());
        it->second._statusCode = request.getStatusCode();
    }
}

int ConnectClients::receiveData(int i)
{
    memset(_clientData, 0, MAX_REQUESTSIZE);
    ssize_t bytesRead = recv(_fdList[i].fd, _clientData, sizeof(_clientData), O_NONBLOCK);
    if (bytesRead < 0)
        return -1;
    if (bytesRead == 0)
        return 0;

    #ifdef DEBUG
        std::cout << "Client Data:\n"<<_clientData<<std::endl;
    #endif

    // converting client data to vector
    size_t charArraySize = MAX_REQUESTSIZE;
    _byteVector.clear();
    _byteVector.reserve(charArraySize); // Reserve space to avoid reallocations
    for (size_t k = 0; k < charArraySize; ++k)
        _byteVector.push_back(static_cast<uint8_t>(_clientData[k]));
    return 69;
}


void ConnectClients::closeConnection(int *i)
{
    Logging::log("Done receiving Data", 200);
    close(_fdList[*i].fd);
    _fdList.erase(_fdList.begin() + *i);
    --*i;
}

bool ConnectClients::newConnection(int fdListFd)
{
    for (int i = 0; i < _serverSockets.size(); i++)
    {
        if (_serverSockets.at(i) == fdListFd)
            return (true);
    }
    return false;
}

void ConnectClients::handleData(int fd)
{
    initClientInfo(fd);

    std::map<int, clientInfo>::iterator it;
    it = _clientInfo.find(fd);
    Response response(fd, it->second);

    switch (it->second._myHTTPMethod)
    {
        case M_GET:
            response.sendRequestedFile();
            break;
        case M_POST:
            it->second._isMultiPart = response.uploadFile(it->second._contentType,
                                                          it->second._postInfo._boundary,
                                                          it->second._postInfo._outfile);
            break;
        case M_DELETE:
            response.deleteFile();
            break;
        default:
            Logging::log("cant detect HTTPMethod", 500);
            break;
    }
}

void ConnectClients::clientConnected()
{
    for (int i = 0; i < _fdList.size(); ++i)
    {
        int fd = _fdList[i].fd;
        if (DATA_TO_READ)   //_fdList[i].revents & POLLIN
        {
            if (newConnection(fd))
                initNewConnection(fd);
            else
            {
                switch (receiveData(i))
                {
                    case 69:
                        handleData(fd);
                        break;
                    case 0:
                        closeConnection(&i);
                        break;
                    default:
                        Logging::log("Unable to read Data from connected Client", 500);
                        exit(69);
                }
            }
        }
    }
}



void ConnectClients::connectClients()
{
    initFdList();

    std::cout << GRN " . . Server ready to connect Clients" RESET << std::endl;
    while (69)
    {
        // poll checks _fdList for read & write events at the same time
        // poll() ≈ select()
        switch (poll(&_fdList[0], _fdList.size(), -1))
        {
            case -1:
                exitWithError("Poll function returned Error [EXIT]");
                break;
            case 0:
                Logging::log("waiting for client to connect", 200);
                break;
            default:
                clientConnected();
                break;
        }
    }
}