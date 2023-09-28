#include "../header/logging.hpp"

Logging::Logging(){}
Logging::~Logging(){}


void Logging::log(const std::string& message, int statusCode)
{
    std::ofstream logFile(LOG_PATH, std::ios::app);
    if (!logFile)
    {
        exitWithError("Unable to open Logfile [EXIT]");
        return;
    }
//    logFile << " ======= ";
    std::time_t currentTime;
    currentTime = std::time(0); // Get current time
    if (statusCode == 200)
    {
        logFile << "\n=== "<< std::asctime(std::localtime(&currentTime)) << "=== "<<message<<std::endl;
    }
    else if (statusCode > 200)
        logFile << "\n"<< std::asctime(std::localtime(&currentTime)) << "=== ERROR: "<<message<<std::endl;

    logFile.close();
}