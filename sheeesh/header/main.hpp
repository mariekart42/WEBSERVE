#ifndef MAIN_HPP
#define MAIN_HPP


#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <fstream>      // for infile stream
#include <sstream>

#include "setServer.hpp"
#include "main.hpp"

#define FAILURE -69
#define SUCCESS 0

// Colour shit
# define RED   "\x1B[31m"
# define GRN   "\x1B[32m"
# define YEL   "\x1B[33m"
# define BLU   "\x1B[34m"
# define MAG   "\x1B[35m"
# define CYN   "\x1B[36m"
# define RESET "\x1B[0m"

void exitWithError(const std::string &);
std::string readFile(const std::string &fileName);

#endif
