#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "main.hpp"
#include "connectClients.hpp"
#include "setServer.hpp"



class serverResponse
{
    private:
        std::string _clientRequest;
        int _clientSocket;


    public:
        serverResponse(const std::string &, int);
        ~serverResponse();

        void sendResponse();
};

#endif
