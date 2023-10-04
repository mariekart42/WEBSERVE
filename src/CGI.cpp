#include "../header/Response.hpp"

size_t Response::getContentLen(const std::string& data)
{
	size_t foundPos = data.find("Content-Length: ") + 16;
	if (foundPos != std::string::npos)
	{
		size_t endPos = data.find('\r', foundPos);
		if (endPos != std::string::npos)
		{
			std::string contentLen = data.substr(foundPos, endPos - (foundPos));
			return atoi(contentLen.c_str());
		}
	}
	return -1;
}

void Response::handleCookies(const std::string& data, size_t pos)
{
	size_t contentLen = getContentLen(data);
	g_cookieName = data.substr(pos, contentLen);
	mySend(DEFAULTWEBPAGE);
}


int Response::validCgiExtension()
{
	std::vector<std::string> allowed_ending;
	allowed_ending.push_back(".py");
	allowed_ending.push_back(".pl");

	if (_info._myHTTPMethod == M_POST)
	{
		std::string	convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
		size_t		body = convert.find("\r\n\r\n");
		if (convert.compare(body + 4, 9, "textData=") == 0)
			return handleCookies(convert, body+13), IS_COOKIE;
		_info._cgiInfo._body = convert.substr(body + 4);
	}
	size_t pos = _info._url.find('?');

	if (pos != std::string::npos)
	{
		_info._cgiInfo._cgiPath = _info._url.substr(0, pos);
		_info._cgiInfo._query = _info._url.substr(pos + 1);
	}
	else
		_info._cgiInfo._cgiPath = _info._url;

	size_t dot = _info._cgiInfo._cgiPath.find_last_of('.');

	if (dot != std::string::npos)
	{
		std::string temp = _info._cgiInfo._cgiPath.substr(dot);

		for (size_t i = 0; i < allowed_ending.size(); i++)
		{
			if (_info._cgiInfo._cgiPath.size() < allowed_ending[i].size())
				continue;
			if (temp == allowed_ending[i])
			{
				_info._cgiInfo._fileExtension = temp;
				return callCGI();
			}
		}
	}
	return 69; // Return false if the file extension is not allowed or not found.
}

bool	Response::checkLanguage() const
{
	int result = false;

	if (_info._cgiInfo._fileExtension == ".py")
		result = std::system("python3 --version");
	else if (_info._cgiInfo._fileExtension == ".pl")
		result = std::system("perl --version");

	remove("temp.txt");
	if (result == 0)
		return true;

	#ifdef INFO
	std::cout << "language is not installed." << std::endl;
	#endif

	return false;
}

int Response::inputCheck()
{
	if (!checkLanguage())
		return INTERNAL_ERROR;
	_info._cgiInfo._cgiPath = _info._configInfo._rootFolder + _info._cgiInfo._cgiPath;

	//not supported file ending
	if (_info._cgiInfo._fileExtension != ".py" && _info._cgiInfo._fileExtension != ".pl")
		return METHOD_NOT_ALLOWED;

	//file doesn't exist
	if (access(_info._cgiInfo._cgiPath.c_str(), F_OK ) != 0)
		return NOT_FOUND;

	//no permission
	if (access(_info._cgiInfo._cgiPath.c_str(), X_OK ) != 0)
		return FORBIDDEN;
	return 69;
}

int Response::callCGI()
{
	int pipefd[2];
	int status;

	if (int check = inputCheck() < 0)
		return check;

	if (!_info._cgiInfo._body.empty())
		_info._cgiInfo._query = _info._cgiInfo._body;

	_info._cgiInfo._query = "QUERY_STRING=" + _info._cgiInfo._query;

	char *query = (char*)_info._cgiInfo._query.c_str();

	std::string exec;

	if (_info._cgiInfo._fileExtension == ".py")
		exec = "python3";
	else
		exec = "perl";

	char *env[] = {query, 0};
	char *cmd = (char*)_info._cgiInfo._cgiPath.c_str();
	char *argv[] = {const_cast<char *>(exec.c_str()), cmd, 0};

	int file = open(TMP_CGI, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if (pipe(pipefd) == -1)
	{
		#ifdef INFO
		std::cerr << BOLDRED << "Error CGI: Something went wrong creating the pipe!" << RESET << std::endl;
		#endif
		close(file);
		return INTERNAL_ERROR;
	}

	int pid = fork();
	if (pid == -1)
	{
		#ifdef INFO
		std::cerr << BOLDRED << "Error CGI: Something went wrong with fork" << RESET << std::endl;
		#endif
		close(file);
		return INTERNAL_ERROR;
	}
	else if (pid == 0)
	{
		close(pipefd[0]);
		dup2(file, STDOUT_FILENO);
		alarm(CGI_TIMEOUT/1000);

		if (_info._cgiInfo._fileExtension == ".py")
			execve("/usr/bin/python3", argv, env);
		else
			execve("/usr/bin/perl", argv, env);

		close(file);
		exit(1);
	}
	else
	{
		close(pipefd[1]);
		waitpid(pid, &status, 0);
		close(pipefd[0]);
	}

	close(file);
	if (WIFSIGNALED(status))
	{
		remove(TMP_CGI);
		return GATEWAY_TIMEOUT;
	}
	return 0;
}

bool Response::cgiOutput()
{
	std::ifstream inputFile(TMP_CGI);
	//error check if file is open
	if (!inputFile.is_open())
		return mySend(INTERNAL_ERROR);

	std::string header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
	std::string line;
	std::string respooonse;

	while (std::getline(inputFile, line))
		respooonse += line + "\n";
	respooonse = header + myItoS(respooonse.size()) + "\r\n\r\n" + respooonse;

	std::string convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
	#ifdef LOG
	Logging::log("send Data:\n" + _info._cgiInfo._cgiPath, 200);
	#endif

	ssize_t check = send(_info._clientSocket, (respooonse).c_str(), respooonse.size(), MSG_DONTWAIT);
	if (check <= 0)
	{
		#ifdef LOG
		Logging::log("Failed to send Data to Client", 500);
		#endif
		inputFile.close();
	}
	remove(TMP_CGI);
	inputFile.close();
	return false;
}
