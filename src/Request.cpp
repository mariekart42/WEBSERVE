
#include "../header/Request.hpp"

Request::Request(const std::vector<uint8_t>& clientData):
        _clientData(clientData),
        _tmp(std::string(_clientData.begin(), _clientData.end()))
{}

Request::~Request() {}



httpMethod Request::getHTTPMethod() const
{
    if (_tmp.compare(0, 3, "GET") == 0)
        return M_GET;
    else if (_tmp.compare(0, 4, "POST") == 0)
        return M_POST;
    else if (_tmp.compare(0, 6, "DELETE") == 0)
        return M_DELETE;
    else
        return M_error;
}


std::string Request::getURL() const
{
    size_t startPos = _tmp.find('/', 0) + 1;
    size_t endPos = _tmp.find(' ', startPos);

    if (endPos != std::string::npos)
        return (_tmp.substr(startPos, endPos - (startPos)));
    else
        return (_tmp.substr(startPos));
}


std::vector<uint8_t> Request::getBody() const
{
    std::vector<uint8_t> bodyVector;

    size_t startPos = _tmp.find("\r\n\r\n") + 4;
    size_t endPos = _tmp.size();

    if (endPos != std::string::npos)
        bodyVector.insert(bodyVector.end(), _tmp.begin() + startPos, _tmp.begin() + endPos);
    else
        bodyVector.insert(bodyVector.end(), _tmp.begin() + startPos, _tmp.end());

    // Print the contents of the vector (numeric values)
//    for (size_t i = 0; i < bodyVector.size(); ++i) {
//        std::cout <<GRN ""<< static_cast<int>(bodyVector[i]) << " "RESET;
//    }

    return bodyVector;
}

