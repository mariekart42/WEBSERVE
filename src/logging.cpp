#include "../header/logging.hpp"

Logging::Logging(){}
Logging::~Logging(){}

// static void logg(const std::string &message)
// {
// 	std::cout << message << std::endl;
// }

// static void exitWithError(const std::string &msg)
// {
// 	logg(RED"ERROR: " + msg + RESET);
// 	exit(1);
// }

void Logging::log(const std::string& message, int statusCode)
{
    std::ofstream logFile(LOG_PATH, std::ios::app);
    if (!logFile)
    {
        std::cout << RED << "Unable to open Logfile" << RESET << std::endl;
        // exitWithError("Unable to open Logfile [EXIT]");
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