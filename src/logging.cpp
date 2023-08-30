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
    logFile << " ======= ";
    std::time_t currentTime;
    currentTime = std::time(nullptr); // Get current time
    if (statusCode == 200)
    {
        logFile << message << "  " << std::asctime(std::localtime(&currentTime));
    }
    else if (statusCode > 200)
        logFile << "ERROR: "<<message<<"  " << std::asctime(std::localtime(&currentTime));

    logFile.close();
}