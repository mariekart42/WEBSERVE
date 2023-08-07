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
        char *_clientData;
        int _statusCode;


    public:
        Request(char *);
        ~Request();
    httpMethod getHTTPMethod() const;
        std::string getURL() const;
        char *getBody() const;
        int getStatusCode() const;
};

#endif