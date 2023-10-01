#include "../header/logging.hpp"

Logging::Logging(){}
Logging::~Logging(){}

void Logging::log(const std::string& message, int statusCode)
{
    std::ofstream logFile(LOG_PATH, std::ios::app);
    if (!logFile)
    {
        #ifdef INFO
        std::cout << RED << "Unable to open Logfile" << RESET << std::endl;
        #endif
        return;
    }
//    logFile << " ======= ";
    time_t currentTime;
    currentTime = time(0); // Get current time
    tm* timeinfo = localtime(&currentTime);
    std::string time_format = "%Y-%m-%d %H:%M:%S";
    char buffer[80];
    strftime(buffer, 80, time_format.c_str(), timeinfo);

    if (statusCode == 200)
    {
        logFile << "\n=== " << buffer << " === " << message << std::endl;
    }
    else if (statusCode > 200)
        logFile << "\n" << buffer << " === ERROR: " << message << std::endl;

    logFile.close();
}