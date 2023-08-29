#include "../header/logging.hpp"

Logging::Logging(){}
Logging::~Logging(){}


void Logging::log(const std::string& message, int statusCode)
{
    std::ofstream logFile(LOG_PATH, std::ios::app);
    if (!logFile)
    {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    if (statusCode == 200)
    {
        logFile << " ============== " << message <<" ============== \n";
    }
    else
        logFile << "ERROR: "<<message<<"\n";

    logFile.close();
}