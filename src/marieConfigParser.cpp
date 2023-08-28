#include "../header/marieConfigParser.hpp"

MarieConfigParser::MarieConfigParser() {}
MarieConfigParser::~MarieConfigParser() {}

std::string MarieConfigParser::getUrl(int port, std::string prevUrl)
{
    if (port == -1)
        exitWithError("port is invalid/not found");
    return prevUrl;
}

std::string MarieConfigParser::getRootFolder(int port)
{
    return (ROOT_FOLDER);
}

int MarieConfigParser::getClientBodysize(int port)
{
    return (PORT);
}

bool MarieConfigParser::getAutoIndex(int port)
{
    return (AUTO_INDEX);
}

std::string MarieConfigParser::getIndexFile(int port)
{
    return (INDEX_FILE);
}