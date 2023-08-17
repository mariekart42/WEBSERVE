#include "../header/Response.hpp"


Response::Response(const std::vector<uint8_t>& input, int clientSocket, const std::string& url)
{
    std::cout << "BEFORE doing anything" << std::endl;
    _info._input = input;
    _info._clientSocket = clientSocket;
    _info._statusCode = 200;
    _info._url = url;

}

Response::~Response()
{
    // for (std::map<int, std::ofstream>::iterator it = _fileStreams.begin(); it != _fileStreams.end(); ++it)
    // {
    //     it->second.close();
    // }
}
//Response::Response() {}


// FOR GET:
//  - url
//  - contentType
void Response::getResponse()
{
    sendRequestedFile();
}




bool Response::postResponse(std::string filename, int bytesLeft, std::string contentType)
{
    _info._postInfo._filename = filename;
    _info._postInfo._bytesLeft = bytesLeft;
    // _info._postInfo._boundary // NEED LATER
    std::cout<< GRN"DEBUG: postResponse: content-Type: " << contentType<<""RESET<<std::endl;
    if (contentType == "multipart/form-data")
        return saveRequestToFile();
    else if (contentType == "application/x-www-form-urlencoded")
    {
        std::cout << GRN"DEBUG: it's application/x-www-form-urlencoded"RESET<<std::endl;
        urlDecodedInput();
        // std::string tmp(_info._input.begin(), _info._input.end());
        // size_t foundPos = tmp.find("textData=");
        // std::string testData;
        // if (foundPos != std::string::npos)
        // {
        //     std::cout << "input: "<<tmp<<std::endl;
        //     size_t endPos = tmp.find("\0", foundPos);// CHanged from ; to ' '
        //     if (endPos != std::string::npos)
        //     {
        //         testData = tmp.substr(foundPos + 9, endPos - (foundPos + 9));

        //         std::cout << GRN"DEBUG: Text data: " << testData << ""RESET<< std::endl;
        //     }
        // }
        // std::string decodedInput = decodeURL(testData);
        // std::cout << "filename: "<<filename<<std::endl;
        // _fileStreams[_info._clientSocket].open((UPLOAD_FOLDER + _info._postInfo._filename).c_str(),  std::ios::binary);
        // // _fileStreams[_info._clientSocket].write((testData.c_str()),testData.size());
        // _fileStreams[_info._clientSocket].write((decodedInput.c_str()),strlen(decodedInput.c_str()));
        // _fileStreams[_info._clientSocket].close();
        // mySend(FILE_SAVED);
    }
    return false;
}





std::string Response::getContentType()
{
   if (_info._url.find('.') != std::string::npos)
   {
       size_t startPos = _info._url.find_last_of('.');
       size_t endPos = _info._url.size();

       // from found till end next space:
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
            // statusCode = 500;
            statusCode = 200;
            _file = readFile(PATH_FILE_NOT_SAVED);
        }
        else if (statusCode == FILE_ALREADY_EXISTS)
        {
            // statusCode = 409;
            statusCode = 200;
            _file = readFile(PATH_FILE_ALREADY_EXISTS);
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

    std::cout << "Response Hedaer:\n"GRN<<header<<""RESET<<std::endl;
    // std::cout << "Response Body:\n"GRN<<(_file.data())<<""RESET<<std::endl;
    std::cout << "Response Body:\n"GRN<<std::string(_file.begin(), _file.end())<<" ["<< _file.size() << "]"RESET<<std::endl;

    send(_info._clientSocket, header.c_str(), header.size(), 0);
//    send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
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




// void Response::DELETEResponse() {std::cout << RED "DELETEResponse not working now!"RESET<<std::endl;}



// ^ ^ ^  GET   ^ ^ ^

// --- --- --- --- ---

// v v v  POST  v v v



// void Response::sendResponse()
// {
//     switch (_info._myHTTPMethod)
//     {
//         case M_GET:
//             sendRequestedFile();
//             break;
//         case M_POST:
//             POSTResponse();
//             break;
//         case M_DELETE:
//             DELETEResponse();
//             break;
//         default:
//             mySend(500);
//             exitWithError("unexpected Error: sendResponse can't identifies HTTPMethod [EXIT]");
//     }
// }


// // TODO: INIT LATER IF GET IS DONE
// void Response::POSTResponse()
// {
//     std::cout << RED "POST Response!"RESET<<std::endl;

// //    saveFile();
// //_file = readFile(PATH_500_ERRORWEBSITE);
//     saveRequestToFile();
// //    mySend(500);
// }


//std::string Response::getFileName(const Response::postInfo& info)
//{
//    std::string debug(_file.begin(), _file.end());
//    std::cout << "POST request:\n"RED <<  debug << RESET""<<std::endl;
//    if (info._filename.empty() || info._filename.compare(0, 16, ("tmpFileForSocket_")) == 0)
//    {
//        std::string tmp(_file.begin(), _file.end());
//        size_t foundPos = tmp.find("filename=");
//
//        if (foundPos != std::string::npos)
//        {
//            size_t endPos = tmp.find("\"", foundPos);
//            if (endPos != std::string::npos)
//            {
//                std::string requestedName = tmp.substr(foundPos + 10, endPos - foundPos + 10);  // NOT SURE ABOUT NUMBER
//
//                std::cout << GRN"DEBUG: filename: " << requestedName << ""RESET<< std::endl;
//                return requestedName;
//            }
//        }
//        std::cout << "DEBUG: no filename found in POST request" << std::endl;
//        return ("tmpFileForSocket_" + std::to_string(_clientSocket));
//    }
//    else
//        return info._filename;
//}



//size_t Response::getContentLen()
//{
//    std::string tmp(_file.begin(), _file.end());
//
//    size_t foundPos = tmp.find("Content-Length: ");
//
//    if (foundPos != std::string::npos)
//    {
//        size_t endPos = tmp.find("\n", foundPos);
//        if (endPos != std::string::npos)
//        {
//            std::string requestLenStr = tmp.substr(foundPos + 15, endPos - foundPos - 15);
//            size_t requestLen = static_cast<size_t>(std::strtol(requestLenStr.c_str(), nullptr, 10));
//
//            std::cout << GRN"DEBUG: Content-Length: " << requestLen << ""RESET<< std::endl;
//            return requestLen;
//        }
//    }
//
//    exitWithError("unexpected error: unable do get Content-Lenght from post request");
//    return -1;  // error
//}




void Response::emptyClientPipe()
{
//     //TODO: MAKE LOOP STOP LOL
//     ssize_t bytesRead = 1;
// std::cout << GRN"in empty pipe"RESET<<std::endl;
//     while (bytesRead > 0)
//     {
//         char clientData[MAX_REQUESTSIZE];
//         memset(clientData, 0, MAX_REQUESTSIZE);
//         bytesRead = recv(_info._clientSocket, clientData, sizeof(clientData), O_NONBLOCK);
// std::cout << GRN"lol"RESET<<std::endl;
//     }
//     _info._postInfo._bytesLeft = 0;

ssize_t bytesRead = 0;
    while (true)
    {
        uint8_t tempBuffer[MAX_REQUESTSIZE];
        // memset(tempBuffer, 0, MAX_REQUESTSIZE);
        bytesRead = read(_info._clientSocket, tempBuffer, sizeof(tempBuffer));
std::cout << GRN"lol"RESET<<std::endl;
        
        if (bytesRead == 0) 
        {
            // Pipe is empty
            break;
        } else 
        {
            // Error reading from the pipe
            break;
        }
    }
    std::cout <<GRN"DEBUG: end of empty client pipe with status["<<bytesRead<<"]"RESET<<std::endl;
}


bool Response::saveRequestToFile()
{
//
//    // INIT _POSTMAP
//    std::map<int, postInfo>::iterator it = _postMap.find(_info._clientSocket);
//    if (it == _postMap.end())   // IF NOT INITTED YET
//    {
//        // Entry with key 3 is not present, initialize it
////        postInfo newPostInfo;
////        newPostInfo._bytesLeft = getContentLen() - _file.size();
////        _postMap[_info._clientSocket] = newPostInfo;
//        _info._filename = getFileName(_info);

// std::cout << GRN"entered save request to file"RESET<<std::endl;


    std::string convert(_info._input.begin(), _info._input.end());

    if (convert.compare(0, 4, "POST") == 0)
    {
        if (fileExistsInDirectory(_info._postInfo._filename) == true)
        {
            std::cout << RED"FILE ["<<_info._postInfo._filename<<"] ALREADY EXISTS" RESET<< std::endl;
            emptyClientPipe();
std::cout << GRN"after empty pype"RESET<<std::endl;
            mySend(FILE_ALREADY_EXISTS);
            return false;
        }
std::cout << GRN"first call shit"RESET<<std::endl;

        // only send stuff after main header
        // is this value right? -> for new sholz 873
        size_t startPos = convert.find("\r\n\r\n") + 4; // should search for boundary string here, but this seems more persistent
        std::vector<uint8_t> newVector(_info._input.begin() + startPos, _info._input.end());
        _fileStreams[_info._clientSocket].open((UPLOAD_FOLDER + _info._postInfo._filename).c_str(), std::ios::binary | std::ofstream::app);
        _fileStreams[_info._clientSocket].write(reinterpret_cast<const char*>(newVector.data()),newVector.size());
    }
    else
    {
std::cout << GRN"in is open if"RESET<<std::endl;
        _fileStreams[_info._clientSocket].open((UPLOAD_FOLDER + _info._postInfo._filename).c_str(), std::ios::binary | std::ofstream::app);
        _fileStreams[_info._clientSocket].write(reinterpret_cast<const char*>(_info._input.data()),_info._input.size());
    }


        // _fileStreams[_info._clientSocket].close();
        if (_info._postInfo._bytesLeft <= 0)
        {

            std::cout << RED"DEBUG: done writing to file [FIRST CALL]"RESET<<std::endl;
            std::cout << RED"DEBUG: file path: "<<(UPLOAD_FOLDER + _info._postInfo._filename)<<""RESET<<std::endl;
            mySend(FILE_SAVED);
            _fileStreams[_info._clientSocket].close();
            close(_info._clientSocket);
            return false;
        }

std::cout << GRN"return"RESET<<std::endl;
    return true;


//    }
//    else if (it->second._bytesLeft > 0) // already used and initted before
//    {
//        it->second._filename = getFileName(it->second);
//        _fileStreams[_info._clientSocket].open((UPLOAD_FOLDER + it->second._filename).c_str(), std::ios::binary | std::ios::app);
//        it->second._bytesLeft -= _file.size();
//        _fileStreams[_info._clientSocket].write(reinterpret_cast<const char*>(&_file[0]), _file.size());
//        _fileStreams[_info._clientSocket].close();
//        if (it->second._bytesLeft <= 0)
//        {
//            std::cout << RED"DEBUG: done writing to file [AFTER FIRST CALL]"RESET<<std::endl;
//        }
//    }





//    std::string filename = "nnnew.jpeg";
//
//    std::ofstream outputFile(UPLOAD_FOLDER+filename , std::ios::binary);
//    if (outputFile)
//    {
//        outputFile.write(reinterpret_cast<const char*>(_body.data()), _body.size());
//        outputFile.close();
//        std::cout << "Request bytes saved to file: " << UPLOAD_FOLDER+filename << std::endl;
//        mySend(FILE_SAVED);
//    }
//    else
//    {
//        std::cout << RED"Failed to open or write to the file." RESET<< std::endl;
//        mySend(FILE_NOT_SAVED);
//    }
}


bool Response::fileExistsInDirectory(std::string filename)
{


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



