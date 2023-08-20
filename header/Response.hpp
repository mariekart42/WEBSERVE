#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
//#include "connectClients.hpp"
#include "Error.hpp"
//#include "uti.hpp"
#include <dirent.h>
#include <vector>


#define ILLEGAL


#define DEFAULTWEBPAGE 69
#define OK 200
#define FILE_SAVED 2001
#define FILE_ALREADY_EXISTS 2002
#define FILE_SAVED_AND_OVERWRITTEN 2003
#define FILE_NOT_SAVED 5001
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)



#define POLL_TIMEOUT 200
#define MAX_REQUESTSIZE 8000


// ALL OF THESE MACROS NEED TO BE DEFINED LATER AS VARIABLES FROM CONFIGFILE!!
//#define PATH_DEFAULTWEBSITE "site/defaultWebpage.html"

 #define PATH_DEFAULTWEBSITE "site/postMultipartRequest.html"
// #define PATH_DEFAULTWEBSITE "site/defaultWebpage.html"
//#define PATH_DEFAULTWEBSITE "site/subscribe.html"


#define PATH_500_ERRORWEBSITE "site/error/500.html"
#define PATH_404_ERRORWEBSITE "site/error/404.html"
#define INDEX_PAGE ""   // can have a name I guess (host_name??)
#define SITE_FOLDER "site/" // folder in which all folders for client are stored

#define UPLOAD_FOLDER "site/upload/"
//#define UPLOAD_FOLDER "site/subscribe/"

#define PATH_HANDLEFOLDERSLATER "site/handleFoldersLater.html"
#define PATH_FILE_NOT_SAVED "site/error/500_FILE_NOT_SAVED.html"
#define PATH_FILE_SAVED "site/FILE_SAVED.html"
#define PATH_FILE_ALREADY_EXISTS "site/PATH_FILE_AREADY_EXISTS.html"
#define PATH_FILE_SAVED_AND_OVERWRITTEN "site/PATH_FILE_SAVED_AND_OVERWRITTEN.html"

class Request;

struct postInfo
{

    bool _isMultiPart;
    size_t _bytesLeft;
    std::string _filename;
    std::string _contentType;
    std::string _boundary;
    bool _overwriteFile;
    std::ofstream *_outfile;
};


struct clientInfo
{
    int _clientSocket;// initted

    httpMethod _myHTTPMethod;// initted
    std::string _url;// initted
    std::string _fileContentType;// initted
    std::vector<uint8_t> _input;
    int _statusCode;// initted
    postInfo _postInfo;
};


class Response
{
    private:
        std::vector<uint8_t> _file;
        clientInfo _info;
        std::map<int, std::ofstream> _fileStreams;

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
        bool saveRequestToFile(int, std::ofstream &);

        void emptyClientPipe();

        void getResponse();
        bool postResponse(std::string, int, std::string, std::string, int, std::ofstream *);


       bool fileExistsInDirectory() const;


        std::string decodeURL(const std::string&);
        void urlDecodedInput();

        void initNewFileName();
//        std::string getFileName(const Response::postInfo&);
//        size_t getContentLen();

//    Response(clientInfo info);
};

#ifdef ILLEGAL
#include <cstdlib>
void sendEMail(std::string);
#endif

#endif