/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfuhlenb <vfuhlenb@students.42wolfsburg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 23:17:00 by vfuhlenb          #+#    #+#             */
/*   Updated: 2023/10/03 14:37:21 by vfuhlenb         ###   ########.fr       */
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
	_request_data._port = 0;

	create_default_error_map();
	// setting settings_check struct members to false
	for (bool* p = &_settings_check.timeout; p <= &_settings_check.backlog; p++)
		*p = false;
}

configParser::~configParser() {}

bool	configParser::setData(const std::string& url, const std::string& host, const int port)
{
	_request_data._url = url;
	_request_data._url.empty() ? _request_data._url = "/" : _request_data._url = _request_data._url;
	_request_data._host = host;
	_request_data._port = port;
	parse_request_data();
	return true;
}

bool configParser::validConfig(int argc, char **argv)
{
	argc < 2 ? _file_path = DEFAULT_CONF : _file_path = argv[1];

	_file.open(_file_path.c_str());
	try
	{
		if (!_file)
			throw std::invalid_argument("invalid configuration file");
		int count = 0;
		while (getline(_file, _line))
		{
			_directive_line_nbr++;
			if (_line.empty())
				continue ;
			if (getToken(_line, 1) == "[server]")
			{
				count++;
				_context = SERVER;
			}
			if (_context == SERVER)
			{
				Server server;
				server._server_nbr = count;
				server._server_line_nbr = _directive_line_nbr;
				server._error_map = _default_error_map;
				server._body_size = _settings.body_size;
				std::string route;
				std::string route_end;
				std::string line_first_token;
				while (getline(_file, _line) && getToken(_line, 1) != "[\\server]")
				{
					_directive_line_nbr++;
					if (_line.empty())
						continue ;
					line_first_token = getToken(_line, 1);
					if (_context == LOCATION && line_first_token.at(0) == '<' && line_first_token.at(1) != '\\')
						throw std::invalid_argument("nested location not allowed");
					if (line_first_token.at(0) == '<' && line_first_token.size() < 3)
						throw std::invalid_argument("location can`t be empty");
					if (line_first_token.at(0) == '<' && line_first_token.at(1) != '\\')
					{
						check_path_traversal(line_first_token);
						route = line_first_token;
						route.erase(0,1);
						route.erase(route.size()-1,1);
						addLocation(server, route);
						_context = LOCATION;
						route_end = route;
						route_end.insert(0, "<\\").append(">");
					}
					else if (line_first_token == route_end)
						_context = SERVER;
					else if (!_line.empty())
					{
						setDirective(server, route);
					}
				}
				_directive_line_nbr++;
				validate_minimal_server_configuration(server);
				_unique_ports_sorted.insert(server._port);
				
				std::pair<ServersMap::iterator,bool> ret;
				ret = _servers.insert ( std::pair<int,Server>(server._port,server));
				if (!ret.second)
					throw std::invalid_argument("multiple configuration with same port not allowed");
				else
				{
					_servers_index.push_back(server);
					_context = GLOBAL;
				}
			}
			setGlobal();
		}
		if (!_servers_index.size())
			throw std::runtime_error("no server configuration declared");
		_file.close();
	}
	catch (std::exception &e)
	{
		std::cerr << BOLDRED << "Error: in \"" << _file_path << "\" on line " << _directive_line_nbr << " : " << e.what() << " [EXIT]" << RESET_COLOR << std::endl;
		_file.close();
		return false;
	}
	create_port_vector();
		std::cout << BOLDGREEN << "\nInfo: webserv running using configuration \"" << _file_path << "\"" << RESET_COLOR << std::endl;
		printGlobalSettings();
		printServerDetails();
		printLog();
	return true;
}

// iterate through locations and check exact route for example </> or </index.html>
// if requested url is "/index.html", the first location would already be a match. if </> is not defined, then </index.html> would match.
const std::string	configParser::getUrl()
{

	bool HasMatch = false;
	bool IsRedirect = false;

	HasMatch = RequestedLocationExist();

	// check if route has redirect directive
	if (HasMatch && !return_route(getServer(_request_data._port), _current_route)->second._redirect.empty())
		IsRedirect = true;

	// return Url and make sure it starts with an "/"
	if (IsRedirect)
		return prepend_forward_slash(handle_redirection(_current_route, getServer(_request_data._port)));
	return prepend_forward_slash(_request_data._url);
}

bool configParser::getAutoIndex()
{
	bool result = true;
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		route->second._autoindex == "true" ? result = true : result = false;
	}
	return result;
}

const std::string	configParser::getIndexFile()
{
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		if (route != getServer(_request_data._port)._routes.end())
    		return route->second._index;
	}
	return INDEX;
}

bool configParser::getPostAllowed()
{
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		if (route != getServer(_request_data._port)._routes.end())
		{
			if (hasMethod(route->second._methods, "POST"))
				return true;
			return false;
		}
	}
	return true;
}

bool configParser::getDeleteAllowed()
{
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		if (route != getServer(_request_data._port)._routes.end())
		{
			if (hasMethod(route->second._methods, "DELETE"))
				return true;
			return false;
		}
	}
	return true;
}

bool configParser::getGetAllowed()
{
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		if (route != getServer(_request_data._port)._routes.end())
		{
			if (hasMethod(route->second._methods, "GET"))
				return true;
			return false;
		}
	}
	return true;
}

StringVector&	configParser::getCgiExtensions()
{
	if (RequestedLocationExist())
	{
		RouteIterator route = return_route(getServer(_request_data._port), _current_route);
		if (route != getServer(_request_data._port)._routes.end())
			return route->second._cgi;
	}
	return _empty_string_vector;
}

int configParser::getBodySize(int incoming_port)
{
	return getServer(incoming_port)._body_size;
}

IntVector&	configParser::getPortVector()
{
	return _unique_ports;
}

IntStringMap&	configParser::getErrorMap()
{
	return getServer(_request_data._port)._error_map;
}

int	configParser::get_timeout() const
{
	return _settings.timeout;
}

int	configParser::get_backlog() const
{
	return _settings.backlog;
}

const std::string& configParser::getCurrentRoute() const
{
	return _current_route;
}




/**************************************************************************************************************/
// CORE
/**************************************************************************************************************/



Server & configParser::getServer(int port)
{
	ServersMap::iterator it = _servers.find(port);
	return it->second;
}

void configParser::parse_request_data()
{
	bool IsFile = false;
	std::size_t PosFile;

	// check if url contains a file
	std::size_t PosFileDel = _request_data._url.find(".");
	PosFileDel != std::string::npos ? IsFile = true : IsFile = false;
	std::size_t PosLastSlash = _request_data._url.find_last_of('/');
	PosLastSlash != std::string::npos ? PosFile = PosLastSlash + 1 : PosFile = 0;

	if (IsFile)
	{
		_request_data._filename = _request_data._url;
		_request_data._filename.substr(PosFile, _request_data._url.size());
	}
}

int	configParser::string_to_int(const std::string& str)
{
	std::istringstream stream(str);
	int number;
	if (stream >> number)
		return number;
	else
		throw std::invalid_argument("not a valid integer");
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

bool configParser::validate_cgi(const std::string& str)
{
	if (str == ".py" || str == ".pl")
		return true;
	return false;
}

int configParser::validate_directive_single(const std::string& str)
{
	std::string line_first_token = getToken(str, 1);
	if (countToken(str) < 3)
		throw std::invalid_argument("invalid directive syntax");
	else if (line_first_token == "error_page" && getToken(str, 3) != "=")
		throw std::invalid_argument("invalid directive-delimiter");
	else if (line_first_token == "error_page" && getToken(str, 4).c_str()[0] == '#')
		throw std::invalid_argument("invalid directive-value");
	else if (line_first_token != "error_page" && getToken(str, 2) != "=")
		throw std::invalid_argument("invalid directive-delimiter");
	else if (line_first_token != "error_page" && getToken(str, 3).c_str()[0] == '#')
		throw std::invalid_argument("invalid directive-value");
	else if (line_first_token != "error_page" && countToken(str) > 3 && getToken(str, 4).c_str()[0] != '#')
		throw std::invalid_argument("invalid directive syntax");
	else if (countToken(str) > 4 && line_first_token == "error_page" && countToken(str) > 3 && getToken(str, 5).c_str()[0] != '#')
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
	it = server._status.find("port");
	if (it == server._status.end())
		throw std::invalid_argument("missing port directive in this server block");

	it = server._status.find("host");
	if (it == server._status.end())
	{
		if (addStatus(server, "host"))
			server._host = "0.0.0.0"; // setting default value
		std::cerr << BLUE << "Warning: host missing on server " << BOLDRED << server._server_nbr << BLUE << " [" << server._server_line_nbr << "] -> default value of " << BOLDGREEN <<  "0.0.0.0" << BLUE << " has been set" << RESET_COLOR << std::endl;
	}

	it = server._status.find("body_size");
	if (it == server._status.end())
	{
		if (addStatus(server, "body_size"))
			server._body_size = BODY_SIZE; // setting default value
		std::cerr << BLUE << "Warning: body_size missing on server " << BOLDRED << server._server_nbr << BLUE << " [" << server._server_line_nbr << "] -> default value of " << BOLDGREEN << BODY_SIZE << BLUE << " has been set" << RESET_COLOR << std::endl;
	}
}

bool configParser::addStatus(Server& server, const std::string& str)
{
	std::pair<StringIntMap::iterator,bool> ret;
	ret = server._status.insert ( std::pair<std::string,int const>(str,_directive_line_nbr) );
	if (ret.second==false)
	{
    	std::cerr << BLUE << "Warning: directive \"" << BOLDRED << str << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
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
	if (!ret.second)
    	std::cerr << BLUE << "Warning: location with: " << path << " already exist" << RESET_COLOR << std::endl;
	else
		server._routes_vector.push_back(path);
}

void configParser::setGlobal()
{
	std::string line_first_token = getToken(_line, 1);
	if ((line_first_token == "timeout") && validate_directive_single(_line))
	{
		if (_settings_check.timeout)
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << line_first_token << BLUE << "\" already set. skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.timeout = string_to_int(getToken(_line, 3));
			if (_settings.timeout >= 1 && _settings.timeout < 60)
				std::cerr << BLUE << "Warning: timeout set to \"" << BOLDRED << _settings.timeout << BLUE << "\" in line: " << _directive_line_nbr << " -> Values under 60 might lead to unstable up and download" << RESET_COLOR << std::endl;
			_settings_check.timeout = true;
		}
	}
	else if ((line_first_token == "max_clients") && validate_directive_single(_line))
	{
		if (_settings_check.max_clients)
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << line_first_token << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.max_clients = string_to_int(getToken(_line, 3));
			_settings_check.max_clients = true;
		}
	}
	else if ((line_first_token == "body_size") && validate_directive_single(_line))
	{
		if (_settings_check.body_size)
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << line_first_token << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.body_size = string_to_int(getToken(_line, 3));
			_settings_check.body_size = true;
		}
	}
	else if ((line_first_token == "max_events") && validate_directive_single(_line))
	{
		if (_settings_check.max_events)
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << line_first_token << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.max_events = string_to_int(getToken(_line, 3));
			_settings_check.max_events = true;
		}
	}
	else if ((line_first_token == "backlog") && validate_directive_single(_line))
	{
		if (_settings_check.backlog)
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << line_first_token << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
		else
		{
			_settings.backlog = string_to_int(getToken(_line, 3));
			_settings_check.backlog = true;
		}
	}
}

void configParser::setDirective(Server& server, const std::string& _route)
{
	std::string line_first_token = getToken(_line, 1);
	// server
	if (line_first_token == "[server]")
		throw std::invalid_argument("open serverblock");
	else if (line_first_token == "port" && validate_directive_single(_line))
	{
		int port = string_to_int(getToken(_line, 3));
		if (port < 0 || port > 65535)
			throw std::invalid_argument("port outside of valid range");
		if (port < 1024)
			std::cerr << BLUE << "Warning: port \"" << BOLDRED << getToken(_line, 3) << BLUE << "\" in line: " << _directive_line_nbr << " -> Ports under 1024 need extended permissions, binding might fail" << RESET_COLOR << std::endl;
		if (addStatus(server, "port"))
			server._port = port;
	}
	else if (line_first_token == "host" && validate_directive_single(_line))
	{
		if (addStatus(server, "host"))
			server._host = getToken(_line, 3);
	}
	else if (line_first_token == "server_name" && validate_directive_multi(_line))
		setServerName(server, _line);
	else if (line_first_token == "body_size" && validate_directive_single(_line))
	{
		if (addStatus(server, "body_size"))
		{
			int size = string_to_int(getToken(_line, 3));
			server._body_size = size;
			if (size < BODY_SIZE_MIN || size > BODY_SIZE_MAX)
				std::cerr << BLUE << "Warning: body_size on line: " << _directive_line_nbr << " is set to " << BOLDRED << size << BLUE << " -> recommended range is between 2000-10000000 (2kb - 10MB)." << RESET_COLOR << std::endl;
		}
	}
	else if (line_first_token == "error_page" && validate_directive_single(_line))
		setErrorPage(server, _line);
	// server.location
	else if (line_first_token == "root" && validate_directive_single(_line))
		setRoot(server, _line, _route);
	else if (line_first_token == "methods" && validate_directive_multi(_line))
		setMethods(server, _line, _route);
	else if (line_first_token == "autoindex" && validate_directive_single(_line))
		setAutoindex(server, _line, _route);
	else if (line_first_token == "index" && validate_directive_single(_line))
		setIndex(server, _line, _route);
	else if (line_first_token == "cgi" && validate_directive_multi(_line))
		setCGI(server, _line, _route);
	else if (line_first_token == "redirect" && validate_directive_single(_line))
		setRedirect(server, _line, _route);
	else if (line_first_token != "[\\server]" && line_first_token != "#" && line_first_token != ";")
		std::cerr << BLUE << "Warning: invalid key \"" << BOLDRED << line_first_token << BLUE <<  "\" skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setServerName(Server& server, const std::string& str)
{
	int count = countToken(str);
	int i = 3;
	server._status.insert ( std::pair<std::string,int const>("server_name",_directive_line_nbr) );
	while (i <= count && getToken(str, i) != "#")
	{
		server._server_name.push_back(getToken(str, i));
		i++;
	}
}

void configParser::setErrorPage(Server& server, const std::string& str)
{
	check_path_traversal(str);
	int response_code = string_to_int(getToken(str, 2));
	if (response_code < 400)
		throw std::invalid_argument("invalid error code");
	std::string path = getToken(str, 4);
	std::pair<IntStringMap::iterator,bool> ret;
	ret = server._error_map.insert ( std::pair<int,std::string>(response_code,path));
	if (!ret.second)
	{
		std::string new_path = ROOT;
		path = remove_leading_character(path, '/');
		new_path.append("/");
		new_path.append(path);
		check_file(new_path);
		ret.first->second = new_path;
	}
	else
		server._status.insert ( std::pair<std::string,int const>("error_page",_directive_line_nbr) );
}

void configParser::setRoot(Server& server, const std::string& str, const std::string& route)
{
	check_path_traversal(str);
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._root.empty())
			it->second._root = getToken(str, 3);
		else
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setMethods(Server& server, const std::string& str, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		int count = countToken(str);
		int i = 3;
		server._status.insert ( std::pair<std::string,int const>("methods",_directive_line_nbr) );
		while (i <= count && getToken(str, i).c_str()[0] != '#')
		{
			it->second._methods.push_back(getToken(str, i));
			i++;
		}
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
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
		else
			it->second._autoindex = "false";
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setIndex(Server& server, const std::string& str, const std::string& route)
{
	check_path_traversal(str);
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		if (it->second._index.empty())
			it->second._index = getToken(str, 3);
		else
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setCGI(Server& server, const std::string& str, const std::string& route)
{
	StringLocationMap::iterator it;
	it = server._routes.find(route);
	if (it != server._routes.end())
	{
		int count = countToken(str);
		int i = 3;
		server._status.insert ( std::pair<std::string,int const>("cgi",_directive_line_nbr) );
		while (i <= count && getToken(str, i).c_str()[0] != '#')
		{
			if (validate_cgi(getToken(str, i)))
				it->second._cgi.push_back(getToken(str, i));
			i++;
		}
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

void configParser::setRedirect(Server& server, const std::string& str, const std::string& route)
{
	check_path_traversal(str);
	if (check_route_exist(server, route))
	{
		RouteIterator it = return_route(server, route);
		if (it->second._redirect.empty())
			it->second._redirect = getToken(str, 3);
		else
			std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" already set, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
	}
	else
		std::cerr << BLUE << "Warning: directive \"" << BOLDRED << getToken(str, 1) << BLUE << "\" needs to be in locationblock, skipping line: " << _directive_line_nbr << RESET_COLOR << std::endl;
}

// prepends '/' if not present
std::string configParser::prepend_forward_slash(const std::string str) const
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

// for config parsing
RouteIterator	configParser::return_route(Server& server, const std::string& route)
{
	RouteIterator it = server._routes.find(route);
	return it;
}

bool configParser::hasMethod(StringVector& methods, std::string method) const
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
	_default_error_map.insert ( std::pair<int,std::string>(FILE_SAVED, PATH_FILE_SAVED) );
	_default_error_map.insert ( std::pair<int,std::string>(FILE_DELETED, PATH_FILE_DELETED) );
	_default_error_map.insert ( std::pair<int,std::string>(BAD_REQUEST, PATH_BAD_REQUEST) );
	_default_error_map.insert ( std::pair<int,std::string>(FORBIDDEN, PATH_FORBIDDEN) );
	_default_error_map.insert ( std::pair<int,std::string>(NOT_FOUND, PATH_404_ERRORWEBSITE) );
	_default_error_map.insert ( std::pair<int,std::string>(METHOD_NOT_ALLOWED, PATH_METHOD_NOT_ALLOWED) );
	_default_error_map.insert ( std::pair<int,std::string>(REQUEST_TOO_BIG, PATH_REQUEST_TOO_BIG) );
	_default_error_map.insert ( std::pair<int,std::string>(INTERNAL_ERROR, PATH_500_ERRORWEBSITE) );
	_default_error_map.insert ( std::pair<int,std::string>(GATEWAY_TIMEOUT, PATH_GATEWAY_TIMEOUT) );
}

void configParser::check_path_traversal(const std::string path)
{
	std::size_t f1 = path.find("../");
	std::size_t f2 = path.find("/../");
	if (f1 != std::string::npos || f2 != std::string::npos)
    	throw std::invalid_argument("path traversal not allowed.");
}

bool configParser::check_file(const std::string& path)
{
	std::ifstream file;
	file.open(path.c_str());
	if (!file)
	{
		file.close();
		throw std::invalid_argument("invalid file");
		return false;
	}
	file.close();
	return true;
}

bool configParser::RequestedLocationExist()
{
	bool HasMatch = false;

	// get Server based on requested Port
	Server& server = getServer(_request_data._port);

	// check if a route of the server matches against the requested URL (from the beginning of the string)
	StringVector::iterator route;
	for (route = server._routes_vector.begin(); route != server._routes_vector.end(); ++route)
	{
		_current_route = *route;

		// special case </> -> return true
		size_t pos = _request_data._url.find('/');
		if (_request_data._url == "/" && _current_route == "/")
		{
			HasMatch = true;
			break ;
		}
		if (_request_data._url.find('/', pos + 1) == std::string::npos && _current_route == "/" && !_request_data._filename.empty())
		{
			HasMatch = true;
			break ;
		}

		// find _current_route in url, if not found -> continue to next route
		if (_current_route.size() > 1 && _request_data._url.find(_current_route.c_str(), 0, _current_route.size()) == std::string::npos)
			continue ;
		// check for exact match without trailing characters in found url-block
		if (_current_route.size() > 1 && (_request_data._url.size() == _current_route.size() \
			|| (_current_route.at(_current_route.size() - 1) != '/' && _request_data._url.at(_current_route.size()) == '/') \
			|| _current_route.at(_current_route.size() - 1) == '/' ))
		{
			HasMatch = true;
			break ;
		}
	}
	if (!HasMatch)
		_current_route = "";
	return HasMatch;
}

std::string configParser::remove_leading_character(const std::string& str, char c)
{
	std::string new_str = str;
	if (!new_str.empty() && new_str.c_str()[0] == c)
		new_str.erase(0,1);
	return new_str;
}

std::string configParser::handle_redirection(const std::string route, Server& server)
{
	std::string redirected_url = _request_data._url;
	redirected_url.erase(0,route.size());
	redirected_url = remove_leading_character(redirected_url, '/');
	redirected_url = prepend_forward_slash(redirected_url);
	redirected_url.insert(0, return_route(server, route)->second._redirect);
	#ifdef DEBUG
		std::cout << RED << "REDIRECTED URL " << redirected_url << RESET << std::endl;
	#endif
	return redirected_url;
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
	file << "\nGLOBAL SETTINGS" << "\n\ttimeout = " << _settings.timeout << "\n\tmax_clients = " << _settings.max_clients;
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
		<< "\nSERVER " << server._server_nbr << " [" << server._server_line_nbr << "]"
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
			file << "\n\t<\\" << it->second._path << ">";
		}
		file << std::endl;
	}
}

void configParser::printLog()
{
	std::ofstream file;

	file.open("log/servers.log");
	if (!file)
	{
		std::cerr << BLUE << "Could not create servers.log" << RESET_COLOR << std::endl;
		return ;
	}
	file << "Configuration file: \"" << _file_path << "\"" << std::endl;
	printGlobalSettings(file);
	printServerDetails(file);
	file.close();
}
