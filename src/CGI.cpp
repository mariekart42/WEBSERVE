 #include "../header/Response.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/time.h>

#define TIMEOUT 300 // TODO PLEASE REDEFINE IT REEEeeee

bool	Response::checkForPython(void){
	int result = std::system("python3 --version > temp.txt");

	if (result == 0) {
		std::cout << "Python is installed." << std::endl;
		remove("temp.txt");
		return true;
	}
	std::cout << "Python is not installed." << std::endl;
	remove("temp.txt");
	return false;
}

bool	Response::validCGIfile()
{//should be changed into ending check

	std::string _cgi_allowed_file_ending = ".py"; //please change this into ending
	std::cout << this->_info._url << std::endl;
	size_t	pos = this->_info._url.find("?");
	if (pos != std::string::npos){			//seperate query
		this->_query = this->_info._url.substr(pos+1);
		this->_cgiPath = this->_info._url.substr(0, pos);
	}
	else
		this->_cgiPath = this->_info._url;
	pos = this->_cgiPath.find(_cgi_allowed_file_ending);
	if (pos == std::string::npos)
		return false;
	return true;
}

int Response::CGIpy() {
	int pipefd[2];
	int status;


	struct timeval start;
	struct timeval end;

	if (checkForPython() == false)
		return -1;
	_cgiPath = this->_info._configInfo._rootFolder + _cgiPath;
	std::cout << "_cgiPath: >>" << _cgiPath<<"<<" << std::endl;
	if(access(this->_cgiPath.c_str(), F_OK ) != 0){
		std::cout << "no cgi file" << std::endl;
		return -2;
	}
	_query = "QUERY_STRING=" + _query;
	char *query = (char*)_query.c_str();
	const char *pythonexec = "python3";
	char *env[] = {query, NULL};
	char *python = (char*)pythonexec;
	char* cmd = (char*)_cgiPath.c_str();
	char* argv[] = {python, cmd, NULL};
	int file = open("temp", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); //TODO hardcoded. Do whatever for naming or keep it

	gettimeofday(&start, NULL);

	int pid = fork();
	if (pid == -1) {
		std::cerr << "Something went wrong with fork" << std::endl;
	}

	if (pid == -1) {
		std::cerr << "Something went wrong with fork" << std::endl;
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(file, STDOUT_FILENO);
		close(file);
	
		if (execve("/usr/bin/python3", argv, env) == -1) {
			std::cerr << "what is wrong" << std::endl;
			exit(1);
		}
	}
	else {
		close(pipefd[1]);
		waitpid(pid, &status, 0);
	}
	gettimeofday(&end, NULL);
	close(file);
	int diff = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
	std::cout << "Time out: " << diff  << "ms compared with " << TIMEOUT << "ms" << std::endl;
	if (diff >= TIMEOUT ) {
		remove("temp");//TODO here as well
		return -3;
	}
	return 0;
}


void Response::CGIoutput(){
    _file = readFile("temp"); //todo change it or nah

    Logging::log("send Data:\n" + _cgiPath, 200);

    std::string respooonse = std::string(_file.begin(), _file.end());
	std::cout << respooonse<<std::endl;
    int respooonseLen = respooonse.size();

    ssize_t check = send(_info._clientSocket, (respooonse).c_str(), respooonseLen, 0);

    if (check <=0)
    {
        Logging::log("Failed to send Data to Client", 500);
        exit(69);
    }
}