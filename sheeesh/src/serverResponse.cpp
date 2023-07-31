#include "serverResponse.hpp"

serverResponse::serverResponse(const std::string &clientResponse, int clientSocket):
    _clientResponse(clientResponse),
    _clientSocket(clientSocket)
{
    std::cout << "SERVER RESPONSE CONSTRUCTOR" << std::endl;
    parseResponse();
}

serverResponse::~serverResponse(){}

void serverResponse::initHTTPMethod()
{
    if (_clientResponse.compare(0, 3, "GET") == 0)
        _HTTPMethod = "GET";
    else if (_clientResponse.compare(0, 4, "POST") == 0)
        _HTTPMethod = "POST";
    else if (_clientResponse.compare(0, 6, "DELETE") == 0)
        _HTTPMethod = "DELETE";
    else
        _HTTPMethod = "";
}

void serverResponse::getURL()
{
    size_t startPosition = _HTTPMethod.size() + 1;
    size_t spacePosition = _clientResponse.find(' ', startPosition);

    if (spacePosition != std::string::npos) {
        // Create the substring using the starting position and the length of the substring
        _url = _clientResponse.substr(startPosition, spacePosition - (startPosition));
    } else
    {
        // If no space is found, extract the substring until the end of the string
        _url = _clientResponse.substr(startPosition);
    }
}

void serverResponse::handleGET()
{
    std::cout << "GET method!" << std::endl;
    getURL();
    if (_url.size() == 1 && _url == "/")
    {
        std::cout << "DIS IS DEFAULT URL" << std::endl;
        std::string file = readFile(defaultWebpage);

        send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
        send(_clientSocket, file.c_str(), file.size(), 0);

        // CHECK header message, CONTENT LEN
        // and also special characters at the very end (i think /n/r)
    }
    else
        std::cout << "DIS IS MORE THEN DEFAULT URL" << std::endl;
    std::cout << "DIS IS URL: " << _url << "|" << std::endl;
}

void serverResponse::parseResponse()
{
    initHTTPMethod();

    if (_HTTPMethod == "GET")
        handleGET();
    else if (_HTTPMethod == "POST")
        std::cout << "POST method!" << std::endl;
    else if (_HTTPMethod == "DELETE")
        std::cout << "DELETE method!" << std::endl;
    else
        exitWithError("unexpected Error: couldn't read HTTP Method");
}
