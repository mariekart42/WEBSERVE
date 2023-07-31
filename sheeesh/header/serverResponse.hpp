#ifndef SERVERRESPONSE_HPP
#define SERVERRESPONSE_HPP

#include "main.hpp"
#include "connectClients.hpp"
#include "setServer.hpp"

#define defaultHTMLPath "/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html"
#define preResponseHardcode "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 661\r\n\r\n"
#define defaultWebpage "/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/defaultWebpage.html"

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
        void sendResponse();
};

#endif
