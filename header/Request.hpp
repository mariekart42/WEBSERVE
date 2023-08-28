#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.h"

#define DEBUG

enum httpMethod{
    M_GET,
    M_POST,
    M_DELETE,
    M_error
};

class Request
{
    private:
        std::string _tmp;
        int _statusCode;

    public:
        Request(const std::vector<uint8_t>&);
        ~Request();
        httpMethod getHTTPMethod();
        std::string getUrlString();
        int getPort();
        static std::string getFileContentType(const std::string&);
        int getStatusCode() const;
        std::string getContentType();
        std::string getFileName(const std::string&, const std::string&, const std::string&);
        std::string getBoundary();
        static bool fileExists(const std::string&, const std::string&);
        static std::string getNewFilename(const std::string&, const std::string&);
        static bool checkPathInFolder(const std::string&, const std::string&);
        static bool pathExists(const std::string&);
};

#endif