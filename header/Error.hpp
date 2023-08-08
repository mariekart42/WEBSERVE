#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <map>
#include <string>

class ErrorResponse
{
    private:
        static std::map<int, std::string> errorMessages;

    public:
        ErrorResponse();
        ~ErrorResponse();
       static void initializeErrorMessages();
       static std::string getErrorMessage(int statusCode);
};



#endif
