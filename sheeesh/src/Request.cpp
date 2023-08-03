#include "../header/Request.hpp"

Request::Request(char *data, int clientSocket):
    _data(data), _clientSocket(clientSocket)
{

}


Request::~Request() {}



std::string Request::getHTTPMethod()
{
    std::string tmp;
    tmp = _data;

    if (tmp.compare(0, 3, "GET") == 0)
        return ("GET");
    else if (tmp.compare(0, 4, "POST") == 0)
        return ("POST");
    else if (tmp.compare(0, 6, "DELETE") == 0)
        return ("DELETE");
    else
        return (exitWithError("unexpected Error, unable to get HTTP Method [EXIT]"), "SHIT");
}


std::string Request::getURL()
{
    std::string tmp;
    tmp = _data;

    size_t startPos = tmp.find('/', 0) + 1;
    size_t endPos = tmp.find(' ', startPos);

    if (endPos != std::string::npos)
        return (tmp.substr(startPos, endPos - (startPos)));
    else
        return (tmp.substr(startPos));
}

char *Request::getBody()
{
    std::string tmp;
    tmp = _data;

    std::string test;

    size_t startPos = tmp.find("\r\n\r\n");
    size_t endPos = tmp.size();

    if (endPos != std::string::npos)
        test = (tmp.substr(startPos, endPos - (startPos)));
    else
        test = (tmp.substr(startPos));
    std::cout << "BODY: " << test << std::endl;

// THIS NOT CORRECT YET
    return (const_cast<char*>(test.c_str()));
}



//
//void Request::folderExists() const
//{
//    struct stat s = {};
//
//    std::string respondFile;
//    serverResponse serverObj(_clientSocket);
//    if (stat((DATA_FOLDER + _url).c_str(), &s) == 0)
//    {
//        if (s.st_mode & S_IFDIR)
//        {
//            // it's a directory
//            std::cout << "SHEEESH its a directory\nCant handle jet, do if config parser is done" << std::endl;
//            respondFile = readFile("site/handleLaterFolders.html");
//
//        }
//        else if (s.st_mode & S_IFREG)
//        {
//            // it's a file
//            std::cout << "SHEEESH its a file" << std::endl;
//
////            if (_url == "error/404.html")
////            {
////                std::string file = readFile(DATA_FOLDER + _url);
////                send(_clientSocket, HCError404, strlen(HCError404), 0);
////                send(_clientSocket, file.c_str(), file.size(), 0);
////            }
////            _respondFile = readFile(DATA_FOLDER + _url);
////            std::string respondFile = readFile(DATA_FOLDER + _url);
//            respondFile = readFile(DATA_FOLDER + _url);
////            send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
////            send(_clientSocket, respondFile.c_str(), respondFile.size(), 0);
//            serverObj.sendResponse(69, "html", respondFile);
//        }
//        else
//        {
//            // something else
//            exitWithError("failed to check if Folder/File exists [EXIT]");
//        }
//    }
//    else
//    {
//        // error
//            std::cout << "FUCKK File/Folder doesnt exists" << std::endl;
//        std::string file = readFile("site/error/404.html");
//        send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
//        send(_clientSocket, file.c_str(), file.size(), 0);
//    }
//}
//
//
//
//void Request::handleGET() const
//{
//    if (_url.empty())
//    {
//        serverResponse serverObj(_clientSocket);
//        std::string file = readFile(defaultWebpage);
//        serverObj.sendResponse(200, "html", file);
////        send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
////        send(_clientSocket, file.c_str(), file.size(), 0);
//    }
//    else
//        folderExists();
//    std::cout << "DIS IS MORE THEN DEFAULT URL" << std::endl;
//    std::cout << "DIS IS URL: " << _url << "|" << std::endl;
//}
//
//void Request::handleHTTPMethod()
//{
//    if (_clientRequest.compare(0, 3, "GET") == 0)
//        handleGET();
////    else if (_clientRequest.compare(0, 4, "POST") == 0)
////        handlePOST();
////    else if (_clientRequest.compare(0, 6, "DELETE") == 0)
////        handleDELETE();
//    else
//        exitWithError("unexpected Error, unable to get HTTP Method [EXIT]");
//}
////src
//
//void Request::initURL()
//{
//    size_t startPos = _clientRequest.find('/', 0) + 1;
//    size_t endPos = _clientRequest.find(' ', startPos);
//
//    if (endPos != std::string::npos)
//        _url = _clientRequest.substr(startPos, endPos - (startPos));
//    else
//        _url = _clientRequest.substr(startPos);
//
//    std::cout << "URL: [" YEL<< _url << RESET"]" << std::endl;
//}
//
//void Request::handleRequest()
//{
//    initURL();
//    handleHTTPMethod();
//    std::cout << _clientSocket << std::endl;
////    exitWithError("DEBUG EXIT");
//}