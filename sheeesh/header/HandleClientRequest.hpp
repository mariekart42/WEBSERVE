#ifndef HANDLECLIENTREQUEST_HPP
#define HANDLECLIENTREQUEST_HPP

#include "main.hpp"
#include <sys/stat.h>
//#include <io.h>

#define INDEX_PAGE ""
#define DATA_FOLDER "site/" // folder in which all folders for client are stored

#define defaultHTMLPath "/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html"
#define preResponseHardcode "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 1074\r\n\r\n"
#define defaultWebpage "defaultWebpage.html"
class HandleClientRequest
{
    private:
        std::string _clientRequest;
        std::string _url;
        int _clientSocket;

        void handleHTTPMethod();
        void handleGET() const;
        void initURL();
//        void findFolder();
    void folderExists() const;

    public:
        HandleClientRequest(const std::string&, int);
        ~HandleClientRequest();
        void handleRequest();

};

#endif
