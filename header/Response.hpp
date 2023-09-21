#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Error.hpp"
#include "configParser.hpp"

#define SEND_CHUNK_SIZE 20000

#define NO_DATA_TO_UPLOAD (convert.find("POST") == 0 && convert.find(startBoundary) == std::string::npos)
#define IS_FOLDER_OR_FILE (stat((_info._configInfo._rootFolder +"/"+ _info._url).c_str(), &s) == 0)
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)
#define UPLOAD_FOLDER "root/upload/"

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
    std::string     _filename;
    std::string     _boundary;
    std::ofstream*  _outfile;
};

struct clientInfo
{
    int         _clientSocket;
    httpMethod  _myHTTPMethod;
    std::string _url;
    std::string _fileContentType;
    std::string _contentType;
    bool        _isMultiPart;
    postInfo    _postInfo;
    configInfo  _configInfo;
    std::streampos _filePos;
//    int _intFilePos;
    std::map<int,std::string> _errorMap;
    bool _isChunkedFile;
};

class Response
{
    private:
        int _statusCode;
        clientInfo  _info;
        std::string _header;
        std::vector<uint8_t> _file;
        std::map<int, std::ofstream> _fileStreams;

    public:
        Response(int, const clientInfo&);
        ~Response();

        std::string getContentType();
        void        initHeader();
        int         initFile(int);
    std::streampos        mySend(int);
        int         getDirectoryIndexPage(const std::string&);
        void        sendIndexPage();
    std::streampos        sendRequestedFile();
        bool        uploadFile(const std::string&, const std::string&, std::ofstream*);
        bool        saveRequestToFile(std::ofstream&, const std::string&);
        void        deleteFile();

        std::vector<uint8_t> readFile(const std::string &fileName);

    void sendShittyChunk(const std::string&);
};

#endif
