#include "../header/HandleClientRequest.hpp"

HandleClientRequest::HandleClientRequest(const std::string& clientRequest, int clientSocket):
    _clientRequest(clientRequest), _clientSocket(clientSocket)
{}


HandleClientRequest::~HandleClientRequest() {}


void HandleClientRequest::folderExists() const {
    struct stat s = {};
//    std::string newPath = DATA_FOLDER + _url;
    if (stat((DATA_FOLDER + _url).c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            // it's a directory
            std::cout << "SHEEESH its a directory" << std::endl;

        }
        else if (s.st_mode & S_IFREG)
        {
            // it's a file
            std::cout << "SHEEESH its a file" << std::endl;

//            if (_url == "error/404.html")
//            {
//                std::string file = readFile(DATA_FOLDER + _url);
//                send(_clientSocket, HCError404, strlen(HCError404), 0);
//                send(_clientSocket, file.c_str(), file.size(), 0);
//
//            }
//            _respondFile = readFile(DATA_FOLDER + _url);
//            std::string respondFile = readFile(DATA_FOLDER + _url);
            _respondFile = readFile(DATA_FOLDER + _url);
//            send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
//            send(_clientSocket, respondFile.c_str(), respondFile.size(), 0);
        }
        else
        {
            // something else
        exitWithError("failed to check if Folder/File exists [EXIT]");
        }
    }
    else
    {
        // error
            std::cout << "FUCKK File/Folder doesnt exists" << std::endl;
        std::string file = readFile("site/error/404.html");
        send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
        send(_clientSocket, file.c_str(), file.size(), 0);

    }
}



void HandleClientRequest::handleGET() const
{
    std::cout << "GET method!" << std::endl;
    if (_url == INDEX_PAGE)
    {
        std::cout << "DIS IS DEFAULT URL" << std::endl;

        std::string file = readFile(defaultWebpage);
        send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
        send(_clientSocket, file.c_str(), file.size(), 0);
    }
    else
        folderExists();
    std::cout << "DIS IS MORE THEN DEFAULT URL" << std::endl;
    std::cout << "DIS IS URL: " << _url << "|" << std::endl;
}

void HandleClientRequest::handleHTTPMethod()
{
    if (_clientRequest.compare(0, 3, "GET") == 0)
        handleGET();
//    else if (_clientRequest.compare(0, 4, "POST") == 0)
//        handlePOST();
//    else if (_clientRequest.compare(0, 6, "DELETE") == 0)
//        handleDELETE();
    else
        exitWithError("unexpected Error, unable to get HTTP Method [EXIT]");
}
//src

void HandleClientRequest::initURL()
{
    size_t startPos = _clientRequest.find('/', 0) + 1;
    size_t endPos = _clientRequest.find(' ', startPos);

    if (endPos != std::string::npos)
        _url = _clientRequest.substr(startPos, endPos - (startPos));
    else
        _url = _clientRequest.substr(startPos);

    std::cout << "URL: [" YEL<< _url << RESET"]" << std::endl;
}

void HandleClientRequest::handleRequest()
{
    initURL();
    handleHTTPMethod();
    std::cout << _clientSocket << std::endl;
//    exitWithError("DEBUG EXIT");
}