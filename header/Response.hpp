#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"

#include "Error.hpp"
#include <dirent.h>
#include <vector>


#define NO_DATA_TO_UPLOAD (convert.find("POST") == 0 && convert.find(startBoundary) == std::string::npos)





#define DEFAULTWEBPAGE 2001
#define FILE_SAVED 2012
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)
#define FILE_DELETED 2043
#define FILE_DELETED_FAIL 4044
#define FORBIDDEN 4035
#define ERROR_INDEXFILE 4036
#define DIRECTORY_LIST 2007
#define BAD_REQUEST 4008
#define METHOD_NOT_ALLOWED 4059

#define POLL_TIMEOUT 200
//#define MAX_REQUESTSIZE 8080         // max for recv() is 1048576 bytes




#define PATH_500_ERRORWEBSITE "error/500.html"
#define PATH_404_ERRORWEBSITE "error/404.html"
#define PATH_ERROR_INDEXFILE "error/PATH_ERROR_INDEXFILE.html"
#define PATH_FORBIDDEN "error/403.html"
#define PATH_BAD_REQUEST "error/400.html"
//#define PATH_HANDLEFOLDERSLATER "error/handleFoldersLater.html"// delete later
#define PATH_METHOD_NOT_ALLOWED "error/405.html"

// NEED FROM CONFIG PARSER!

#define UPLOAD_FOLDER "root/upload/"


#define PATH_FILE_SAVED "root/PATH_FILE_SAVED.html"
#define PATH_FILE_DELETED "root/PATH_FILE_DELETED.html"
#define PATH_FILE_DELETED_FAIL "root/PATH_FILE_DELETED.html"


class Request;


struct configInfo
{
    std::string _indexFile; // set to index.html/php... or FAILURE
    std::string _rootFolder;
    bool _autoIndex;
    bool _postAllowed;
    bool _getAllowed;
    bool _deleteAllowed;
};

struct postInfo
{
    std::vector<uint8_t> _input;
    std::string _filename;
    std::string _boundary;
    std::ofstream *_outfile;
};


struct clientInfo
{
    int _clientSocket;

    httpMethod _myHTTPMethod;
    std::string _url;
    std::string _fileContentType;
    std::string _contentType;
    bool _isMultiPart;

    postInfo _postInfo;
    configInfo _configInfo;
};


class Response
{
    private:
        std::vector<uint8_t> _file;
        clientInfo _info;
        int _statusCode;
        std::map<int, std::ofstream> _fileStreams;

    public:
        Response(int, const clientInfo&);
        ~Response();


        std::string getContentType();

        static std::vector<uint8_t> readFile(const std::string&);
        std::string getHeader();
        void mySend(int);
        int initFile(int);

        std::string decodeURL(const std::string&);
        std::string generateList(const std::string&, const std::string&);
        int getDirectoryIndexPage(const std::string&);
        void sendIndexPage();

        // POST
        void    sendRequestedFile();
        bool    uploadFile(const std::string&, const std::string&, std::ofstream*);
        bool    saveRequestToFile(std::ofstream&, const std::string&);
//        void    urlDecodedInput();

        // DELETE
        void    deleteFile();

};

#endif