#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <fstream>

// #include "utils.h"

#define LOG_PATH "log/log.log"
//#define INFO_LOG_PATH ""

class Logging
{
    private:

    public:
        Logging();
        ~Logging();
        static void log(const std::string&, int);
};

#endif
