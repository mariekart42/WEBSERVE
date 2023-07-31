#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "main.hpp"
#include "connectClients.hpp"
#include "setServer.hpp"

#define defaultHTMLPath "/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html"
#define preResponseHardcode "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\n"

class serverResponse
{
    private:
        std::string _clientResponse;
        int _clientSocket;
        std::string _url;
        std::string _HTTPMethod;

        void parseResponse();
        void handleGET();
        void getURL();

    public:
        serverResponse(const std::string &, int);
        ~serverResponse();

        void initHTTPMethod();
};

#endif
