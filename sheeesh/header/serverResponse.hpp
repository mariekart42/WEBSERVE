#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "main.hpp"
#include "connectClients.hpp"
#include "setServer.hpp"



class serverResponse
{
    private:
        int _statusCode;
        std::string contentType;
        int contentLength;
        std::string _respondFile;

//        std::string _clientRequest;
//        int _clientSocket;


    public:
        serverResponse();
        ~serverResponse();

};

#endif
