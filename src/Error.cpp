#include "../header/Error.hpp"

std::map<int, std::string> ErrorResponse::errorMessages;

ErrorResponse::ErrorResponse()
{}

ErrorResponse::~ErrorResponse() {}

void ErrorResponse::initializeErrorMessages()
{
    errorMessages[69] = "default Webpage";
    errorMessages[200] = "OK";
    errorMessages[201] = "Created";
    errorMessages[204] = "No Content";
    errorMessages[400] = "Bad Request";
    errorMessages[403] = "Forbidden";
    errorMessages[404] = "Not Found";
    errorMessages[405] = "Method not allowed";
    errorMessages[409] = "Conflict";
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