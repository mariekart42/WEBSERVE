#include "Response.hpp"

//serverResponse::serverResponse(int clientSocket /*Enum method, std::string url, char *body  */):
//    _statusCode(), _contentType(), _contentLength(), _respondFile()
//{
////    std::cout << "SERVER REQUEST CONSTRUCTOR" << std::endl;
//}

Response::Response(const std::string& HTTPMethod, const std::string& url, char *body):
    _HTTPMethod(HTTPMethod), _url(url), _body(body)
{}

Response::~Response()
{
}

//void Response::sendResponse(int statusCode, std::string contentType, std::string respondFile)
//{
////    parse
//    send(_clientSocket, preResponseHardcode, strlen(preResponseHardcode), 0);
//    send(_clientSocket, respondFile.c_str(), respondFile.size(), 0);
//}