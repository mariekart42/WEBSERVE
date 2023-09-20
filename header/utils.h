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

#include "logging.hpp"

#define FAILURE "FAILURE"
#define BAD_CONTENT_TYPE "bad_contentType"
#define INFO
//#define DEBUG


// Colour shit
# define RED   "\x1B[31m"
# define GRN   "\x1B[32m"
# define YEL   "\x1B[33m"
# define BLU   "\x1B[34m"
# define MAG   "\x1B[35m"
# define CYN   "\x1B[36m"
# define RESET "\x1B[0m"

#define RESET_COLOR	"\033[0m"
#define BLACK		"\033[30m"
#define RED_COLOR	"\033[31m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define BLUE		"\033[34m"
#define MAGENTA		"\033[35m"
#define CYAN		"\033[36m"
#define WHITE		"\033[37m"
#define BOLDBLACK	"\033[1m\033[30m"
#define BOLDRED		"\033[1m\033[31m"
#define BOLDGREEN	"\033[1m\033[32m"
#define BOLDYELLOW	"\033[1m\033[33m"
#define BOLDBLUE	"\033[1m\033[34m"
#define BOLDMAGENTA	"\033[1m\033[35m"
#define BOLDCYAN	"\033[1m\033[36m"
#define BOLDWHITE	"\033[1m\033[37m"


void exitWithError(const std::string &);
std::string comparerContentType(const std::string& fileExtension);
std::string myItoS(int);
bool endsWith(const std::string&, const std::string&);
//std::vector<uint8_t> readFile(const std::string&);
std::string generateList(const std::string& rootFolder, const std::string& currentFolder);

#endif
