#include "../header/connectClients.hpp"


volatile sig_atomic_t	g_shutdown_flag = 0;

static void signalHandler(int sigNum)
{
	if (sigNum == SIGINT || sigNum == SIGTERM)
	{
		std::cout << "\nReceived shutdown signal. Terminating webserv..." << std::endl;
		g_shutdown_flag = 1;
	}
}

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
                if (setNonBlocking(_fdPortList._fds[k].fd == -1))
                {
                    #ifdef INFO
                        std::cout << BOLDRED << "fcntl error, could not set flag to O_NONBLOCK" << RESET << std::endl;
                    #endif
                }
                _fdPortList._fds[k].events = POLLIN;
                break;
            }
        }
    }
}


void ConnectClients::initNewConnection()
{
    int socketFd = CURRENT_FD;
    int newClientSocket = accept(socketFd, (struct sockaddr *) &_clientAddress, &_clientAddressLen);

    if (newClientSocket == -1)
    {
        close(newClientSocket);
        return ;
    }

    // Find an available slot or expand the vector
    bool foundSlot = false;
    int fdListSize = _fdPortList._fds.size() /3 ;
    for (int j = 0; j < fdListSize; j++)
    {
        if (_fdPortList._fds[j].fd == -1)
        {
            _fdPortList._fds[j].fd = newClientSocket;
            if (setNonBlocking(_fdPortList._fds[j].fd))
            {
                #ifdef INFO
                    std::cout << BOLDRED << "fcntl error, could not set flag to O_NONBLOCK" << RESET << std::endl;
                #endif
            }
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

void ConnectClients::initClientInfo(configParser& config)
{
    std::vector<uint8_t> input = _byteVector;
    int clientSocket = _fdPortList._fds[_x].fd;

    std::map<int, clientInfo>::iterator rm = _clientInfo.find(clientSocket);
    if (rm != _clientInfo.end() && rm->second._filePos > 0)
        return;
    if (rm != _clientInfo.end() && !rm->second._isMultiPart)
        _clientInfo.erase(rm);

    std::map<int, clientInfo>::iterator it = _clientInfo.find(clientSocket);
    if (it == _clientInfo.end())   // IF NOT INITTED YET
    {
        clientInfo initNewInfo;
        Request request(input);

        config.setData(request.getUrlString(), "127.0.0.1", request.getPort());

        initNewInfo._myHTTPMethod = request.getHTTPMethod();
        initNewInfo._clientSocket = clientSocket;
        initNewInfo._url = config.getUrl();
        if (request.traversalAttack(initNewInfo._url))
            initNewInfo._globalStatusCode = FORBIDDEN;
        initNewInfo._fileContentType = request.getFileContentType(initNewInfo._url);
        initNewInfo._contentType = request.getContentType();
        initNewInfo._isMultiPart = false;
        initNewInfo._configInfo._rootFolder = ROOT;
        initNewInfo._configInfo._autoIndex = config.getAutoIndex();
        initNewInfo._configInfo._indexFile = config.getIndexFile();
        initNewInfo._errorMap = config.getErrorMap();
        initNewInfo._isChunkedFile = false;
        initNewInfo._filePos = 0;
        initNewInfo._globalStatusCode = 200;
        if (initNewInfo._myHTTPMethod == M_POST)
        {
            initNewInfo._postInfo._input = input;
            initNewInfo._postInfo._contentLen = request.getContentLen();
            if (initNewInfo._postInfo._contentLen > config.get_body_size())
                initNewInfo._globalStatusCode = REQUEST_TOO_BIG;

            initNewInfo._configInfo._postAllowed = config.getPostAllowed();
            initNewInfo._postInfo._filename = request.getFileName(initNewInfo._contentType, initNewInfo._postInfo._filename, UPLOAD_FOLDER);
            std::string temp_filename = UPLOAD_FOLDER + initNewInfo._postInfo._filename;
            initNewInfo._postInfo._outfile = new std::ofstream (temp_filename.c_str(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);
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
        _clientInfo[clientSocket] = initNewInfo;
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

int ConnectClients::receiveData(configParser& config)
{
    if (_fdPortList._fds[_x].revents & POLLOUT)
        return 69;
    int len = _fdPortList._ports.size();
    if (_x >= len)
        return 0;

    int clientBodySize = config.getBodySize(_fdPortList._ports.at(_x));
    char clientData[clientBodySize];

    memset(clientData, 0, sizeof(clientData));
    // ssize_t bytesRead = recv(_fdPortList._fds[_x].fd, clientData, sizeof(clientData), MSG_DONTWAIT); // before
    ssize_t bytesRead = recv(_fdPortList._fds[_x].fd, clientData, sizeof(clientData), 0);

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


void ConnectClients::closeConnection()
{
    int len = _fdPortList._ports.size();
    if (_x >= len)
    {
        int fdListLen = _fdPortList._fds.size() / 3;
        if (_x >= fdListLen)
            return;
        close(_fdPortList._fds[_x].fd);
        _fdPortList._fds.erase(_fdPortList._fds.begin() + _x);
        _x--; // This is necessary otherwise the webserver is unstable
        return ;
    }
    #ifdef LOG
        Logging::log("Done receiving Data", 200);
    #endif
    close(_fdPortList._fds[_x].fd);
    _fdPortList._fds.erase(_fdPortList._fds.begin() + _x);
    _fdPortList._ports.erase(_fdPortList._ports.begin() + _x);
    _x--;
}

bool ConnectClients::newConnection()
{
    int portSize = _fdPortList._sockets.size();
    for (int i = 0; i < portSize; i++)
    {
        if (_fdPortList._sockets.at(i) == _fdPortList._fds[_x].fd)
            return (true);
    }
    return false;
}

void ConnectClients::setPollEvent(int filePos)
{
    std::map<int, clientInfo>::iterator it;
    it = _clientInfo.find(_fdPortList._fds[_x].fd);

    it->second._filePos = filePos;

    if (filePos > 0)
        _fdPortList._fds[_x].events = POLLOUT;
    else
    {
        _fdPortList._fds[_x].events = 0;
        closeConnection();
    }
}

void ConnectClients::handleData(configParser& config)
{
    initClientInfo(config);

    std::map<int, clientInfo>::iterator it;
    it = _clientInfo.find(_fdPortList._fds[_x].fd);

    if (it == _clientInfo.end())
    {
        // closeConnection(); // VF testing
        return ;
    }

    Response response(_fdPortList._fds[_x].fd, it->second);

    switch (it->second._myHTTPMethod)
    {
        case M_GET:
//            it->second._filePos = response.sendRequestedFile ();
            setPollEvent(response.sendRequestedFile());
            break;
        case M_POST:
            it->second._isMultiPart = response.uploadFile(it->second._contentType,
                                                          it->second._postInfo._boundary,
                                                          it->second._postInfo._outfile);
            if (!it->second._isMultiPart)
            {
                it->second._postInfo._outfile->close();
                delete it->second._postInfo._outfile;
            }
            break;
        case M_DELETE:
            response.deleteFile();
            break;
        default:
            #ifdef LOG
                Logging::log("cant detect HTTPMethod", 500);
            #endif
            break;
    }
}

void ConnectClients::clientConnected(configParser& config)
{
    int fdListLen = _fdPortList._fds.size() / 3;
    for (_x = 0; _x < fdListLen; _x++)
    {
        if (INCOMING_DATA)
        {
            if (g_shutdown_flag == 0 && newConnection())
                initNewConnection();
            else
            {
                switch (receiveData(config))
                {
                    case 69:
                        handleData(config);
                        break;
                    case 0:
                        closeConnection();
                        break;
                    default:
                        #ifdef LOG
                            Logging::log("Unable to read Data from connected Client", 500);
                        #endif
                        closeConnection();
                        break;
                }
            }
        }
    }
}

void ConnectClients::connectClients(configParser& config)
{
    initFdList();
    std::cout << GRN " . . Server ready to connect Clients" RESET << std::endl;
    #ifdef DEBUG_LEAKS
        int counter = 0;
    #endif
    while (69)
    {
        signal(SIGINT, signalHandler);
	    signal(SIGTERM, signalHandler);
        // poll checks _fdList for read & write events at the same time
        if (g_shutdown_flag == 1)
        {
            clientConnected(config);
            break;
        }
        int ret = 0;
        if (!g_shutdown_flag)
            ret = poll(&_fdPortList._fds[0], _fdPortList._fds.size(), config.get_timeout());
        switch (ret)
        {
            case -1:
                if (!g_shutdown_flag)
                    exitWithError("Poll function returned Error [EXIT]");
                break;
            case 0:
                // #ifdef LOG
                //     Logging::log("waiting for client to connect", 200);
                // #endif
                break;
            default:
                clientConnected(config);
                break;
        }
        #ifdef DEBUG_LEAKS
            counter++;
            if (counter == 1000)
                break;
        #endif
    }
}
