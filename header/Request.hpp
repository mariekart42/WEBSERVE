#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "utils.h"
#include <vector>

enum httpMethod{
    M_GET,
    M_POST,
    M_DELETE,
    M_error
};

class Request
{
    private:
        std::vector<uint8_t> _clientData;
        std::string _tmp;

    public:
        Request(const std::vector<uint8_t>&);
        ~Request();
        httpMethod getHTTPMethod() const;
        std::string getURL() const;
        std::vector<uint8_t> getBody() const;
};

#endif