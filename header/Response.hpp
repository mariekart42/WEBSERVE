#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Error.hpp"

#include <vector>

#define preResponseHardcode "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 909\r\n\r\n"
#define HCError404 "HTTP/1.1 404 BITCH\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: 1074\r\n\r\n"


// ALL OF THESE MACROS NEED TO BE DEFINED LATER AS VARIABLES FROM CONFIGFILE!!
#define PATH_DEFAULTWEBSITE "site/defaultWebpage.html"
#define PATH_500_ERRORWEBSITE "site/error/500.html"
#define PATH_404_ERRORWEBSITE "site/error/404.html"
#define INDEX_PAGE ""   // can have a name I guess (host_name??)
#define SITE_FOLDER "site/" // folder in which all folders for client are stored
#define PATH_HANDLEFOLDERSLATER "site/handleFoldersLater.html"


class Request;
class Response
{
    private:
        httpMethod _HTTPMethod;
        std::string _url;
        int _clientSocket;

        int _statusCode;
        std::vector<uint8_t> _file;
        std::string _contentType;
        char *_body;
        std::string _header;


    public:
        Response(const Request &, int);
        Response();
        ~Response();

        std::string getContentType();
        void sendResponse();
        void sendHeader();
        void sendBody();
        void sendDefaultWebpage();
        httpMethod HTTPMethod();
        void GETResponse();
        void POSTResponse();
        void DELETEResponse();
        void sendRequestedFile();
//        int getContentLen();
        static std::vector<uint8_t> readFile(const std::string&);
        static std::string contentType(const std::string&);
};


std::string getHeader(int statusCode, const std::string& contentType, int contentLength);

#endif