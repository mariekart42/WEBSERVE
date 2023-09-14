/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfuhlenb <vfuhlenb@students.42wolfsburg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 23:17:00 by vfuhlenb          #+#    #+#             */
/*   Updated: 2023/09/14 19:25:18 by vfuhlenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/configParser.hpp"

configParser::configParser(int argc, char **argv) : _context(GLOBAL), _directive_line_nbr(0)
{
	argc < 2 ? _file_path = DEFAULT_CONF : _file_path = argv[1];

	_settings.timeout = TIMEOUT;
	_settings.max_clients = MAX_CLIENTS;
	_settings.buff_size = BUFF_SIZE;
	_settings.max_events = MAX_EVENTS;
	_settings.backlog = BACKLOG;

	// setting settings_check struct members to false
	for (bool* p = &_settings_check.timeout; p <= &_settings_check.backlog; p++)
		*p = false;
}

bool configParser::validConfig()
{
	_file.open(_file_path.c_str());
	try
	{
		if (!_file)
			throw std::invalid_argument("invalid configuration file"); // TODO VF new exception overload
		int count = 0;
		while (getline(_file, _line))
		{
			_directive_line_nbr++;
			if (getToken(_line, 1) == "[server]")
			{
				count++;
				_context = SERVER;
			}
			if (_context == SERVER)
			{
				// Server server(count, _directive_line_nbr);
				Server server;
				server._server_nbr = count;
				server._directive_line_nbr = _directive_line_nbr;
				std::string route;
				std::string route_end;
				while (getline(_file, _line) && getToken(_line, 1) != "[\\server]") // DONE VF handle open Server Block
				{
					_directive_line_nbr++;
					if (_context == LOCATION && getToken(_line, 1).c_str()[0] == '<' && getToken(_line, 1).c_str()[1] != '\\')
						throw std::invalid_argument("nested location not allowed");
					if (getToken(_line, 1).c_str()[0] == '<' && getToken(_line, 1).size() < 3)
						throw std::invalid_argument("location can`t be empty");
					if (getToken(_line, 1).c_str()[0] == '<' && getToken(_line, 1).c_str()[1] != '\\')
					{
						route = getToken(_line, 1);
						route = route.erase(0,1);
						route = route.erase(route.size()-1,1);
						addLocation(server, route);
						_context = LOCATION;
						route_end = route;
						route_end = route_end.insert(0, "<\\").append(">");
					}
					else if (getToken(_line, 1) == route_end)
						_context = SERVER;
					else if (!_line.empty())
						setDirective(server, route);
				}
				_directive_line_nbr++;
				validate_minimal_server_configuration(server);
				_unique_ports.insert(server._port);
				ServersMap::iterator it;
				it = _servers.insert ( std::pair<int,Server>(server._port,server));
				_servers_index.push_back(it->second);
				_context = GLOBAL;
			}
			else if (!_line.empty())
			{
				setGlobal();
			}
		}
		if (!_servers_index.size())
			throw std::runtime_error("no server configuration declared");
	}
	catch (std::exception &e)
	{
		std::cerr << BOLDRED << "Error: in \"" << _file_path << "\" on line " << _directive_line_nbr << " : " << e.what() << RESET << std::endl;
		return false;
	}
	std::cout << BOLDGREEN << "\nInfo: webserv running using configuration \"" << _file_path << "\"" << RESET << std::endl;
	printGlobalSettings();
	printServerDetails();
	return true;
}

configParser::~configParser() {}

Server&	configParser::getServer(const int index)
{
	return _servers_index[index];
}

int configParser::getPort(const int index) const
{
	return _servers_index[index]._port;
}

std::string configParser::getHost(const int index) const
{
	return _servers_index[index]._host;
}

int	configParser::getLineToServerBlock(const int index) const
{
	return _servers_index[index]._server_line_nbr;
}

int	configParser::getSize() const
{
	return _servers_index.size();
}

void configParser::setDirective(Server& server, const std::string& _route)
{
	server._directive_line_nbr = _directive_line_nbr;
	// server
	if (getToken(_line, 1) == "[server]")
		throw std::invalid_argument("open serverblock"); // DONE VF DEBUG
	else if (getToken(_line, 1) == PORT && validate_directive_single(_line))
	{
		if (string_to_int(getToken(_line, 3)) < 1024)
			std::cerr << YELLOW << "Warning: port \"" << getToken(_line, 3) << "\" in line: " << _directive_line_nbr << " -> Ports under 1024 need extended permissions, binding might fail." << RESET << std::endl;
		if (addStatus(server, PORT))
			server._port = string_to_int(getToken(_line, 3));
	}
	else if (getToken(_line, 1) == HOST && validate_directive_single(_line))
	{
		if (addStatus(server, HOST))
			server._host = getToken(_line, 3);
	}
	else if (getToken(_line, 1) == SERVER_NAME && validate_directive_multi(_line))
		setServerName(server, _line);
	else if (getToken(_line, 1) == BODY_SIZE && validate_directive_single(_line))
	{
		if (addStatus(server, BODY_SIZE))
			server._body_size = string_to_int(getToken(_line, 3));
	}
	else if (getToken(_line, 1) == ERROR_PAGE && validate_directive_single(_line))
		setErrorPage(server, _line);
	// server.location
	else if (getToken(_line, 1) == ROOT && validate_directive_single(_line))
		setRoot(server, _line, _route);
	else if (getToken(_line, 1) == METHODS && validate_directive_multi(_line))
		setMethods(server, _line, _route);
	else if (getToken(_line, 1) == AUTOINDEX && validate_directive_single(_line))
		setAutoindex(server, _line, _route);
	else if (getToken(_line, 1) == INDEX && validate_directive_single(_line))
		setIndex(server, _line, _route);
	else if (getToken(_line, 1) == CGI && validate_directive_multi(_line))
		setCGI(server, _line, _route);
	else if (getToken(_line, 1) == REDIRECT && validate_directive_single(_line))
		setRedirect(server, _line, _route);
	else if (getToken(_line, 1) != "[\\server]")
		std::cerr << YELLOW << "Warning: invalid key \"" << getToken(_line, 1) << "skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setGlobal()
{
	if ((getToken(_line, 1) == "timeout") && validate_directive_single(_line))
	{
		if (_settings_check.timeout)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set. skipping line: " << _directive_line_nbr << RESET << std::endl;
		else
		{
			_settings.timeout = string_to_int(getToken(_line, 3));
			if (_settings.timeout < 600)
				std::cerr << YELLOW << "Warning: timeout set to \"" << _settings.timeout << "\" in line: " << _directive_line_nbr << " -> Values under 600 might lead to unstable up and download." << RESET << std::endl;
			_settings_check.timeout = true;
		}
	}
	else if ((getToken(_line, 1) == "max_clients") && validate_directive_single(_line))
	{
		if (_settings_check.max_clients)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
		else
		{
			_settings.max_clients = string_to_int(getToken(_line, 3));
			_settings_check.max_clients = true;
		}
	}
	else if ((getToken(_line, 1) == "buff_size") && validate_directive_single(_line))
	{
		if (_settings_check.buff_size)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
		else
		{
			_settings.buff_size = string_to_int(getToken(_line, 3));
			_settings_check.buff_size = true;
		}
	}
	else if ((getToken(_line, 1) == "max_events") && validate_directive_single(_line))
	{
		if (_settings_check.max_events)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
		else
		{
			_settings.max_events = string_to_int(getToken(_line, 3));
			_settings_check.max_events = true;
		}
	}
	else if ((getToken(_line, 1) == "backlog") && validate_directive_single(_line))
	{
		if (_settings_check.backlog)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
		else
		{
			_settings.backlog = string_to_int(getToken(_line, 3));
			_settings_check.backlog = true;
		}
	}
}

// debug

void configParser::printServerDetails()
{
	std::cout << "\n" << BOLDWHITE << _servers_index.size() << " servers with " << _unique_ports.size() << " unique ports:" << RESET << std::endl;
	int size = _servers_index.size();
	std::cout << std::endl;
	for (int i = 0; i < size; i++)
		printServer(_servers_index[i]);
}

void configParser::printServerDetails(std::ofstream& file)
{
	file << "\n" << _servers_index.size() << " Servers with " << _unique_ports.size() << " unique ports:\n" << std::endl;
	
	int size = _servers_index.size();
	for (int i = 0; i < size; i++)
		printServer(_servers_index[i], file);
}

void configParser::printGlobalSettings()
{
	std::cout << BOLDWHITE << "\nGLOBAL SETTINGS:" << RESET << " timeout = " << _settings.timeout << ", max_clients = " << _settings.max_clients;
	std::cout << ", buff_size = " << _settings.buff_size << ", max_events = " << _settings.max_events;
	std::cout << ", backlog = " << _settings.backlog << std::endl;
}

void configParser::printGlobalSettings(std::ofstream& file)
{
	file << "\nGLOBAL SETTINGS" << "\n\ttimeout = " << _settings.timeout << "\n\tmax_clients =" << _settings.max_clients;
	file << "\n\tbuff_size = " << _settings.buff_size << "\n\tmax_events = " << _settings.max_events;
	file << "\n\tbacklog = " << _settings.backlog << std::endl;
}

void configParser::printLog()
{
	std::ofstream file;

	file.open("servers.log");
	if (!file)
	{
		std::cerr << YELLOW << "Could not create Logfile" << RESET << std::endl;
		return ;
	}
	file << "Configuration file: \"" << _file_path << "\"" << std::endl;
	printGlobalSettings(file);
	printServerDetails(file);
	file.close();
}

void configParser::validate_minimal_server_configuration(Server& server)
{
	StringIntMap::iterator it;
	it = server._status.find(PORT);
	if (it == server._status.end())
	{
		if (addStatus(server, PORT))
			server._port = string_to_int("8080"); // setting default value
		std::cerr << YELLOW << "Warning: port missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of 8080 is set." << RESET << std::endl;
	}

	it = server._status.find(HOST);
	if (it == server._status.end())
	{
		if (addStatus(server, HOST))
			server._host = "0.0.0.0"; // setting default value
		std::cerr << YELLOW << "Warning: host missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of 0.0.0.0 is set." << RESET << std::endl;
	}

	it = server._status.find(BODY_SIZE);
	if (it == server._status.end())
	{
		if (addStatus(server, HOST))
			server._body_size = 8000; // setting default value
		std::cerr << YELLOW << "Warning: body_size missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of 8000 is set." << RESET << std::endl;
	}
}

int configParser::getUniquePorts()
{
	return _unique_ports.size();
}

int configParser::getUniquePorts(const int index)
{

	IntSet::iterator it;
	it = _unique_ports.begin();
	for (int i = 0; i < index; i++)
		it++;
	return *it;
}

int configParser::getTimeout() const
{
	return _settings.timeout;
}

int configParser::getBacklog() const
{
	return _settings.backlog;
}

ServersMap& configParser::getServerMap()
{
	return _servers;
}

int	configParser::getBuffSize() const
{
	return _settings.buff_size;
}

int	configParser::getMaxEvents() const
{
	return _settings.max_events;
}

int	configParser::getMaxClients() const
{
	return _settings.max_clients;
}


int	string_to_int(const std::string& str)
{
	std::istringstream stream(str);
	int number;
	if (stream >> number)
		return number;
	else
		throw std::invalid_argument("not a valid integer"); // TODO VF more precise handling
}

std::string getToken(std::string str, int n)
{
	std::istringstream line(str);
	std::string token;

	for (int i = 1; i <= n; ++i)
	{
		if (!(line >> token))
			return "";
	}

	return token;
}

int countToken(std::string str)
{
	std::istringstream line(str);
	int count = 0;

	std::string token;
	while (line >> token)
		++count;

	return count;
}

int validate_directive_single(std::string str)
{
	if (countToken(str) < 3)
		throw std::invalid_argument("invalid directive syntax");
	else if (getToken(str, 1) == "error_page" && getToken(str, 3) != "=")
		throw std::invalid_argument("invalid directive-delimiter");
	else if (getToken(str, 1) == "error_page" && getToken(str, 4).c_str()[0] == '#')
		throw std::invalid_argument("invalid directive-value");
	else if (getToken(str, 1) != "error_page" && getToken(str, 2) != "=")
		throw std::invalid_argument("invalid directive-delimiter");
	else if (getToken(str, 1) != "error_page" && getToken(str, 3).c_str()[0] == '#')
		throw std::invalid_argument("invalid directive-value");
	else if (getToken(str, 1) != "error_page" && countToken(str) > 3 && getToken(str, 4).c_str()[0] != '#')
		throw std::invalid_argument("invalid directive syntax");
	else if (countToken(str) > 4 && getToken(str, 1) == "error_page" && countToken(str) > 3 && getToken(str, 5).c_str()[0] != '#')
		throw std::invalid_argument("invalid directive syntax");
	return 1;
}

int validate_directive_multi(std::string str)
{
	if (countToken(str) < 3)
		throw std::invalid_argument("invalid directive syntax (too few arguments)");
	else if (getToken(str, 2) != "=")
		throw std::invalid_argument("invalid directive-delimiter");
	else if (getToken(str, 3).c_str()[0] == '#')
		throw std::invalid_argument("invalid directive-value");

	int size = countToken(str);
	if (size > 3)
	{
		int count = 4;
		while (count <= size && getToken(str, count).c_str()[0] != '#')
			count++;
		if (count <= size && getToken(str, count).c_str()[0] != '#')
			throw std::invalid_argument("invalid multi-directive syntax");
	}
	return 1;
}

bool configParser::addStatus(Server& server, const std::string& str)
{
	std::pair<StringIntMap::iterator,bool> ret;
	ret = server._status.insert ( std::pair<std::string,int const>(str,_directive_line_nbr) );
	if (ret.second==false)
	{
    	std::cerr << YELLOW << "Warning: directive \"" << str << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
		return 0;
	}
	return 1;
}

void configParser::addLocation(Server& server, std::string path)
{
	location newLocation;
	
	newLocation._path = path;

	std::pair<StringLocationMap::iterator,bool> ret;
	ret = server._routes.insert ( std::pair<std::string,location>(path,newLocation) );
	if (ret.second==false)
    	std::cerr << YELLOW << "Warning: location with: " << path << " already exist" << RESET << std::endl;
}

void configParser::printServer(Server& server)
{
	std::cout
		<< "server " << server._server_nbr << " [" << server._server_line_nbr << "]"
		<< "\t" << server._host << ":" << server._port
		<< "\t";
	for (size_t i = 0; i < server._server_name.size(); ++i)
		std::cout << server._server_name[i] << " ";
	IntStringMap::iterator it;
	std::cout << "\t";
	for (it = server._error_page.begin(); it != server._error_page.end(); ++it)
		std::cout << it->first << " ";
	StringLocationMap::iterator it2;
	std::cout << "\t";
	for (it2 = server._routes.begin(); it2 != server._routes.end(); ++it2)
		std::cout << it2->second._path << " ";
	std::cout << "\n" << std::endl;
}

void configParser::printServer(Server& server, std::ofstream& file)
{
	file
		<< "SERVER " << server._server_nbr << " [" << server._server_line_nbr << "]"
		<< "\n\tlisten = " << server._port
		<< "\n\thost = " << server._host
		<< "\n\tbody_size = " << server._body_size;
	if (!server._server_name.empty())
	{
		file << "\n\tserver_name = ";
		for (size_t i = 0; i < server._server_name.size(); ++i)
			file << server._server_name[i] << " ";
	}
	{
		if (!server._error_page.empty())
		{	
			IntStringMap::iterator it;
			for (it = server._error_page.begin(); it != server._error_page.end(); ++it)
			{
				file << "\n\terror_page ";	
				file << it->first << " = " << it->second;
			}
		}
	}
	{
		StringLocationMap::iterator it;
		for (it = server._routes.begin(); it != server._routes.end(); ++it)
		{
			file << "\n\t<" << it->second._path << ">";
			if (!it->second._root.empty())
				file << "\n\t\troot = " << it->second._root;
			if (!it->second._index.empty())
				file << "\n\t\tindex = " << it->second._index;
			if (!it->second._methods.empty())
			{
				StringVector::iterator methods;
				file << "\n\t\tmethods = ";
				for (methods = it->second._methods.begin(); methods != it->second._methods.end(); methods++)
					file << *methods << " ";
			}
			if (!it->second._cgi.empty())
			{
				StringVector::iterator cgi;
				file << "\n\t\tcgi = ";
				for (cgi = it->second._cgi.begin(); cgi != it->second._cgi.end(); cgi++)
					file << *cgi << " ";
			}
			if (!it->second._redirect.empty())
				file << "\n\t\tredirect = " << it->second._redirect;
			if (!it->second._autoindex.empty())
				file << "\n\t\tautoindex = " << it->second._autoindex;
		}
		file << std::endl;
	}
	file << std::endl;
}

void configParser::setServerName(Server& server, const std::string& str)
{
	int count = countToken(str);
	int i = 3;
	server._status.insert ( std::pair<std::string,int const>(SERVER_NAME,_directive_line_nbr) );
	while (i <= count && getToken(str, i) != "#") // // TODO VF check for existing entries in Vector
	{
		server._server_name.push_back(getToken(str, i));
		i++;
	}
}

void configParser::setErrorPage(Server& server, const std::string & str)
{
	server._status.insert ( std::pair<std::string,int const>(ERROR_PAGE,_directive_line_nbr) );
	server._error_page.insert ( std::pair<int,std::string>(string_to_int(getToken(str, 2)),getToken(str, 4)));
}

void configParser::setRoot(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._root.empty())
			it->second._root = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setMethods(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		int count = countToken(str);
		int i = 3;
		server._status.insert ( std::pair<std::string,int const>(METHODS,_directive_line_nbr) );
		while (i <= count && getToken(str, i).c_str()[0] != '#') // // TODO VF check for existing entries in Vector
		{
			it->second._methods.push_back(getToken(str, i));
			i++;
		}
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setAutoindex(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (getToken(str, 3) == "on")
			it->second._autoindex = "on";
		else if (getToken(str, 3) != "off")
			throw std::invalid_argument("invalid directive value");
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setIndex(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._index.empty())
			it->second._index = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setCGI(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		int count = countToken(str);
		int i = 3;
		server._status.insert ( std::pair<std::string,int const>(CGI,_directive_line_nbr) );
		while (i <= count && getToken(str, i).c_str()[0] != '#') // TODO VF check for existing entries in Vector
		{
			it->second._cgi.push_back(getToken(str, i));
			i++;
		}
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}

void configParser::setRedirect(Server& server, const std::string &str, const std::string &route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._redirect.empty())
			it->second._redirect = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET << std::endl;
}
