#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Error.hpp"
#include <dirent.h>


#define DEFAULTWEBPAGE 69
#define OK 200
#define FILE_SAVED 2001
#define FILE_NOT_SAVED 5001
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)
#define FAILURE "FAILURE"
#define FILE_ALREADY_EXISTS 2002


#define MAX_REQUESTSIZE 4000000
// ALL OF THESE MACROS NEED TO BE DEFINED LATER AS VARIABLES FROM CONFIGFILE!!
//#define PATH_DEFAULTWEBSITE "site/defaultWebpage.html"
#define PATH_DEFAULTWEBSITE "site/postRequest.html"

#define PATH_500_ERRORWEBSITE "site/error/500.html"
#define PATH_404_ERRORWEBSITE "site/error/404.html"
#define INDEX_PAGE ""   // can have a name I guess (host_name??)
#define SITE_FOLDER "site/" // folder in which all folders for client are stored
#define UPLOAD_FOLDER "site/upload/"
#define PATH_HANDLEFOLDERSLATER "site/handleFoldersLater.html"
#define PATH_FILE_NOT_SAVED "site/error/500_FILE_NOT_SAVED.html"
#define PATH_FILE_SAVED "site/FILE_SAVED.html"
#define PATH_FILE_ALREADY_EXISTS "site/PATH_FILE_AREADY_EXISTS.html"

class Request;
class Response
{
    private:
        httpMethod _HTTPMethod;
        std::string _url;
        int _clientSocket;

        std::vector<uint8_t> _file;
        std::string _contentType;

        std::vector<uint8_t> _body;


    public:
        Response(const Request &, int);
//        Response();
        ~Response();

        std::string getContentType();
        void sendResponse();
        void sendDefaultWebpage();
        void POSTResponse();
        void DELETEResponse();
        void sendRequestedFile();
        static std::vector<uint8_t> readFile(const std::string&);
        void mySend(int);
        std::string getHeader(int statusCode);
        void saveRequestToFile();

        bool fileExistsInDirectory(std::string);
};


#endif