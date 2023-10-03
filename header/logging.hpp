#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <iostream>
#include <fstream>

#define LOG_PATH "log/log.log"

class Logging
{
    private:
    public:
        Logging();
        ~Logging();
        static void log(const std::string&, int);
};

#endif
