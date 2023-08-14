#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
//#include "connectClients.hpp"
#include "Error.hpp"
//#include "uti.hpp"
#include <dirent.h>
#include <vector>


#define DEFAULTWEBPAGE 69
#define OK 200
#define FILE_SAVED 2001
#define FILE_NOT_SAVED 5001
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)

#define FILE_ALREADY_EXISTS 2002


#define POLL_TIMEOUT 200
#define MAX_REQUESTSIZE 5000


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

struct clientInfo
{
    int _clientSocket;// initted
    bool _isMultiPart;

    httpMethod _myHTTPMethod;// initted
    std::string _url;// initted
    std::string _fileContentType;// initted
    std::vector<uint8_t> _input;
    std::string _filename;
    std::string _contentType;   // only for POST
    size_t _bytesLeft;
    int _statusCode;// initted
};


class Response
{
    private:
        std::map<int, std::ofstream> _fileStreams;
        std::vector<uint8_t> _file;
        clientInfo _info;


    public:
        Response(const std::vector<uint8_t>&, int, const std::string&);

//        Response();
        ~Response();

       std::string getContentType();
        void sendResponse();
        void sendDefaultWebpage();
        // void POSTResponse();
        // void DELETEResponse();
        void sendRequestedFile();
        static std::vector<uint8_t> readFile(const std::string&);
        void mySend(int);
        std::string getHeader(int statusCode);
        void saveRequestToFile();


        void getResponse();
        bool postResponse(std::string, int);


//        bool fileExistsInDirectory(std::string);

//        std::string getFileName(const Response::postInfo&);
//        size_t getContentLen();

//    Response(clientInfo info);
};



#endif