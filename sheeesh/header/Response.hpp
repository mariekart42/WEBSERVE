#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "main.hpp"
#include "connectClients.hpp"
#include "setServer.hpp"

#include <iostream>

class Response
{
    private:
        std::string _HTTPMethod;
        std::string _url;
        char *_body;
//        int _clientSocket;
//        int _statusCode;
//        std::string _contentType;
//        int _contentLength;
//        std::string _respondFile; //data

//        std::string _clientRequest;
//        int _clientSocket;


    public:
        Response(const std::string&, const std::string&, char*);
        ~Response();

//        void sendResponse(int, std::string, std::string);

};

#endif
