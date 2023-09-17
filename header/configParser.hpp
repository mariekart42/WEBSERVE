/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfuhlenb <vfuhlenb@student.42wolfsburg.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 23:16:58 by vfuhlenb          #+#    #+#             */
/*   Updated: 2023/09/17 09:29:03 by vfuhlenb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_CLASS_H
#define CONFIGPARSER_CLASS_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>
#include <set>
#include <map>
#include <vector>

// GLOBAL SETTINGS
#define TIMEOUT			60
#define MAX_CLIENTS		100
#define BODY_SIZE		9000
#define BODY_SIZE_MIN	2000
#define BODY_SIZE_MAX	1000000
#define MAX_EVENTS		100
#define BACKLOG			10
#define DEFAULT_CONF	"../default.conf"
#define PORT			"port"
#define HOST			"host"
#define SERVER_NAME		"server_name"
#define ERROR_PAGE		"error_page"
#define ROOT			"root"
#define METHODS			"methods"
#define AUTOINDEX		"autoindex"
#define INDEX			"index"
#define CGI				"cgi"
#define REDIRECT		"redirect"

#define RESET		"\033[0m"
#define BLACK		"\033[30m"
#define RED			"\033[31m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define BLUE		"\033[34m"
#define MAGENTA		"\033[35m"
#define CYAN		"\033[36m"
#define WHITE		"\033[37m"
#define BOLDBLACK	"\033[1m\033[30m"
#define BOLDRED		"\033[1m\033[31m"
#define BOLDGREEN	"\033[1m\033[32m"
#define BOLDYELLOW	"\033[1m\033[33m"
#define BOLDBLUE	"\033[1m\033[34m"
#define BOLDMAGENTA	"\033[1m\033[35m"
#define BOLDCYAN	"\033[1m\033[36m"
#define BOLDWHITE	"\033[1m\033[37m"

typedef std::vector<std::string> StringVector;
typedef std::map<int,std::string> IntStringMap;
typedef std::map<std::string,int> StringIntMap;
typedef std::vector<int> IntVector;

typedef std::set<int> IntSet;

enum context {
	GLOBAL,
	SERVER,
	LOCATION
};

typedef struct location{

		std::string		_path;
		std::string		_root;
		StringVector	_methods;
		std::string		_autoindex;
		std::string		_index;
		StringVector	_cgi;
		std::string		_redirect;
		StringIntMap	_status;

}location;

typedef std::map<std::string,location> StringLocationMap;
typedef StringLocationMap::iterator RouteIterator;

typedef struct Server{
	int					_port;
	int					_body_size;
	std::string			_host;
	StringVector		_server_name;
	IntStringMap		_error_page;
	StringLocationMap	_routes; // path, location_struct

	// internal
	StringIntMap		_status; // directive-key and line-value
	int					_server_nbr;
	int					_server_line_nbr;
	int					_directive_line_nbr;
	int					_fd;
}Server;

typedef std::map<int,Server> ServersMap;
typedef std::vector<Server> ServersIndex;

typedef struct global_settings {
	int	timeout;
	int	max_clients;
	int	body_size;
	int	max_events;
	int	backlog;
}global_settings;

typedef struct settings_check {
	bool	timeout;
	bool	max_clients;
	bool	body_size;
	bool	max_events;
	bool	backlog;
}settings_check;

typedef struct RequestData {
	std::string	host;
	int			port;
	std::string	full_path;
	std::string	filename; // TODO VF needs to be the file+extension -> index.html
	std::string	route;
}RequestData;

class configParser {

	public:

		configParser();
		~configParser();

		/* setData
		- discuss with Marie if function should handle error 403
		- port is always valid and existing right?
		*/
		bool				setData(const std::string& url, const std::string& host,const int port);
		bool				validConfig(int argc, char **argv);
		std::string			getUrl(); // TODO VF do i expect only an absolute path starting with a forward slash? so without the protocol and possible domain name
		bool				getAutoIndex();
		std::string			getIndexFile(); // returns empty string if not set
		bool				getPostAllowed();
		bool				getDeleteAllowed();
		bool				getGetAllowed();
		IntVector&			getPortVector();
		// std::string			getRootFolder();


	private:

		int				_context;
		int				_directive_line_nbr;
		global_settings	_settings;
		settings_check	_settings_check;
		std::ifstream	_file;
		std::string		_file_path;
		std::string		_line;
		ServersMap		_servers;
		ServersIndex	_servers_index;
		IntSet			_unique_ports_sorted;
		IntVector		_unique_ports;
		RequestData		_request_data;

		Server&			getServer(const int port);
		void			parse_request_data();
		int				string_to_int(const std::string&);
		std::string		getToken(const std::string& str, int n);
		int				countToken(const std::string& str);
		int				validate_directive_single(const std::string& str);
		int				validate_directive_multi(const std::string& str);
		void			validate_minimal_server_configuration(Server& server);
		bool			addStatus(Server& server, const std::string& str);
		void			addLocation(Server& server, const std::string& path);
		void			setGlobal();
		void			setDirective(Server&, const std::string&);
		void			setServerName(Server& server, const std::string& str);
		void			setErrorPage(Server& server, const std::string& str);
		void			setRoot(Server& server, const std::string& str, const std::string &route);
		void			setMethods(Server& server, const std::string &str, const std::string &route);
		void			setAutoindex(Server& server, const std::string &str, const std::string &route);
		void			setIndex(Server& server, const std::string &str, const std::string &route);
		void			setCGI(Server& server, const std::string &str, const std::string &route);
		void			setRedirect(Server& server, const std::string &str, const std::string &route);
		std::string		prepend_forward_slash(const std::string str);
		bool			check_route_exist(Server& server, const std::string& route);
		RouteIterator	return_route(Server& server, const std::string& route);
		RouteIterator	return_route();
		bool			hasRoute(Server& server, const std::string& route);
		bool			hasMethod(StringVector& methods, std::string method);
		void			create_port_vector();
		void			printServerDetails();
		void			printServerDetails(std::ofstream&);
		void			printGlobalSettings();
		void			printGlobalSettings(std::ofstream&);
		void			printServer(Server& server);
		void			printServer(Server& server, std::ofstream& file);
		void			printLog();
};

#endif

/*
TODO`s

- convert uniquePorts set to int vector

validConfig()
- if configurations with same port is declared -> error √
- if body-size <2000 || >1000000 give warning, define these as macro √

processRequest()
- 





filename.html		just change filename
/filename.html		root + filename
path/filename.html	replace path with current route (location) and append filename
path				replace path with current route (location) and use 








Redirects and Location Headers: When an HTTP server sends a redirect response
(e.g., HTTP status code 301 or 302), it typically includes an absolute URL in
the "Location" header to specify the new location to which the client should navigate.

HTTP/1.1 301 Moved Permanently
Location: https://www.new-example.com/new-location



*/
