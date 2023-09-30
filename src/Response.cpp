#include "../header/Response.hpp"
#include <sys/fcntl.h>

Response::Response(int clientSocket, const clientInfo& cInfo):
       _localStatusCode(-1), _info(cInfo), _header()
{
    _info._clientSocket = clientSocket;
}

Response::~Response() {}

void Response::deleteFile()
{
    #ifdef INFO
        std::cout << RED " . . . Received Data  --  DELETE  " <<_info._url<< "" << RESET << std::endl;
    #endif
    #ifdef LOG
        Logging::log("Received Data  --  DELETE  " + _info._url, 200);
    #endif
    if (!_info._configInfo._deleteAllowed)
        return mySend(METHOD_NOT_ALLOWED), (void)0;

    if (_info._configInfo._indexFile.empty() && _info._configInfo._autoIndex && _info._url.compare(0, 7, "/upload") == 0)
    {
        std::string path = _info._configInfo._rootFolder + _info._url;
        if (std::remove(path.c_str()) != 0)
            return mySend(FORBIDDEN), (void)0;
        return mySend(FILE_DELETED), (void)0;
    }

    if (_info._url == FAILURE)
        return mySend(404), (void)0;
    return mySend(FORBIDDEN), (void)0;
}

int Response::getDirectoryIndexPage(const std::string& directory)
{
    std::string htmlFile;
    htmlFile = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\">"
                          "<meta name=\"viewport\" content=\"\"width=device-width, initial-scale=1.0\"\"><title>Index of /</title>\n"
                          "<link rel=\"stylesheet\" href=\"styles/styleIndex.css\"></head><body><div class=\"background-image\"></div>\n"
                          "<div class=\"container\"><h1>Index of  /</h1><br><div id=\"fileItems\"></div></div><script>const filePaths = [";
    htmlFile += generateList(_info._configInfo._rootFolder, directory);
    htmlFile += "];</script><script src=\"scripts/script.js\"></script></body></html>";
    for (size_t i = 0; i < htmlFile.length(); ++i)
        _file.push_back(static_cast<uint8_t>(htmlFile[i]));
    return (DIRECTORY_LIST);
}

void Response::sendIndexPage()
{
    // if there is index.html in root folder
    if (!_info._configInfo._indexFile.empty())
    {
        if (Request::fileExists(_info._configInfo._indexFile, _info._configInfo._rootFolder))
            mySend(DEFAULTWEBPAGE);
        else
            mySend(FORBIDDEN);
    }
    else if (_info._configInfo._autoIndex)
        mySend(getDirectoryIndexPage(""));
    else
        mySend(FORBIDDEN);
}

std::streampos Response::sendRequestedFile()
{
    #ifdef INFO
        std::cout << YEL " . . . Received Data  --  GET  " << _info._url << "" << RESET << std::endl;
    #endif
    #ifdef LOG
        Logging::log("Received Data  --  GET  " + _info._url, 200);
    #endif

    if (!_info._configInfo._getAllowed)
        return mySend(METHOD_NOT_ALLOWED);
    if (_info._url == "/")
        return sendIndexPage(), 0;

    if(validCGIextension() == true)
	{
		int check = callCGI();
		std::cout << "Return of CGI is " << check << std::endl;
		switch (check)
		{
		case -1:
			return(mySend(500));
		case -2:
			return (mySend(404));
		case -3:
			return (mySend(408));
		default:
			std::cout << "Default case has been called" << std::endl;
			std::cout << "Here" << std::endl;
			return (CGIoutput());
		}
	}
    #ifdef DEBUG
        std::cout <<"ALIVE 1"<<std::endl;
    #endif

    struct stat s = {};
    if IS_FOLDER_OR_FILE
    {
        if IS_FOLDER
            return mySend(getDirectoryIndexPage(_info._url));
        else if IS_FILE
        {
            if (endsWith(_info._url, ".DS_Store"))
                return mySend(FORBIDDEN);
            else
            {
                _file = readFile(_info._configInfo._rootFolder + _info._url);
                if (_info._isChunkedFile)
                    return _info._filePos;
                if (_file.empty())
                    return mySend(NOT_FOUND);
                return mySend(OK);
            }
        }
        else
        {
            #ifdef LOG
                Logging::log("ERROR: unexpected Error in sendRequestedFile()", 500);
            #endif
            return mySend(500);
        }
    }
    else
        return mySend(NOT_FOUND);
}

std::string Response::getContentType()
{
    if (_info._url.find('.') != std::string::npos)
    {
       std::string fileExtension;
       size_t startPos = _info._url.find_last_of('.');
       size_t endPos = _info._url.size();

       if (endPos != std::string::npos)
           fileExtension = _info._url.substr(startPos + 1, endPos - (startPos));
       else
           fileExtension = _info._url.substr(startPos);
       std::string contentType = comparerContentType(fileExtension);
       if (contentType == FAILURE)
           mySend(404);
       return contentType;
    }
    #ifdef LOG
        Logging::log("is File but can't detect file extension", 500);
    #endif
    return FAILURE;
}

std::vector<uint8_t> Response::readFile(const std::string &fileName)
{
    if (_info._filePos > 0)
    {
        sendShittyChunk(fileName);
        return static_cast<std::vector<uint8_t> >(0);
    }
    std::ifstream file;
    file.open(fileName.c_str(), std::ios::binary);

    if (!file)
    {
        #ifdef LOG
            Logging::log("Failed to open file: " + fileName, 500);
        #endif
        return static_cast<std::vector<uint8_t> >(0);
    }

    // Read the file content into a vector
    std::vector<uint8_t> content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
    );

    if (_info._myHTTPMethod == M_GET && content.size() > SEND_CHUNK_SIZE)
    {

        _info._fileContentType = getContentType();

        std::string header = "HTTP/1.1 200 " +
                             ErrorResponse::getErrorMessage(200) + "\r\nConnection: keep-alive\r\n"
                             "Content-Type: "+_info._fileContentType+"\r\n"
                             "Content-Length: " + myItoS(content.size()) + "\r\n\r\n";

        #ifdef LOG
            Logging::log("send Data:\n" + header, 200);
        #endif

        char buffer[SEND_CHUNK_SIZE];
        file.seekg(0);
        file.read(buffer, SEND_CHUNK_SIZE);

        std::string response = header.append(buffer, SEND_CHUNK_SIZE);
        int len = response.size();

        int check = send(_info._clientSocket, response.data(), len, 0);
        if (check <=0)
        {
            #ifdef LOG
                Logging::log("Failed to send Data to Client", 500);
            #endif
            exit(69);
        }

        _info._filePos = file.tellg();
        _info._isChunkedFile = true;
        file.close();
        return static_cast<std::vector<uint8_t> >(0);
    }

    _info._isChunkedFile = false;
    return content;
}

int Response::initFile(int statusCode)
{
    if (statusCode == 200)
    {
        if ((_info._fileContentType = getContentType()) == FAILURE)
            return _info._fileContentType = "text/html", _file = readFile(_info._errorMap.at(NOT_FOUND)), _localStatusCode = 404;
        return _localStatusCode = 200;
    }

    _info._fileContentType = "text/html";
    switch (statusCode)
    {
        case DEFAULTWEBPAGE:
            return _file = readFile(_info._errorMap.at(DEFAULTWEBPAGE)), _localStatusCode = 200;
        case DIRECTORY_LIST:
            return _localStatusCode = 200;
        case FILE_SAVED:
            return _file = readFile(_info._errorMap.at(FILE_SAVED)), _localStatusCode = 201;
        case FILE_DELETED:
            return _file = readFile(_info._errorMap.at(FILE_DELETED)), _localStatusCode = 204;
        case BAD_REQUEST:
            return _file = readFile(_info._errorMap.at(BAD_REQUEST)), _localStatusCode = 400;
        case FORBIDDEN:
            return _file = readFile(_info._errorMap.at(FORBIDDEN)), _localStatusCode = 403;
        case NOT_FOUND:
            return _file = readFile(_info._errorMap.at(NOT_FOUND)), _localStatusCode = 404;
        case METHOD_NOT_ALLOWED:
            return _file = readFile(_info._errorMap.at(METHOD_NOT_ALLOWED)), _localStatusCode = 405;
        case REQUEST_TOO_BIG:
            return _file = readFile(_info._errorMap.at(REQUEST_TOO_BIG)), _localStatusCode = 413;
        default:
            return _file = readFile(_info._errorMap.at(INTERNAL_ERROR)), _localStatusCode = 500;
    }
}


std::streampos Response::mySend(int statusCode)
{
    initFile(statusCode);
    if (_info._isChunkedFile)
        return _info._filePos;

    initHeader();
    #ifdef LOG
        Logging::log("send Data:\n" + _header, 200);
    #endif

    std::string header = _header;
    std::string response = header + std::string(_file.begin(), _file.end());
    const char* response_data = response.data();
    int len = response.size();

    int check = send(_info._clientSocket, response_data, len, 0);
    if (check <=0)
    {
        #ifdef LOG
            Logging::log("Failed to send Data to Client", 500);
        #endif
        exit(69);
    }

    return 0;
}



void Response::sendShittyChunk(const std::string& fileName)
{
    std::ifstream file;
    file.open(fileName.c_str(), std::ios::binary);

    if (!file)
    {
        #ifdef LOG
            Logging::log("Failed to open file: " + fileName, 500);
        #endif
        _info._filePos = 0;
        return ;
    }

    char	buffer[SEND_CHUNK_SIZE];
    file.seekg(_info._filePos);

    // Read a chunk of data from the file
    file.read(buffer, SEND_CHUNK_SIZE);

    if (send(_info._clientSocket, buffer, file.gcount(), O_NONBLOCK | MSG_DONTROUTE | MSG_OOB) == -1)
    {
        #ifdef LOG
            Logging::log("Failed to send Data to Client", 500);
        #endif
        exit(69);
    }

    if (file.eof())
    {
        file.close();
        _info._filePos = 0;
        return;
    }

    _info._filePos = file.tellg();
    _info._isChunkedFile = true;
    file.close();
    return ;
}



void Response::initHeader()
{
    _header = "HTTP/1.1 " + myItoS(_localStatusCode) + " " +
            ErrorResponse::getErrorMessage(_localStatusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_info._fileContentType+"\r\n"
                                                                                           "Content-Length: " + myItoS(_file.size()) + "\r\n\r\n";
}


// ^ ^ ^  GET   ^ ^ ^

// --- --- --- --- ---

// v v v  POST  v v v



bool Response::uploadFile(const std::string& contentType, const std::string& boundary, std::ofstream *outfile)
{
    if (contentType == "multipart/form-data")
        return saveRequestToFile(*outfile, boundary);
	else if (contentType == "application/x-www-form-urlencoded")
	{
		std::cout << RED << "FOUND application/x-www-form-urlencoded" << RESET << std::endl;
		validCGIextension();
		{
			std::string	convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
			size_t		body = convert.find("\r\n\r\n");
			_cgiInfo._body = convert.substr(body+4);
			int check = callCGI();
			std::cout << "Return of CGI is " << check << std::endl;
			switch (check)
			{
			case -1:
				return(mySend(500));
			case -2:
				return (mySend(404));
			case -3:
				return (mySend(408));
			case -4:
				return (mySend(403));
			case -5:
				return (mySend(501));
			default:
				std::cout << "Default case has been called" << std::endl;
				std::cout << "Here" << std::endl;
				return (CGIoutput());
			}
		}
	}
	return false;
}

bool Response::saveRequestToFile(std::ofstream &outfile, const std::string& boundary)
{
    #ifdef INFO
        std::cout << BLU " . . . Received Data  --  POST  " <<_info._url<< "" << RESET << std::endl;
    #endif
    #ifdef LOG
        Logging::log("Received Data  --  POST  " + _info._url, 200);
    #endif

    std::string convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
    std::string startBoundary = "--"+boundary+"\r\n";
    std::string endBoundary = "\r\n--"+boundary+"--";
    std::vector<uint8_t>::iterator startPos69 = _info._postInfo._input.begin();
    std::vector<uint8_t>::iterator endPos69 = _info._postInfo._input.end();
    size_t posStartBoundary = convert.find(startBoundary);
    size_t posEndBoundary = convert.find(endBoundary);
    bool endOfFile = false;

    if NO_DATA_TO_UPLOAD
        return true;


    if (posStartBoundary != std::string::npos)  // cut header and put stuff afterward to outfile
    {
        size_t bodyPos = convert.find("\r\n\r\n", (posStartBoundary + startBoundary.size() + 2)) + 4;
        if (bodyPos == std::string::npos)   // not the beginning of body content
            return true;
        startPos69 += bodyPos;
        if (posEndBoundary != std::string::npos)    // found last boundary
        {
            endOfFile = true;
            endPos69 = _info._postInfo._input.begin() + posEndBoundary;
        }
    }
    else if (posEndBoundary != std::string::npos)    // found last boundary
    {
        endPos69 = _info._postInfo._input.begin() + posEndBoundary;
        endOfFile = true;
    }
    std::vector<uint8_t>::iterator it;
    for (it = startPos69; it != endPos69; it++)
        outfile << *it;

    if (endOfFile)
    {
        outfile.close();


        mySend(getRightResponse());
        return false;
    }
    return true;
}


int Response::getRightResponse() const
{
    bool removeFile = true;
    int responseStatus = FILE_SAVED;

    if (_info._globalStatusCode == REQUEST_TOO_BIG)
        responseStatus = REQUEST_TOO_BIG;
    else if (_info._globalStatusCode == FORBIDDEN)
        responseStatus = FORBIDDEN;
    else if (_info._postInfo._filename == BAD_CONTENT_TYPE)
        responseStatus = BAD_REQUEST;
    else if (!_info._configInfo._postAllowed)
        responseStatus = METHOD_NOT_ALLOWED;
    else
        removeFile = false;

    if (removeFile)
        std::remove((_info._configInfo._rootFolder + _info._url + "/" + _info._postInfo._filename).c_str());

    return responseStatus;
}
