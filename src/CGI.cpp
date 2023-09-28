 #include "../header/Response.hpp"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/time.h>

// #define TIMEOUT_CGI 30000 // TODO PLEASE REDEFINE IT REEEeeee

bool	Response::checkForP(void){

	int result = false;

	if (_cgiInfo._fileEnding == ".py") {
		result = std::system("python3 --version");
	} else if (_cgiInfo._fileEnding == ".pl") {
		result = std::system("perl --version");
	}

	if (result == 0) {
		std::cout << "language is installed." << std::endl;
		remove("temp.txt");
		return true;
	}

	std::cout << "language is not installed." << std::endl;
	remove("temp.txt");
	return false;
}

bool Response::validCGIextension() {
	std::vector<std::string> allowed_ending;
	allowed_ending.push_back(".py");
	allowed_ending.push_back(".pl");
	std::string	temp;
	size_t		dot;
	size_t pos = _info._url.find("?");
	if (pos != std::string::npos){
		this->_cgiInfo._cgiPath = this->_info._url.substr(0, pos);
		this->_cgiInfo._query = this->_info._url.substr(pos+1);
	}
	else
		this->_cgiInfo._cgiPath = this->_info._url;

	// Find the last dot in the _cgiPath
	std::cout << "Path is " << _cgiInfo._cgiPath << std::endl;
	std::cout << "Extension check" << std::endl;
	dot = _cgiInfo._cgiPath.find_last_of('.');
	if (dot == std::string::npos)
		return false;
	temp = _cgiInfo._cgiPath.substr(dot);
	for (size_t i = 0; i < allowed_ending.size(); i++)
	{
		std::cout << "We compare " << temp << " with " << allowed_ending[i] << std::endl;
		if (_cgiInfo._cgiPath.size() < allowed_ending[i].size())
			continue;
		if(temp == allowed_ending[i]){
			_cgiInfo._fileEnding = temp;
			return true;
		}
	}
	return false; // Return false if the file extension is not allowed or not found.
}



int Response::callCGI(){
	int pipefd[2];
	int status;
	int result;


	struct timeval start;
	struct timeval end;

	if (checkForP() == false)
		return -1;
	_cgiInfo._cgiPath = this->_info._configInfo._rootFolder + _cgiInfo._cgiPath;

	//file doesn't exist
	if(access(this->_cgiInfo._cgiPath.c_str(), F_OK ) != 0){
		return -2;
	}

	//no permission
	if(access(this->_cgiInfo._cgiPath.c_str(), X_OK ) != 0){
		return -4;
	}

	//not supported file ending
	if(_cgiInfo._fileEnding != ".py" && _cgiInfo._fileEnding != ".pl")
		return -5;

	if (!_cgiInfo._body.empty())
		_cgiInfo._query = _cgiInfo._body;

	_cgiInfo._query = "QUERY_STRING=" + _cgiInfo._query;
	std::cout << "created query: "<< _cgiInfo._query << std::endl;


char *query = (char*)_cgiInfo._query.c_str();
const char *exec;

	if (_cgiInfo._fileEnding == ".py") {
		exec = "python3";
	} else {
		exec = "perl";
	}
	char *env[] = {query, 0};
	char *p = (char*)exec;
	char *cmd = (char*)_cgiInfo._cgiPath.c_str();
	char *argv[] = {p, cmd, 0};

	int file = open("root/tempCGI", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); //TODO hardcoded. Do whatever for naming or keep it
	gettimeofday(&start, 0);


	if (pipe(pipefd) == -1) {
		std::cerr << "Something went wrong creating the pipe!" << std::endl;
		exit(1); // TODO VF implement Exception
	}

	int pid = fork();
	if (pid == -1) {
		std::cerr << "Something went wrong with fork" << std::endl;
	}

	if (pid == 0) {
		close(pipefd[0]);
		dup2(file, STDOUT_FILENO);
		if (_cgiInfo._fileEnding == ".py"){
			if (execve("/usr/bin/python3", argv, env) == -1) {
				std::cerr << "what is wrong" << std::endl;
				close(file);
				exit(1);
			}
		}
		else
			if (execve("/usr/bin/perl", argv, env) == -1) {
				std::cerr << "what is wrong" << std::endl;
				close(file);
				exit(1);
			}
		close(file);
	}
	else {
		// usleep(TIMEOUT * 1000);
		close(pipefd[1]);
		result = waitpid(pid, &status, 0);

		if (result == 0)
			std::cout << RED << "CHILD ALIVE" << RESET << std::endl;
	}
	gettimeofday(&end, 0);

	int diff = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
	std::cout << "Time out: " << diff  << "ms compared with " << TIMEOUT << "ms" << std::endl;
	if (TIMEOUT > 0 && diff >= TIMEOUT ) {
		remove("root/tempCGI");//TODO here as well
		return -3;
	}
	return 0;
}


bool Response::CGIoutput(){
	std::ifstream inputFile("root/tempCGI");
	//error check if file is open
	if (!inputFile.is_open())
		return (mySend(500));
	std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: "; // little clean up here needed
	std::stringstream ss;
	std::string line;
	std::string respooonse;

	while (std::getline(inputFile, line))
		respooonse += line + "\n";
	ss << respooonse.size();
	respooonse = header + ss.str() + "\r\n\r\n" + respooonse;

	std::string convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
	Logging::log("send Data:\n" + _cgiInfo._cgiPath, 200);


	ssize_t check = send(_info._clientSocket, (respooonse).c_str(), respooonse.size(), 0);
	if (check <=0)
	{
		Logging::log("Failed to send Data to Client", 500);
		exit(69);
	}
	remove("root/tempCGI");
	return false;
}
