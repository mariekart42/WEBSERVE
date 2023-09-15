 #include "../header/Response.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/time.h>

#define TIMEOUT 200 // TODO PLEASE REDEFINE IT REEEeeee

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
/* 	size_t	ending = _headers.find(_cgi_allowed_file_ending);
	std::string	header;
	if (ending == std::string::npos){
		std::cout << "no ending"<< std::endl;
		return 1;
	}
	if (pos == std::string::npos){
		std::cout << "no newline"<< std::endl;
		return 1;
	}

	if (_type == "POST"){
		header = _headers.substr(5, pos - 6);
	}
	else
		header = _headers.substr(4, pos - 5); //first we get the first line of the buffer
	size_t	qmark = header.find("?");
	std::string temp = header;
	if(qmark != std::string::npos){
		this->_query = header.substr(qmark+1);
		temp = header.substr(1, qmark-1);
	}
	else {
		temp = header.substr(1);
	}
	_cgi = temp;
	return 0;
}
 */

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

	std::vector<const char*> envp;
	envp.push_back(_query.c_str());
	std::vector<char*> casted;
	casted.push_back(const_cast<char*>(envp[0]));
	std::cout << "_query=" << _query << std::endl;
	const char *path_info = "root/cgi-bin/simple.py";
	std::cout << ">>" << _cgiPath.c_str()<< "<<" << std::endl;
	std::cout << ">>root/cgi-bin/simple.py<<" << std::endl;
	const char *pythonexec = "python3";
	char *python = (char*)pythonexec;
	char* cmd = (char*)path_info;
	char* argv[] = {python, cmd, nullptr};
	int file = open("temp", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

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
//		dup2(file, STDOUT_FILENO);
//		close(file);
		if (execve("/usr/bin/python3", argv, NULL) == -1) {
			std::cerr << "Error: execve failed" << std::endl;
			std::cerr << _cgiPath << std::endl;
			close(file);
		}
	}
	else {
			close(pipefd[1]);
			waitpid(pid, &status, 0);
		}
	gettimeofday(&end, NULL);
	close(file);
	int diff = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
	//std::cout << "Time out: " << diff  << "ms compared with " << TIMEOUT << "ms" << std::endl;
		if (diff >= TIMEOUT ) {
			remove("temp");
			return -3;
		}
	return 0;
}
