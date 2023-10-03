#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Error.hpp"
#include "configParser.hpp"

#define SEND_CHUNK_SIZE 9000
#define IS_COOKIE 111

#define NO_DATA_TO_UPLOAD (convert.find("POST") == 0 && convert.find(startBoundary) == std::string::npos)
#define IS_FOLDER_OR_FILE (stat((_info._configInfo._rootFolder +"/"+ _info._url).c_str(), &s) == 0)
#define IS_FOLDER (s.st_mode & S_IFDIR)
#define IS_FILE (s.st_mode & S_IFREG)
#define UPLOAD_FOLDER "root/upload/"
#define TMP_CGI "root/tempCGI"


class Request;

struct configInfo
{
    bool                    _autoIndex;
    bool                    _postAllowed;
    bool                    _getAllowed;
    bool                    _deleteAllowed;
    std::string             _indexFile;
    std::string             _rootFolder;
};

struct postInfo
{
    int                     _contentLen;
    std::string             _filename;
    std::string             _boundary;
    std::ofstream*          _outfile;
    std::vector<uint8_t>    _input;
};

struct cgiInfo
{
	std::string             _cgiPath;
	std::string             _query;
	std::string             _fileExtension;
	std::string             _body;
};

struct clientInfo
{
    bool                        _isMultiPart;
    int                         _clientSocket;
    int                         _globalStatusCode;
    bool                        _isChunkedFile;
    postInfo                    _postInfo;
    configInfo                  _configInfo;
	cgiInfo                     _cgiInfo;
    httpMethod                  _myHTTPMethod;
    std::string                 _url;
    std::string                 _fileContentType;
    std::string                 _contentType;
    std::streampos              _filePos;
    std::map<int,std::string>   _errorMap;
	std::vector<std::string>    _cgiFileExtension;
};

class Response
{
    private:
        int                             _localStatusCode;
        clientInfo                      _info;
        std::string                     _header;
        std::vector<uint8_t>            _file;
        std::map<int, std::ofstream>    _fileStreams;


    public:
        Response(int, const clientInfo&);
        ~Response();

        std::streampos          sendRequestedFile();
		std::streampos          mySend(int);
        int                     initFile(int);
        int                     getDirectoryIndexPage(const std::string&);
		int                     validCgiExtension();
		int	                    callCGI();
		int                     inputCheck();
        int                     getRightResponse() const;
        bool                    uploadFile(const std::string&, const std::string&, std::ofstream*);
        bool                    saveRequestToFile(std::ofstream&, const std::string&);
		bool                    checkLanguage() const;
		bool                    cgiOutput();
		bool                    isCgi();
        void                    deleteFile();
        void                    initHeader();
        void                    sendIndexPage();
        void                    sendShittyChunk(const std::string&);
		void                    handleCookies(const std::string&, size_t);
		static size_t           getContentLen(const std::string&);
        std::string             getContentType();
        std::vector<uint8_t>    readFile(const std::string&);
};

#endif
