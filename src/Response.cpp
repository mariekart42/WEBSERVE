#include "../header/Response.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

Response::Response(const std::vector<uint8_t>& input, int clientSocket, const std::string& url)
{
    _info._input = input;
    _info._clientSocket = clientSocket;
    _info._statusCode = 200;
    _info._url = url;

}

Response::~Response()
{
}

void Response::deleteFile(){};



void Response::sendRequestedFile()
{
    if (_info._url == INDEX_PAGE)
        return (sendDefaultWebpage());

    struct stat s = {};
    int statusCode = OK;

    if (stat((SITE_FOLDER + _info._url).c_str(), &s) == 0)
    {
        if (IS_FOLDER)  //-> LATER if config is parsed
        {
            statusCode=6969;
            exitWithError("Cant handle Folders jet, do if config parser is done");
        }
        else if (IS_FILE)
        {
            _file = readFile(SITE_FOLDER + _info._url);
            if (_file.empty())   // if file doesn't exist
                statusCode = 404;
        }
        else
        {
            statusCode = 500;
            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
        statusCode = 404;
    mySend(statusCode);
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

   std::cout << RED"ERROR: is File but can't detect file extension"RESET<<std::endl;
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
            statusCode = 200;
            _file = readFile(PATH_FILE_SAVED);
        }
        else if (statusCode == FILE_NOT_SAVED)
        {
            statusCode = 200;
            _file = readFile(PATH_FILE_NOT_SAVED);
        }
        else if (statusCode == FILE_ALREADY_EXISTS)
        {
            statusCode = 200;
            _file = readFile(PATH_FILE_ALREADY_EXISTS);
        }
        else if (statusCode == FILE_SAVED_AND_OVERWRITTEN)
        {
            statusCode = 200;
            _file = readFile(PATH_FILE_SAVED_AND_OVERWRITTEN);

        }
        else if (statusCode == DEFAULTWEBPAGE)
        {
            statusCode = 200;
            _file = readFile(PATH_DEFAULTWEBSITE);
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
            exitWithError("status code not defined in mySend() [EXIT]");
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


void Response::sendDefaultWebpage()
{
    // _file = readFile(PATH_DEFAULTWEBSITE);
    // if (_file.empty())
    // {
    //     mySend(500);
    //     std::cout << RED"ERROR: unexpected Error, path to defaultWebsite wrong or no defaultWebsite provided"RESET << std::endl;   // LATER WRITE IN ERROR FILE
    //     return;
    // }
    mySend(DEFAULTWEBPAGE);
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
    std::string convert(_info._input.begin(), _info._input.end());
    std::string startBoundary = "--"+boundary+"\r\n";
    std::string endBoundary = "\r\n--"+boundary+"--";
    std::vector<uint8_t>::iterator startPos69 = _info._input.begin();
    std::vector<uint8_t>::iterator endPos69 = _info._input.end();
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
            endPos69 = _info._input.begin() + posEndBoundary;
        }
    }
    else if (posEndBoundary != std::string::npos)    // found last boundary
    {
        endPos69 = _info._input.begin() + posEndBoundary;
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





bool Response::fileExistsInDirectory() const
{
    std::string filename = _info._postInfo._filename;

   DIR* dir = opendir(UPLOAD_FOLDER);
   if (dir == nullptr) {
       std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
       return false;
   }

   struct dirent* entry;
   while ((entry = readdir(dir)) != nullptr) {
       if (strcmp(entry->d_name, filename.c_str()) == 0) {
           closedir(dir);
           return true;
       }
   }

   closedir(dir);
   return false;
}


