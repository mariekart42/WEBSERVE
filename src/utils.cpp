#include "../header/utils.h"

void logg(const std::string &message)
{
	std::cout << message << std::endl;
}

void exitWithError(const std::string &msg)
{
	logg(RED"ERROR: " + msg + RESET);
	exit(1);
}

// Function to convert IPv4 address (32-bit integer) to string
std::string ipv4ToString(uint32_t addr) 
{
    std::stringstream ss;
    ss << (addr >> 24 & 0xFF) << "." << (addr >> 16 & 0xFF)
       << "." << (addr >> 8 & 0xFF) << "." << (addr & 0xFF);
    return ss.str();
}

//char *readFile(const std::string &fileName)
//{
//    std::ifstream file(fileName, std::ios::binary);
//    if (!file.is_open())
//        return nullptr;
//
//    // Get the file content as a string
//    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//
//    // Allocate memory for the content
//    char* returnContent = new char[content.size() + 1]; // +1 for null-terminator
//
//    // Copy the content to the allocated buffer
//    std::copy(content.begin(), content.end(), returnContent);
//    returnContent[content.size()] = '\0';
//
//    return returnContent;
//}