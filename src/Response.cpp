#include "../header/Response.hpp"

Response::Response(int clientSocket, const clientInfo& cInfo):
        _statusCode(-1),  _info(cInfo),  _header()
{
    _info._clientSocket = clientSocket;
}

Response::~Response() {}

void Response::deleteFile()
{
    #ifdef INFO
        std::cout << RED " . . . Received Data  --  DELETE  " <<_info._url<<""RESET<< std::endl;
    #endif
    Logging::log("Received Data  --  DELETE  " + _info._url, 200);

    if (!_info._configInfo._deleteAllowed)
        return mySend(METHOD_NOT_ALLOWED);

    if (_info._configInfo._indexFile.empty() && _info._configInfo._autoIndex && _info._url.compare(0, 7, "/upload") == 0)
    {
        std::string path = _info._configInfo._rootFolder + _info._url;
        if (std::remove(path.c_str()) != 0)
            return mySend(FORBIDDEN);
        return mySend(FILE_DELETED);
    }

    if (_info._url == FAILURE)
        return mySend(404);
    return mySend(FORBIDDEN);
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

void Response::sendRequestedFile()
{
#ifdef INFO
    std::cout << YEL " . . . Received Data  --  GET  " <<_info._url<<""RESET<< std::endl;
#endif
    Logging::log("Received Data  --  GET  " + _info._url, 200);

    if (!_info._configInfo._getAllowed)
        return mySend(METHOD_NOT_ALLOWED);
    if (_info._url == "/")
        return sendIndexPage();

    // TODO: try CGI
    std::cout <<"ALIVE 1"<<std::endl;

    struct stat s = {};
    if IS_FOLDER_OR_FILE
    {
        if IS_FOLDER
            return mySend(getDirectoryIndexPage(_info._url));
        else if IS_FILE
        {
            if (endsWith(_info._url, "/.DS_Store"))
                return mySend(FORBIDDEN);
            else
            {
                _file = readFile(_info._configInfo._rootFolder +"/"+ _info._url);
                if (_file.empty())   // if file doesn't exist
                    return mySend(404);
                return mySend(200);
            }
        }
        else
            return Logging::log("ERROR: unexpected Error in sendRequestedFile()", 500), mySend(500);
    }
    else
        return mySend(404);
}


std::string Response::getContentType()
{
   if (_info._url.find('.') != std::string::npos)
   {
       std::string fileExtension;
       size_t startPos = _info._url.find_last_of('.');
       size_t endPos = _info._url.size();

       if (endPos != std::string::npos)
           fileExtension = (_info._url.substr(startPos + 1, endPos - (startPos)));
       else
           fileExtension = (_info._url.substr(startPos));
       std::string contentType = comparerContentType(fileExtension);
       if (contentType == FAILURE)
           mySend(404);
       return contentType;
   }
   Logging::log("is File but can't detect file extension", 500);
   return FAILURE;
}


int Response::initFile(int statusCode)
{
    if (statusCode == 200)
    {
        if ((_info._fileContentType = getContentType()) == FAILURE)
            return _info._fileContentType = "text/html", _file = readFile(PATH_404_ERRORWEBSITE), _statusCode = 404;
        return _statusCode = 200;
    }

    _info._fileContentType = "text/html";
    switch (statusCode)
    {
        case DEFAULTWEBPAGE:
            return _file = readFile(_info._configInfo._rootFolder +"/"+ _info._configInfo._indexFile), _statusCode = 200;
        case DIRECTORY_LIST:
            return _statusCode = 200;
        case FILE_SAVED:
            return _file = readFile(PATH_FILE_SAVED), _statusCode = 201;
        case FILE_DELETED:
            return _file = readFile(PATH_FILE_DELETED), _statusCode = 204;
        case BAD_REQUEST:
            return _file = readFile(PATH_BAD_REQUEST), _statusCode = 400;
        case FORBIDDEN:
            return _file = readFile(PATH_FORBIDDEN), _statusCode = 403;
        case 404:
            return _file = readFile(PATH_404_ERRORWEBSITE), _statusCode = 404;
        case METHOD_NOT_ALLOWED:
            return _file = readFile(PATH_METHOD_NOT_ALLOWED), _statusCode = 405;
        default:
            return _file = readFile(PATH_500_ERRORWEBSITE), _statusCode = 500;
    }
}


void Response::mySend(int statusCode)
{
    initFile(statusCode);
    initHeader();
    Logging::log("send Data:\n" + _header, 200);


//    std::string header = _header;
//    std::string response = header + std::string(_file.begin(), _file.end());
//    const char* response_data = response.data();
//int len = response.size();
//int check = send(_info._clientSocket, response_data, len, 0);
//
//    if (check <=0)
//    {
//        Logging::log("Failed to send Data to Client", 500);
//        exit(69);
//    }
//    std::cout << GRN"ALIVE [after single sends]"RESET<<std::endl;





    send(_info._clientSocket, _header.c_str(), _header.size(), 0);
    send(_info._clientSocket, (std::string(_file.begin(), _file.end())).c_str(), _file.size(), 0);
    std::cout << GRN"ALIVE 1 [after 2 sends]"RESET<<std::endl;
}


void Response::initHeader()
{
    _header = "HTTP/1.1 " + std::to_string(_statusCode) + " " +
            ErrorResponse::getErrorMessage(_statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_info._fileContentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(_file.size()) + "\r\n\r\n";
}


// ^ ^ ^  GET   ^ ^ ^

// --- --- --- --- ---

// v v v  POST  v v v



bool Response::uploadFile(const std::string& contentType, const std::string& boundary, std::ofstream *outfile)
{
    if (contentType == "multipart/form-data")
        return saveRequestToFile(*outfile, boundary);
    return false;
}

bool Response::saveRequestToFile(std::ofstream &outfile, const std::string& boundary)
{
    #ifdef INFO
        std::cout << BLU " . . . Received Data  --  POST  " <<_info._url<<""RESET<< std::endl;
    #endif
    Logging::log("Received Data  --  POST  " + _info._url, 200);

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

        //try CGI

        if (_info._postInfo._filename == BAD_CONTENT_TYPE || !_info._configInfo._postAllowed)
        {
            std::remove((_info._configInfo._rootFolder + "/" + _info._url + "/" + _info._postInfo._filename).c_str());
            if (_info._postInfo._filename == BAD_CONTENT_TYPE)
                mySend(BAD_REQUEST);
            else
                mySend(METHOD_NOT_ALLOWED);
        }
        else
            mySend(FILE_SAVED);
        return false;
    }
    return true;
}
