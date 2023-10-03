#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <fstream>          // ifstream
#include <sstream>          // stringstream
#include <unistd.h>         // read()
#include <sys/socket.h>     // send()
#include <sys/stat.h>       // stat struct -> check for File/Folder
#include <fcntl.h>          // nonblock macro
#include <map>
#include <string>
#include <vector>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <netdb.h>
#include <csignal>
#include <sys/time.h>
#include <sys/wait.h>


#include "logging.hpp"
#include "colors.h"

#define FAILURE "FAILURE"
#define BAD_CONTENT_TYPE "bad_contentType"
#define INFO
#define LOG
#define DEBUG
// #define DEBUG_LEAKS

extern std::string g_cookieName;

void 		exitWithError(const std::string &);
std::string comparerContentType(const std::string& fileExtension);
std::string myItoS(int);
bool 		endsWith(const std::string&, const std::string&);
int			setNonBlocking(int fd);
std::string	generateList(const std::string& rootFolder, const std::string& currentFolder);

#endif
