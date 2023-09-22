#include "../header/connectClients.hpp"

ConnectClients::ConnectClients(const fdList& initList):
_fdPortList(initList), _clientAddressLen(sizeof(_clientAddress)),_clientAddress(),
    _byteVector(),
    _clientInfo()
{}


ConnectClients::~ConnectClients()
{}


void ConnectClients::initFdList()
{
    int portSize = _fdPortList._sockets.size();
    for (int x = 0; x < portSize; x++)
    {
        for (int i = 0; i < MAX_USERS; i++) {
            pollfd newSocket = {};
            newSocket.fd = -1;
            newSocket.events = 0;
            newSocket.revents = 0;
            _fdPortList._fds.push_back(newSocket);

        }
        for (int k = 0 + x; k < MAX_USERS; k++) {
            if (_fdPortList._fds[k].fd == -1) {
                _fdPortList._fds[k].fd = _fdPortList._sockets.at(x);
                _fdPortList._fds[k].events = POLLIN;
                break;
            }
        }
    }
}


void ConnectClients::initNewConnection(int socketFd)
{
    int newClientSocket = accept(socketFd, (struct sockaddr *) &_clientAddress, &_clientAddressLen);

    if (newClientSocket != -1)
    {
        // Find an available slot or expand the vector
        bool foundSlot = false;
        int portSize = _fdPortList._fds.size();
        for (int j = 0; j < portSize; j++)
        {
            if (_fdPortList._fds[j].fd == -1)
            {
                _fdPortList._fds[j].fd = newClientSocket;
                _fdPortList._fds[j].events = POLLIN;
                foundSlot = true;
                break;
            }
        }
        if (!foundSlot)
        {
            pollfd newClient = {newClientSocket, POLLIN, 0};
            _fdPortList._fds.push_back(newClient);
        }

        for (std::vector<int>::size_type i = 0; i < _fdPortList._sockets.size(); ++i)
        {
            if (_fdPortList._sockets[i] == socketFd)
            {
                // The target value is found; store the position
                int pos = static_cast<int>(i);
                _fdPortList._ports.push_back(_fdPortList._ports.at(pos));
                break;
            }
        }
    }
}



void ConnectClients::initClientInfo(int _clientSocket, configParser& config)
{
    std::vector<uint8_t> input = _byteVector;

    std::map<int, clientInfo>::iterator rm = _clientInfo.find(_clientSocket);
    if (rm->second._filePos > 0)
        return;
    if (rm != _clientInfo.end() && !rm->second._isMultiPart)
        _clientInfo.erase(rm);

    std::map<int, clientInfo>::iterator it = _clientInfo.find(_clientSocket);
    if (it == _clientInfo.end())   // IF NOT INITTED YET
    {
        clientInfo initNewInfo;
        Request request(input);

        config.setData(request.getUrlString(), "127.0.0.1", request.getPort()); // TODO Marie

//        int currentPort = request.getPort();
        // httpMethod getHTTP(myHHTTP, Port, Url);
        initNewInfo._myHTTPMethod = request.getHTTPMethod();
        initNewInfo._clientSocket = _clientSocket;
        initNewInfo._url = config.getUrl();
        initNewInfo._fileContentType = request.getFileContentType(initNewInfo._url);
        initNewInfo._contentType = request.getContentType();
        initNewInfo._isMultiPart = false;
        initNewInfo._configInfo._rootFolder = ROOT; // TODO : als macro lassen oder getter?
        initNewInfo._configInfo._autoIndex = config.getAutoIndex();
        initNewInfo._configInfo._indexFile = config.getIndexFile();
        initNewInfo._errorMap = config.getErrorMap();
        initNewInfo._isChunkedFile = false;
        initNewInfo._filePos = 0;
        if (initNewInfo._myHTTPMethod == M_POST)
        {
            initNewInfo._postInfo._input = input;
            initNewInfo._configInfo._postAllowed = config.getPostAllowed();
            initNewInfo._postInfo._filename = request.getFileName(initNewInfo._contentType, initNewInfo._postInfo._filename, UPLOAD_FOLDER);
            initNewInfo._postInfo._outfile = new std::ofstream (UPLOAD_FOLDER+initNewInfo._postInfo._filename, std::ofstream::out | std::ofstream::app | std::ofstream::binary);
            chmod((UPLOAD_FOLDER+initNewInfo._postInfo._filename).c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH); // no execution permission for user
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
            if (!Request::checkPathInFolder(initNewInfo._url, initNewInfo._configInfo._rootFolder))
                initNewInfo._url = FAILURE;
            initNewInfo._configInfo._deleteAllowed = config.getDeleteAllowed();
        }
        else
            initNewInfo._configInfo._getAllowed = config.getGetAllowed();
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
    }
}

int ConnectClients::receiveData(int i, configParser& config)
{
//    configParser config;
    if (_fdPortList._fds[i].revents & POLLOUT)
        return 69;
    int len = _fdPortList._ports.size();
    if (i >= len)
    {
        std::cout << "INVALID PORT ACCESS"<<std::endl;
        return 0;
    }

    int clientBodySize = config.getBodySize(_fdPortList._ports.at(i));
    char clientData[clientBodySize];

    memset(clientData, 0, sizeof(clientData));
    ssize_t bytesRead = recv(_fdPortList._fds[i].fd, clientData, sizeof(clientData), O_NONBLOCK);

    std::cout << "bytes Read: "<<bytesRead<< "\nclientBodySize: "<<clientBodySize<<"  at port: "<< _fdPortList._ports.at(i)<<std::endl;
    #ifdef DEBUG
        std::cout << "Client Data["<<bytesRead<<"]:\n"<<clientData<<std::endl;
    #endif
    if (bytesRead < 0)
        return -1;
    if (bytesRead == 0)
        return 0;


    // converting client data to vector
    size_t charArraySize = bytesRead;
    _byteVector.clear();
    _byteVector.reserve(charArraySize); // Reserve space to avoid reallocations
    for (size_t k = 0; k < charArraySize; ++k)
        _byteVector.push_back(static_cast<uint8_t>(clientData[k]));
    return 69;
}


void ConnectClients::closeConnection(int *i)
{
    int len = _fdPortList._ports.size();
    if (*i >= len)
    {
        len = _fdPortList._fds.size();
        if (*i >= len)
            return;
        close(_fdPortList._fds[*i].fd);
        _fdPortList._fds.erase(_fdPortList._fds.begin() + *i);
        return ;
    }

    Logging::log("Done receiving Data", 200);
    close(_fdPortList._fds[*i].fd);
    _fdPortList._fds.erase(_fdPortList._fds.begin() + *i);
    _fdPortList._ports.erase(_fdPortList._ports.begin() + *i);
    --*i;
}

bool ConnectClients::newConnection(pollfd poll)
{
    int portSize = _fdPortList._sockets.size();
    for (int i = 0; i < portSize; i++)
    {
        if (_fdPortList._sockets.at(i) == poll.fd)
            return (true);
    }
    return false;
}

void ConnectClients::handleData(pollfd poll, configParser& config, int i)
{
    initClientInfo(poll.fd, config);

    std::map<int, clientInfo>::iterator it;
    it = _clientInfo.find(poll.fd);
    Response response(poll.fd, it->second);

    switch (it->second._myHTTPMethod)
    {
        case M_GET:
            it->second._filePos = response.sendRequestedFile();
            std::cout << GRN"filePos: "RESET<<it->second._filePos<<std::endl;
            if (it->second._filePos > 0)
                _fdPortList._fds[i].events = POLLOUT;
            else
                _fdPortList._fds[i].events = 0;
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

void ConnectClients::clientConnected(configParser& config)
{
    int portSize = _fdPortList._fds.size();
    for (int i = 0; i < portSize; ++i)
    {
        pollfd poll = _fdPortList._fds[i];
        if (_fdPortList._fds[i].revents & (POLLIN | POLLOUT))
        {
            if (newConnection(poll))
                initNewConnection(poll.fd);
            else
            {
                switch (receiveData(i, config))
                {
                    case 69:
                        handleData(poll, config, i);
                        break;
                    case 0:
                        closeConnection(&i);
                        break;
                    default:
                        Logging::log("Unable to read Data from connected Client", 500);
                        closeConnection(&i);
//                        exit(69);
                        break;
                }
            }
        }
//        else if (_fdPortList._fds[i].revents & POLLOUT)
//        {
//            std::cout <<GRN"------ -- --- -- - POLLOUT - - ------ -- --- -"RESET<<std::endl;
//            std::map<int, clientInfo>::iterator it;
//            it = _clientInfo.find(poll.fd);
//            Response response(poll.fd, it->second);
//            it->second._isChunkedFile = response.sendShittyChunk();
//
//        }
    }
}



void ConnectClients::connectClients(configParser& config)
{
    initFdList();

    std::cout << GRN " . . Server ready to connect Clients" RESET << std::endl;
    while (69)
    {
//            std::cout << YEL"while loop"RESET<<std::endl;
        // poll checks _fdList for read & write events at the same time
        switch (poll(&_fdPortList._fds[0], _fdPortList._fds.size(), config.get_timeout()))
        {
            case -1:
                exitWithError("Poll function returned Error [EXIT]");
                break;
            case 0:
                Logging::log("waiting for client to connect", 200);
                break;
            default:
                clientConnected(config);
                break;
            std::cout << GRN"poll loop"RESET<<std::endl;
        }
    }
}