#include "../header/Request.hpp"

Request::Request(char *clientData):
        _clientData(clientData)
{
    _statusCode = 200;
}


Request::~Request() {}



httpMethod Request::getHTTPMethod() const
{
    std::string tmp;
    tmp = _clientData;

    if (tmp.compare(0, 3, "GET") == 0)
        return M_GET;
    else if (tmp.compare(0, 4, "POST") == 0)
        return M_POST;
    else if (tmp.compare(0, 6, "DELETE") == 0)
        return M_DELETE;
    else
        return M_error;
}


std::string Request::getURL() const
{
    std::string tmp;
    tmp = _clientData;

    size_t startPos = tmp.find('/', 0) + 1;
    size_t endPos = tmp.find(' ', startPos);

    if (endPos != std::string::npos)
        return (tmp.substr(startPos, endPos - (startPos)));
    else
        return (tmp.substr(startPos));
}


// DIS SHOULD NOY BE CLIENT DATA BUT SERVER DATA
char *Request::getBody() const
{
    std::string tmp;
    tmp = _clientData;

    std::string test;

    std::cout << "_clientData:\n["RED << _clientData << RESET"]"<<std::endl;

    size_t startPos = tmp.find("\r\n\r\n") + 4;
    size_t endPos = tmp.size();

    if (endPos != std::string::npos)
        test = (tmp.substr(startPos, endPos - (startPos)));
    else
        test = (tmp.substr(startPos));
    std::cout << "BODY:\n[" << test<<"]" << std::endl;

    char* bodyPtr = new char[test.size() + 1];
    std::strcpy(bodyPtr, test.c_str());

    return bodyPtr; // !1!1! // NEED TO DELETE SOMEWHERE
}

int Request::getStatusCode() const {
    return _statusCode;
}










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