#include "../header/marieConfigParser.hpp"

MarieConfigParser::MarieConfigParser() {}
MarieConfigParser::~MarieConfigParser() {}

std::vector<int> MarieConfigParser::getPortVector()
{
    std::vector<int> tmpPortVector;
    tmpPortVector.push_back(PORT1);
    tmpPortVector.push_back(PORT2);
    tmpPortVector.push_back(PORT3);
    tmpPortVector.push_back(PORT4);
    std::cout << " . . . " << PORT1<< std::endl;
    std::cout << " . . . "<< PORT2 << std::endl;
    std::cout << " . . . "<< PORT3 << std::endl;
    std::cout << " . . . "<< PORT4 << std::endl;
    return tmpPortVector;
}

std::string MarieConfigParser::getUrl(int port, std::string prevUrl)
{
    if (port == -1)
        exitWithError("port is invalid/not found");
    if (!prevUrl.empty() && prevUrl[0] != '/') {
        prevUrl = "/" + prevUrl;
    }


    // TODO: @Valentin always put / in the beginning of path
    return prevUrl;
}

std::string MarieConfigParser::getRootFolder(int port)
{
    return (ROOT_FOLDER);
}

int MarieConfigParser::getClientBodysize(int port)
{
    return (CLIENT_BODY);
}

bool MarieConfigParser::getAutoIndex(int port)
{
    return (AUTO_INDEX);
}

std::string MarieConfigParser::getIndexFile(int port)
{
    if (port == PORT1 || port == PORT2)
        return (INDEX_FILE);
//    if (port == PORT2)
    std::string emprty= "";
        return emprty;
}

bool MarieConfigParser::getPostAllowed(int port)
{
    (void)port;
    return POST_ALLOWED;
}
bool MarieConfigParser::getGetAllowed(int port)
{
    (void)port;
    return GET_ALLOWED;
}
bool MarieConfigParser::getDeleteAllowed(int port)
{
    (void)port;
    return DELETE_ALLOWED;
}