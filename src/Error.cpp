#include "../header/Error.hpp"

std::map<int, std::string> ErrorResponse::errorMessages;

ErrorResponse::ErrorResponse()
{}

ErrorResponse::~ErrorResponse() {}

void ErrorResponse::initializeErrorMessages()
{
    errorMessages[200] = "OK";
    errorMessages[400] = "Bad Request";
    errorMessages[404] = "Not Found";
    errorMessages[500] = "Internal Server Error";
}

std::string ErrorResponse::getErrorMessage(int statusCode)
{
    initializeErrorMessages();
    std::map<int, std::string>::iterator it = errorMessages.find(statusCode);

    if (it != errorMessages.end())
        return it->second;
    else
        return "Unknown Error";
}