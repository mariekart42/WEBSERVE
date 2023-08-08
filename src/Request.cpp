#include "../header/Request.hpp"

Request::Request(char *clientData):
        _clientData(clientData)
{}

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


char *Request::getBody() const
{
    std::string tmp;
    tmp = _clientData;

    std::string test;

    size_t startPos = tmp.find("\r\n\r\n") + 4;
    size_t endPos = tmp.size();

    if (endPos != std::string::npos)
        test = (tmp.substr(startPos, endPos - (startPos)));
    else
        test = (tmp.substr(startPos));
    std::cout << "== REQUEST body:    [interesting for POST and DELETE]\n["BLU << test<< RESET"]\n" << std::endl;

    char* bodyPtr = new char[test.size() + 1];
    std::strcpy(bodyPtr, test.c_str());

    return bodyPtr; // !1!1! // NEED TO DELETE SOMEWHERE
}

