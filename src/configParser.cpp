/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfuhlenb <vfuhlenb@students.42wolfsburg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 23:17:00 by vfuhlenb          #+#    #+#             */
/*   Updated: 2023/09/17 17:46:14 by vfuhlenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../header/configParser.hpp"
#include <algorithm>
#include <stdexcept>

configParser::configParser() : _context(GLOBAL), _directive_line_nbr(0)
{
	_settings.timeout = TIMEOUT;
	_settings.max_clients = MAX_CLIENTS;
	_settings.body_size = BODY_SIZE;
	_settings.max_events = MAX_EVENTS;
	_settings.backlog = BACKLOG;
	_request_data.port = 0;

	// setting settings_check struct members to false
	for (bool* p = &_settings_check.timeout; p <= &_settings_check.backlog; p++)
		*p = false;
}

configParser::~configParser() {}

bool	configParser::setData(const std::string& url, const std::string& host, const int port) 
{
	_request_data.full_path = url;
	_request_data.host = host; // TODO VF do we process this?
	_request_data.port = port;
	parse_request_data();
	create_port_vector();
	return true;
}

bool configParser::validConfig(int argc, char **argv)
{
	argc < 2 ? _file_path = DEFAULT_CONF : _file_path = argv[1];

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
				Server server;
				server._server_nbr = count;
				server._directive_line_nbr = _directive_line_nbr;
				server._server_line_nbr = _directive_line_nbr;
				server._error_map = _default_error_map;
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
				_unique_ports_sorted.insert(server._port);
				
				std::pair<ServersMap::iterator,bool> ret;
				ret = _servers.insert ( std::pair<int,Server>(server._port,server));
				if (ret.second == false)
				{
					throw std::invalid_argument("multiple configuration with same port not allowed.");
				}
				else
				{
					_servers_index.push_back(server);
					_context = GLOBAL;
				}
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
		std::cerr << BOLDRED << "Error: in \"" << _file_path << "\" on line " << _directive_line_nbr << " : " << e.what() << RESET_COLOR << std::endl;
		return false;
	}
	std::cout << BOLDGREEN << "\nInfo: webserv running using configuration \"" << _file_path << "\"" << RESET_COLOR << std::endl;
	printGlobalSettings();
	printServerDetails();
	printLog();
	return true;
}

const std::string	configParser::getUrl()
{
	RouteIterator route;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	if (route != getServer(_request_data.port)._routes.end() && !route->second._redirect.empty())
	{
		std::string route_temp = route->second._redirect;
		if (route_temp.size() > 2 && route_temp.at(route_temp.size() - 1) == '/')
			route_temp.erase(route_temp.size() - 1, 1);
		std::string redirected_url = prepend_forward_slash(route_temp);
		redirected_url.append(prepend_forward_slash(_request_data.filename));
		return redirected_url;
	}
	return prepend_forward_slash(_request_data.full_path);
}

const bool configParser::getAutoIndex()
{
	RouteIterator route;
	bool result = false;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	if (route != getServer(_request_data.port)._routes.end() && !route->second._autoindex.empty())
		route->second._autoindex == "true" ? result = true : result = false;
	return result;
}

const std::string	configParser::getIndexFile()
{
	RouteIterator route;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	return route->second._index;
}

const bool configParser::getPostAllowed()
{
	RouteIterator route;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	if (route != getServer(_request_data.port)._routes.end())
	{
		if (hasMethod(route->second._methods, "POST"))
			return true;
	}
	return false;
}

const bool configParser::getDeleteAllowed()
{
	RouteIterator route;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	if (route != getServer(_request_data.port)._routes.end())
	{
		if (hasMethod(route->second._methods, "DELETE"))
			return true;
	}
	return false;
}

const bool configParser::getGetAllowed()
{
	RouteIterator route;
	route = getServer(_request_data.port)._routes.find(_request_data.route);
	if (route != getServer(_request_data.port)._routes.end())
	{
		if (hasMethod(route->second._methods, "GET"))
			return true;
	}
	return false;
}

const IntVector&	configParser::getPortVector() const
{
	return _unique_ports;
}

const IntStringMap&	configParser::getErrorMap()
{
	return getServer(_request_data.port)._error_map;
}

const int	configParser::get_timeout() const
{
	return _settings.timeout;
}

const int	configParser::get_max_clients() const
{
	return _settings.max_clients;
}

const int	configParser::get_body_size() const
{
	return _settings.body_size;
}

const int	configParser::get_max_events() const
{
	return _settings.max_events;
}

const int	configParser::get_backlog() const
{
	return _settings.backlog;
}






/**************************************************************************************************************/
// CORE
/**************************************************************************************************************/



Server&	configParser::getServer(const int port)
{
	ServersMap::iterator it = _servers.find(port);
	return it->second;
}

void configParser::parse_request_data()
{
	// set filename
	std::size_t pos_route = _request_data.full_path.find_last_of('/') + 1;
	std::size_t pos = _request_data.full_path.find(".");
	if (pos != std::string::npos) // if filename.empty() -> directory is requested
	{
		std::string temp =_request_data.full_path;
		std::string temp2 = temp.substr(pos_route, _request_data.full_path.size());
		_request_data.filename = temp2;
	}
	// set route
	std::string temp = _request_data.full_path;
	std::string route = temp.erase(pos_route, _request_data.full_path.size());
	if (route.size() > 1 && route.at(0) == '/')
		route.erase(0,1);
	if (route.size() > 2 && route.at(route.size() - 1) == '/')
		route.erase(route.size() - 1, 1);
	_request_data.route = route;
}

int	configParser::string_to_int(const std::string& str)
{
	std::istringstream stream(str);
	int number;
	if (stream >> number)
		return number;
	else
		throw std::invalid_argument("not a valid integer"); // TODO VF more precise handling
}

std::string configParser::getToken(const std::string& str, int n)
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

int configParser::countToken(const std::string& str)
{
	std::istringstream line(str);
	int count = 0;

	std::string token;
	while (line >> token)
		++count;

	return count;
}

int configParser::validate_directive_single(const std::string& str)
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

int	configParser::validate_directive_multi(const std::string& str)
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

void configParser::validate_minimal_server_configuration(Server& server)
{
	StringIntMap::iterator it;
	it = server._status.find(PORT);
	if (it == server._status.end())
		throw std::invalid_argument("missing port directive in this server block.");
	// {
	// 	// if (addStatus(server, PORT))
	// 	// 	server._port = string_to_int("8080"); // setting default value
	// 	// std::cerr << YELLOW << "Warning: port missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of 8080 is set." << RESET_COLOR << std::endl;
	// }

	it = server._status.find(HOST);
	if (it == server._status.end())
	{
		if (addStatus(server, HOST))
			server._host = "0.0.0.0"; // setting default value
		std::cerr << YELLOW << "Warning: host missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of 0.0.0.0 is set." << RESET_COLOR << std::endl;
	}

	it = server._status.find("body_size");
	if (it == server._status.end())
	{
		if (addStatus(server, "body_size"))
			server._body_size = BODY_SIZE; // setting default value
		std::cerr << YELLOW << "Warning: body_size missing on server " << server._server_nbr << " [" << server._server_line_nbr << "] -> default value of " << BODY_SIZE << " is set." << RESET_COLOR << std::endl;
	}
}

bool configParser::addStatus(Server& server, const std::string& str)
{
	std::pair<StringIntMap::iterator,bool> ret;
	ret = server._status.insert ( std::pair<std::string,int const>(str,_directive_line_nbr) );
	if (ret.second==false)
	{
    	std::cerr << YELLOW << "Warning: directive \"" << str << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		return 0;
	}
	return 1;
}

void configParser::addLocation(Server& server, const std::string& path)
{
	location newLocation;
	
	newLocation._path = path;

	std::pair<StringLocationMap::iterator,bool> ret;
	ret = server._routes.insert ( std::pair<std::string,location>(path,newLocation) );
	if (ret.second==false)
    	std::cerr << YELLOW << "Warning: location with: " << path << " already exist" << RESET_COLOR << std::endl;
}

void configParser::setGlobal()
{
	if ((getToken(_line, 1) == "timeout") && validate_directive_single(_line))
	{
		if (_settings_check.timeout)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set. skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.timeout = string_to_int(getToken(_line, 3));
			if (_settings.timeout < 60)
				std::cerr << YELLOW << "Warning: timeout set to \"" << _settings.timeout << "\" in line: " << _directive_line_nbr << " -> Values under 60 might lead to unstable up and download." << RESET_COLOR << std::endl;
			_settings_check.timeout = true;
		}
	}
	else if ((getToken(_line, 1) == "max_clients") && validate_directive_single(_line))
	{
		if (_settings_check.max_clients)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.max_clients = string_to_int(getToken(_line, 3));
			_settings_check.max_clients = true;
		}
	}
	else if ((getToken(_line, 1) == "body_size") && validate_directive_single(_line))
	{
		if (_settings_check.body_size)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			int size = string_to_int(getToken(_line, 3));
			if (size < BODY_SIZE_MIN || size > BODY_SIZE_MAX)
				std::cerr << YELLOW << "Warning: body_size on line: " << _directive_line_nbr << " is set to " << size << " -> recommended range is between 2000-1000000." << RESET_COLOR << std::endl;
			_settings.body_size = string_to_int(getToken(_line, 3));
			_settings_check.body_size = true;
		}
	}
	else if ((getToken(_line, 1) == "max_events") && validate_directive_single(_line))
	{
		if (_settings_check.max_events)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.max_events = string_to_int(getToken(_line, 3));
			_settings_check.max_events = true;
		}
	}
	else if ((getToken(_line, 1) == "backlog") && validate_directive_single(_line))
	{
		if (_settings_check.backlog)
			std::cerr << YELLOW << "Warning: directive \"" << getToken(_line, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.backlog = string_to_int(getToken(_line, 3));
			_settings_check.backlog = true;
		}
	}
}

void configParser::setDirective(Server& server, const std::string& _route)
{
	server._directive_line_nbr = _directive_line_nbr;
	// server
	if (getToken(_line, 1) == "[server]")
		throw std::invalid_argument("open serverblock");
	else if (getToken(_line, 1) == PORT && validate_directive_single(_line))
	{
		if (string_to_int(getToken(_line, 3)) < 1024)
			std::cerr << YELLOW << "Warning: port \"" << getToken(_line, 3) << "\" in line: " << _directive_line_nbr << " -> Ports under 1024 need extended permissions, binding might fail." << RESET_COLOR << std::endl;
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
	else if (getToken(_line, 1) == "body_size" && validate_directive_single(_line))
	{
		if (addStatus(server, "body_size"))
		{
			int size = string_to_int(getToken(_line, 3));
			if (size < BODY_SIZE_MIN || size > BODY_SIZE_MAX)
				std::cerr << YELLOW << "Warning: body_size on line: " << _directive_line_nbr << " is set to " << size << " -> recommended range is between 2000-1000000." << RESET_COLOR << std::endl;
			server._body_size = string_to_int(getToken(_line, 3));
		}
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
		std::cerr << YELLOW << "Warning: invalid key \"" << getToken(_line, 1) << "\" skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
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

void configParser::setErrorPage(Server& server, const std::string& str)
{
	/*
	- prepend "root/"
	- map error code to maries code-syntax if she has an "alias" for a regular code
	- key is error-code value is path, overwrite path with custom one
	- check file, if not existing then trow error
	- filter input >400
	*/
	std::pair<IntStringMap::iterator,bool> ret;
	int response_code = string_to_int(getToken(str, 2));
	std::string temp_path = getToken(str, 4);
	if (response_code < 400)
		throw std::invalid_argument("invalid error code");
	ret = server._error_map.insert ( std::pair<int,std::string>(response_code,temp_path));
	if ( ret.second == false)
	{
		std::string path = "root/";
		if (!temp_path.empty() && temp_path.c_str()[0] == '/')
			temp_path.erase(0,1);
		path.append(temp_path);
		std::ofstream file;
		file.open(path);
		if (!file)
			throw std::invalid_argument("invalid file");
		file.close();
		ret.first->second = path;
	}
	else
	{
		server._status.insert ( std::pair<std::string,int const>(ERROR_PAGE,_directive_line_nbr) );
	}
}

void configParser::setRoot(Server& server, const std::string& str, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._root.empty())
			it->second._root = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setMethods(Server& server, const std::string& str, const std::string& route)
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
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setAutoindex(Server& server, const std::string& str, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (getToken(str, 3) == "true" || getToken(str, 3) == "on" || getToken(str, 3) == "1")
			it->second._autoindex = "true";
		else if (getToken(str, 3) != "false" && getToken(str, 3) != "off" && getToken(str, 3) != "0")
			throw std::invalid_argument("invalid directive value");
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setIndex(Server& server, const std::string& str, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._index.empty())
			it->second._index = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setCGI(Server& server, const std::string& str, const std::string& route)
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
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setRedirect(Server& server, const std::string& str, const std::string& route)
{
	if (check_route_exist(server, route))
	{
		RouteIterator it = return_route(server, route);
		if (it->second._redirect.empty())
			it->second._redirect = getToken(str, 3);
		else
			std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << YELLOW << "Warning: directive \"" << getToken(str, 1) << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

// prepends '/' if not present
std::string configParser::prepend_forward_slash(const std::string str)
{
	std::string temp = str;
	if (!temp.empty() && temp.at(0) != '/')
		temp.insert(temp.begin(), '/');
	return temp;
}

bool configParser::check_route_exist(Server& server, const std::string& route)
{
	RouteIterator it;
	it = server._routes.find(route);
	if (it == server._routes.end())
		return false;
	return true;
}

RouteIterator	configParser::return_route(Server& server, const std::string& route)
{
	RouteIterator it = server._routes.find(route);
	return it;
}

RouteIterator	configParser::return_route()
{
	RouteIterator it = getServer(_request_data.port)._routes.find(_request_data.route);
	return it;
}

bool configParser::hasRoute(Server& server, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it == server._routes.end())
		return false;
	return true;
}

bool configParser::hasMethod(StringVector& methods, std::string method)
{

	for (size_t i = 0; i < methods.size(); ++i)
	{
		if (methods[i] == method)
			return true;
	}
	return false;
}

void	configParser::create_port_vector()
{
	IntSet::iterator it;
	for (it = _unique_ports_sorted.begin(); it != _unique_ports_sorted.end(); ++it)
		_unique_ports.push_back(*it);
}

void	configParser::create_default_error_map()
{
	_default_error_map.insert ( std::pair<int,std::string>(DEFAULTWEBPAGE, PATH_DEFAULTWEBPAGE) );
	_default_error_map.insert ( std::pair<int,std::string>(DIRECTORY_LIST, PATH_DIRECTORY_LIST) );
	_default_error_map.insert ( std::pair<int,std::string>(FILE_SAVED, PATH_FILE_SAVED) );
	_default_error_map.insert ( std::pair<int,std::string>(FILE_DELETED, PATH_FILE_DELETED) );
	_default_error_map.insert ( std::pair<int,std::string>(BAD_REQUEST, PATH_BAD_REQUEST) );
	_default_error_map.insert ( std::pair<int,std::string>(FORBIDDEN, PATH_FORBIDDEN) );
	_default_error_map.insert ( std::pair<int,std::string>(NOT_FOUND, PATH_404_ERRORWEBSITE) );
	_default_error_map.insert ( std::pair<int,std::string>(METHOD_NOT_ALLOWED, PATH_METHOD_NOT_ALLOWED) );
	_default_error_map.insert ( std::pair<int,std::string>(INTERNAL_ERROR, PATH_500_ERRORWEBSITE) );
}






/**************************************************************************************************************/
// PRINT & LOG
/**************************************************************************************************************/



void configParser::printServerDetails()
{
	std::cout << "\n" << BOLDWHITE << _servers_index.size() << " servers with " << _unique_ports_sorted.size() << " unique ports:" << RESET_COLOR << std::endl;
	int size = _servers_index.size();
	std::cout << std::endl;
	for (int i = 0; i < size; i++)
		printServer(_servers_index[i]);
}

void configParser::printServerDetails(std::ofstream& file)
{
	file << "\n" << _servers_index.size() << " Servers with " << _unique_ports_sorted.size() << " unique ports:\n" << std::endl;
	
	int size = _servers_index.size();
	for (int i = 0; i < size; i++)
		printServer(_servers_index[i], file);
}

void configParser::printGlobalSettings()
{
	std::cout << BOLDWHITE << "\nGLOBAL SETTINGS:" << RESET_COLOR << " timeout = " << _settings.timeout << ", max_clients = " << _settings.max_clients;
	std::cout << ", body_size = " << _settings.body_size << ", max_events = " << _settings.max_events;
	std::cout << ", backlog = " << _settings.backlog << std::endl;
}

void configParser::printGlobalSettings(std::ofstream& file)
{
	file << "\nGLOBAL SETTINGS" << "\n\ttimeout = " << _settings.timeout << "\n\tmax_clients =" << _settings.max_clients;
	file << "\n\tbody_size = " << _settings.body_size << "\n\tmax_events = " << _settings.max_events;
	file << "\n\tbacklog = " << _settings.backlog << std::endl;
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
	for (it = server._error_map.begin(); it != server._error_map.end(); ++it)
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
		if (!server._error_map.empty())
		{	
			IntStringMap::iterator it;
			for (it = server._error_map.begin(); it != server._error_map.end(); ++it)
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

void configParser::printLog()
{
	std::ofstream file;

	file.open("log/servers.log");
	if (!file)
	{
		std::cerr << YELLOW << "Could not create servers.log" << RESET_COLOR << std::endl;
		return ;
	}
	file << "Configuration file: \"" << _file_path << "\"" << std::endl;
	printGlobalSettings(file);
	printServerDetails(file);
	file.close();
}
