#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "main.hpp"
#include "Response.hpp"
#include <sys/stat.h>

//#define INDEX_PAGE ""
#define DATA_FOLDER "site/" // folder in which all folders for client are stored

#define defaultHTMLPath "/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html"
#define preResponseHardcode "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 909\r\n\r\n"
#define defaultWebpage "defaultWebpage.html"
#define HCError404 "HTTP/1.1 404 BITCH\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 1074\r\n\r\n"

class Request
{
    private:
        char *_data;
        int _clientSocket;


//        std::string _url;
//        std::string _respondFile;
//        void handleHTTPMethod();
//        void handleGET() const;
//        void initURL();
////        void findFolder();
//    void folderExists() const;


    public:
        Request(char *, int);
        ~Request();
        std::string getHTTPMethod();
        std::string getURL();
        char *getBody();
};

#endif
