#ifndef UTILS_H
#define UTILS_H


#include <iostream>
#include <fstream>          // ifstream
#include <sstream>          // stringstream
#include <unistd.h>         // read()
#include <sys/socket.h>     // send()
#include <sys/stat.h>       // stat struct -> check for File/Folder
#include <fcntl.h>          // nonblock macro


#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>



// Colour shit
# define RED   "\x1B[31m"
# define GRN   "\x1B[32m"
# define YEL   "\x1B[33m"
# define BLU   "\x1B[34m"
# define MAG   "\x1B[35m"
# define CYN   "\x1B[36m"
# define RESET "\x1B[0m"

void exitWithError(const std::string &);
std::string comparerContentType(const std::string& fileExtension);

#endif
