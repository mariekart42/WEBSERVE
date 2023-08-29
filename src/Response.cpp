#include "../header/Response.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

Response::Response(const std::vector<uint8_t>& input, int clientSocket, const std::string& url, const clientInfo& cInfo):
        _info(cInfo)
{
    _info._postInfo._input = input;
    _info._clientSocket = clientSocket;
    _info._statusCode = 200;
    _info._url = url;

}

Response::~Response()
{
}

void Response::deleteFile()
{
    if (_info._configInfo._indexFile.empty() && _info._configInfo._autoIndex == true)
    {
        if (std::remove((UPLOAD_FOLDER + _info._url).c_str()) != 0)
            mySend(FORBIDDEN);
        else
            mySend(FILE_DELETED);
    }
    else
    {
        if (_info._url == FAILURE)
            mySend(FILE_DELETED_FAIL);
        if (std::remove((UPLOAD_FOLDER + _info._url).c_str()) != 0)
            mySend(FORBIDDEN);
        mySend(FILE_DELETED);
    }
}



std::string Response::generateList(const std::string& rootFolder, const std::string& currentFolder = "")
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
                        // Recurse into subfolder
                        std::string subfolderPaths = generateList(rootFolder, currentFolder + "/" + itemName);
                        filePaths += subfolderPaths;
                    } else if (S_ISREG(itemStat.st_mode)) {
                        std::string linkPath = currentFolder.empty() ? itemName : currentFolder.substr(1) + "/" + itemName;
                        filePaths += "\""+linkPath+"\",";
                    }
                }
            }
        }
        closedir(dir);
    }
    return filePaths;
}


//std::string Response::generateList2(const std::string& path)
//{
//    std::string filePaths;
//
//    DIR* dir = opendir(path.c_str());
//
//    if (dir) {
//        struct dirent* entry;
//        while ((entry = readdir(dir)) != NULL) {
//            std::string itemName = entry->d_name;
//
//            if (itemName != "." && itemName != "..") {
//                std::string itemPath = path + "/" + itemName;
//                struct stat itemStat;
//
//                if (stat(itemPath.c_str(), &itemStat) == 0) {
//                    if (S_ISREG(itemStat.st_mode)) {
//                        filePaths += "\"" + itemName + "\",";
//                    } else if (S_ISDIR(itemStat.st_mode)) {
//                        // Add directory path with a trailing slash
//                        filePaths += "\"" + itemName + "/" + "\",";
//                    }
//                }
//            }
//        }
//        closedir(dir);
//    }
//    if (!filePaths.empty())
//        filePaths = filePaths.substr(0, filePaths.size() - 1);
//    return filePaths;
//}


int Response::getDirectoryIndexPage(const std::string& directory)
{
    std::string startHtml = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\">"
                          "<meta name=\"viewport\" content=\"\"width=device-width, initial-scale=1.0\"\"><title>Index of /</title>\n"
                          "<link rel=\"stylesheet\" href=\"styles/styleIndex.css\"></head><body><div class=\"background-image\"></div>\n"
                          "<div class=\"container\"><h1>Index of  /</h1><br><div id=\"fileItems\"></div></div><script>const filePaths = [";
    std::string middleHtml = generateList(directory, "");
    std::string endHtml ="];</script><script src=\"scripts/script.js\"></script></body></html>";

    std::string result = startHtml + middleHtml + endHtml;
    for (size_t i = 0; i < result.length(); ++i)
        _file.push_back(static_cast<uint8_t>(result[i]));

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
            mySend(ERROR_INDEXFILE);
    }
    else if (_info._configInfo._autoIndex)
        mySend(getDirectoryIndexPage(_info._configInfo._rootFolder));
    else
        mySend(FORBIDDEN);
}


void Response::sendRequestedFile()
{
    if (_info._url.empty())
        return (sendIndexPage());

    struct stat s = {};
    if (stat((_info._configInfo._rootFolder + _info._url).c_str(), &s) == 0)
    {
        if (IS_FOLDER)  //-> LATER if config is parsed
            mySend(getDirectoryIndexPage(_info._url));
        else if (IS_FILE)
        {
            if (_info._url == ".DS_Store"){
                mySend(FORBIDDEN);
            }
            else
            {
                _file = readFile(_info._configInfo._rootFolder + _info._url);
                if (_file.empty())   // if file doesn't exist
                    mySend(404);
                mySend(200);
            }
        }
        else
        {
            mySend(500);
            Logging::log("ERROR: unexpected Error in sendRequestedFile()", 500);
//            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
        mySend(404);
}


std::string Response::getContentType()
{
   if (_info._url.find('.') != std::string::npos)
   {
       size_t startPos = _info._url.find_last_of('.');
       size_t endPos = _info._url.size();

       std::string fileExtension;

       if (endPos != std::string::npos)
           fileExtension = (_info._url.substr(startPos + 1, endPos - (startPos)));
       else
           fileExtension = (_info._url.substr(startPos));
       std::string contentType = comparerContentType(fileExtension);
       if (contentType == "FAILURE")
           mySend(404);
       return (contentType);
   }
   Logging::log("is File but can't detect file extension", 500);
   return FAILURE;
}



// if statusCode 200, _file NEEDS so be initialized!!
void Response::mySend(int statusCode)
{
    if (statusCode != 200)     // WRITE FUNCTION THAT RETURNS FILE SPECIFIED ON STATUS CODE
    {
        _info._fileContentType = "text/html";

        if (statusCode == FILE_SAVED)
        {
            statusCode = 201;
            _file = readFile(PATH_FILE_SAVED);
        }
        else if (statusCode == FILE_DELETED)
        {
            statusCode = 204;
            _file = readFile(PATH_FILE_DELETED);
        }
        else if (statusCode == DIRECTORY_LIST)
        {
            statusCode = 200;
        }
        else if (statusCode == FILE_DELETED_FAIL)
        {
            statusCode = 404;
            _file = readFile(PATH_FILE_DELETED_FAIL);
        }
        else if (statusCode == DEFAULTWEBPAGE)
        {
            statusCode = 200;
            _file = readFile(_info._configInfo._rootFolder + _info._configInfo._indexFile);
        }
        else if (statusCode == FORBIDDEN)
        {
            statusCode = 403;
            _file = readFile(PATH_FORBIDDEN);
        }
        else if (statusCode == ERROR_INDEXFILE)
        {
            statusCode = 404;
            _file = readFile(PATH_ERROR_INDEXFILE);
        }
        else if (statusCode == 500)
            _file = readFile(PATH_500_ERRORWEBSITE);
        else if (statusCode == 404)
        {
            _file = readFile(PATH_404_ERRORWEBSITE);
            std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
        else if (statusCode == 6969)
            _file = readFile(PATH_HANDLEFOLDERSLATER);
        else
        {
            _file = readFile(PATH_500_ERRORWEBSITE);
            std::cout << RED"ERROR: status code not defined in mySend()"RESET << std::endl;
        }
    }
    else
    {
//        // can't find file extension
       _info._fileContentType = getContentType();
        if (_info._fileContentType == FAILURE)
            mySend(404);
    }
    std::string header = getHeader(statusCode);

    send(_info._clientSocket, header.c_str(), header.size(), 0);
    send(_info._clientSocket, (std::string(_file.begin(), _file.end())).c_str(), _file.size(), 0);
}


std::string Response::getHeader(int statusCode)
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(statusCode) + " " +
            ErrorResponse::getErrorMessage(statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_info._fileContentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(_file.size()) + "\r\n\r\n";
    return header;
}


std::vector<uint8_t> Response::readFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file)
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
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
    else if (contentType == "application/x-www-form-urlencoded")
        urlDecodedInput();
    return false;
}


bool Response::saveRequestToFile(std::ofstream &outfile, const std::string& boundary)
{
    std::string convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
    std::string startBoundary = "--"+boundary+"\r\n";
    std::string endBoundary = "\r\n--"+boundary+"--";
    std::vector<uint8_t>::iterator startPos69 = _info._postInfo._input.begin();
    std::vector<uint8_t>::iterator endPos69 = _info._postInfo._input.end();
    bool endOfFile = false;

    size_t posStartBoundary = convert.find(startBoundary);
    size_t posEndBoundary = convert.find(endBoundary);

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
        mySend(FILE_SAVED);
        return false;
    }
    return true;
}


