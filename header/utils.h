#ifndef UTILS_H
#define UTILS_H


#include <set>
#include <map>
#include <ctime>
#include <vector>
#include <cstdio>
#include <string>
#include <poll.h>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <csignal>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <exception>
#include <stdexcept>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include "logging.hpp"


#define FAILURE             "FAILURE"
#define BAD_CONTENT_TYPE    "bad_contentType"


#define INFO
#define LOG


// Colour shit
#define RED         "\x1B[31m"
#define GRN         "\x1B[32m"
#define YEL         "\x1B[33m"
#define BLU         "\x1B[34m"
#define BLUE		"\033[34m"
#define RESET       "\x1B[0m"
#define BOLDRED		"\033[1m\033[31m"
#define BOLDGREEN	"\033[1m\033[32m"
#define BOLDWHITE	"\033[1m\033[37m"
#define RESET_COLOR	"\033[0m"


// globals
extern std::string              g_cookieName;
extern volatile sig_atomic_t    g_shutdown_flag;


void        mySignals();
void        printError();
int			setNonBlocking(int fd);
void        signalHandler(int sigNum);
void 		exitWithError(const std::string &);
bool 		endsWith(const std::string&, const std::string&);
std::string myItoS(int);
std::string comparerContentType(const std::string& fileExtension);
std::string	generateList(const std::string& rootFolder, const std::string& currentFolder);

#endif
