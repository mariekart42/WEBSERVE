#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.h"


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
        std::string getURL();
        std::string getFileContentType(const std::string&);
        int getStatusCode() const;
        std::string getContentType();
        size_t getBytesLeft(const std::string&, const std::string&);
        std::string getFileName(const std::string&, const std::string&);
//        std::vector<uint8_t> getFile() const;
        std::string getBoundary();
    std::ofstream *getOutfile(std::string);

};

#endif