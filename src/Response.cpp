#include "../header/Response.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

Response::Response(int clientSocket, const clientInfo& cInfo):
        _info(cInfo), _statusCode(-1)
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

//    if (!_info._configInfo._deleteAllowed)
//        return mySend(METHOD_NOT_ALLOWED);
//    if (_info._configInfo._indexFile.empty() && _info._configInfo._autoIndex)
//    {
//        if (_info._url.compare(0, 7, "/upload") == 0)
//        {
//            std::string path = "root" + _info._url;
//            if (std::remove((path).c_str()) != 0)
//                return (mySend(FORBIDDEN));
//            return (mySend(FILE_DELETED));
//        }
//        return (mySend(FORBIDDEN));
//    }
//    else
//    {
//        if (_info._url == FAILURE)
//            return (mySend(FILE_DELETED_FAIL));
//
//        if (_info._url.compare(0, 7, "/upload") == 0)
//        {
//            std::string path = "root" + _info._url;
//            if (std::remove((path).c_str()) != 0)
//                return (mySend(FORBIDDEN));
//            return (mySend(FILE_DELETED));
//        }
//        return (mySend(FORBIDDEN));
//    }

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



std::string Response::generateList(const std::string& rootFolder, const std::string& currentFolder)
{
    std::string filePaths;

    std::string folderPath = rootFolder + "/" + currentFolder;
    DIR* dir = opendir(folderPath.c_str());

    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string itemName = entry->d_name;

            if (itemName != "." && itemName != "..") {
                std::string itemPath = folderPath + "/" + itemName;
                struct stat itemStat;

                if (stat(itemPath.c_str(), &itemStat) == 0)
                {
                    if (S_ISDIR(itemStat.st_mode))
                    {
                        // Recurse into subfolder, passing the current folder
                        std::string subfolderPaths = generateList(rootFolder, currentFolder + "/" + itemName);
                        filePaths += subfolderPaths;
                    } else if (S_ISREG(itemStat.st_mode)) {
                        std::string linkPath = currentFolder + "/" + itemName;
                        filePaths += "\"" + linkPath + "\",";
                    }
                }
            }
        }
        closedir(dir);
    }
    return filePaths;
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
        mySend(getDirectoryIndexPage(""));// changed
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
        return (mySend(METHOD_NOT_ALLOWED));
    if (_info._url.empty())
        return (sendIndexPage());

    // TODO: try CGI

    struct stat s = {};
    if (stat((_info._configInfo._rootFolder +"/"+ _info._url).c_str(), &s) == 0)
    {
        if (IS_FOLDER)
            mySend(getDirectoryIndexPage(_info._url));
        else if (IS_FILE)
        {
            if (_info._url == "/.DS_Store"){
                return (mySend(FORBIDDEN));
            }
            else
            {
                _file = readFile(_info._configInfo._rootFolder +"/"+ _info._url);
                if (_file.empty())   // if file doesn't exist
                    return (mySend(404));
                return (mySend(200));
            }
        }
        else
        {
            Logging::log("ERROR: unexpected Error in sendRequestedFile()", 500);
            return (mySend(500));
        }
    }
    else
        return (mySend(404));
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
       return (contentType);
   }
   Logging::log("is File but can't detect file extension", 500);
   return FAILURE;
}


int Response::initFile(int statusCode)
{
    if (statusCode == 200)
    {
        if ((_info._fileContentType = getContentType()) == FAILURE)
        {
            _file = readFile(PATH_404_ERRORWEBSITE);
            return 404;
        }
        return 200;
    }

    _info._fileContentType = "text/html";
    switch (statusCode)
    {
        case DEFAULTWEBPAGE:
            _file = readFile(_info._configInfo._rootFolder +"/"+ _info._configInfo._indexFile);
            return 200;
        case DIRECTORY_LIST:
            return 200;
        case FILE_SAVED:
            _file = readFile(PATH_FILE_SAVED);
            return 201;
        case FILE_DELETED:
            _file = readFile(PATH_FILE_DELETED);
            return 204;
        case BAD_REQUEST:
            _file = readFile(PATH_BAD_REQUEST);
            return 400;
        case FORBIDDEN:
            _file = readFile(PATH_FORBIDDEN);
            return 403;
//        case FILE_DELETED_FAIL:
//            _file = readFile(PATH_FILE_DELETED_FAIL);
//            return 404;
//        case ERROR_INDEXFILE:
//            _file = readFile(PATH_ERROR_INDEXFILE);
//            return 404;
        case 404:
            _file = readFile(PATH_404_ERRORWEBSITE);
            return 404;
        case METHOD_NOT_ALLOWED:
            _file = readFile(PATH_METHOD_NOT_ALLOWED);
            return 405;
        default:
            _file = readFile(PATH_500_ERRORWEBSITE);
            return 500;
    }
}


void Response::mySend(int statusCode)
{
    _statusCode = initFile(statusCode);

    std::string header = getHeader();
    Logging::log("send Data:\n" + header, 200);

    std::string respooonse = header + std::string(_file.begin(), _file.end());
    int respooonseLen = respooonse.size();

    ssize_t check = send(_info._clientSocket, (respooonse).c_str(), respooonseLen, 0);

//    ssize_t val1 = send(_info._clientSocket, header.c_str(), header.size(), 0);
//    ssize_t val2 = send(_info._clientSocket, (std::string(_file.begin(), _file.end())).c_str(), _file.size(), 0);

    if (check <=0)
    {
        Logging::log("Failed to send Data to Client", 500);
        exit(69);
    }
}


std::string Response::getHeader()
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(_statusCode) + " " +
            ErrorResponse::getErrorMessage(_statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_info._fileContentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(_file.size()) + "\r\n\r\n";
    return header;
}


std::vector<uint8_t> Response::readFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file)
    {
        Logging::log("Failed to open file: " + fileName, 500);
        return static_cast<std::vector<uint8_t> >(0);
    }

    // Read the file content into a vector
    std::vector<uint8_t> content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
    );
    return content;
}




// ^ ^ ^  GET   ^ ^ ^

// --- --- --- --- ---

// v v v  POST  v v v



bool Response::uploadFile(const std::string& contentType, const std::string& boundary, std::ofstream *outfile)
{
    if (contentType == "multipart/form-data")
        return saveRequestToFile(*outfile, boundary);
//    else if (contentType == "application/x-www-form-urlencoded")
//        urlDecodedInput();
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

    if (NO_DATA_TO_UPLOAD)
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

//    if (endOfFile)
//    {
//        std::cout << "END OF FILE"<<std::endl;
//        outfile.close();
//        if (_info._postInfo._filename == BAD_CONTENT_TYPE)
//        {
//            std::remove((_info._configInfo._rootFolder+"/"+ _info._url + "/"+ _info._postInfo._filename).c_str());// maybe here rootfolder + "/" ...
//            return mySend(BAD_REQUEST), false;
//        }
//        if (!_info._configInfo._postAllowed)
//        {
//            std::remove((_info._configInfo._rootFolder +"/"+ _info._url + "/"+ _info._postInfo._filename).c_str());// maybe here rootfolder + "/" ...
//            return mySend(METHOD_NOT_ALLOWED), false;
//        }
//        return mySend(FILE_SAVED), false;
//    }

    return true;
}
