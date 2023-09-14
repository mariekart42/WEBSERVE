/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vfuhlenb <vfuhlenb@students.42wolfsburg    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 23:16:58 by vfuhlenb          #+#    #+#             */
/*   Updated: 2023/09/14 19:57:26 by vfuhlenb         ###   ########.fr       */
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
#define TIMEOUT 60
#define MAX_CLIENTS 100
#define BUFF_SIZE 8000
#define MAX_EVENTS 100
#define BACKLOG 10
#define DEFAULT_CONF "default.conf"

#define PORT "port"
#define HOST "host"
#define SERVER_NAME "server_name"
#define BODY_SIZE "body_size"
#define ERROR_PAGE "error_page"
#define ROOT "root"
#define METHODS "methods"
#define AUTOINDEX "autoindex"
#define INDEX "index"
#define CGI "cgi"
#define REDIRECT "redirect"

# define RESET		"\033[0m"
# define BLACK		"\033[30m"		/* Black */
# define RED		"\033[31m"		/* Red */
# define GREEN		"\033[32m"		/* Green */
# define YELLOW		"\033[33m"		/* Yellow */
# define BLUE		"\033[34m"		/* Blue */
# define MAGENTA	"\033[35m"		/* Magenta */
# define CYAN		"\033[36m"		/* Cyan */
# define WHITE		"\033[37m"		/* White */
# define BOLDBLACK		"\033[1m\033[30m"		/* Bold Black */
# define BOLDRED		"\033[1m\033[31m"		/* Bold Red */
# define BOLDGREEN		"\033[1m\033[32m"		/* Bold Green */
# define BOLDYELLOW		"\033[1m\033[33m"		/* Bold Yellow */
# define BOLDBLUE		"\033[1m\033[34m"		/* Bold Blue */
# define BOLDMAGENTA	"\033[1m\033[35m"		/* Bold Magenta */
# define BOLDCYAN		"\033[1m\033[36m"		/* Bold Cyan */
# define BOLDWHITE		"\033[1m\033[37m"		/* Bold White */

typedef std::vector<std::string> StringVector;
typedef std::map<int,std::string> IntStringMap;
typedef std::map<std::string,int> StringIntMap;

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

typedef std::multimap<int,Server> ServersMap;
typedef std::vector<Server> ServersIndex;

typedef struct global_settings {
	int	timeout;
	int	max_clients;
	int	buff_size;
	int	max_events;
	int	backlog;
}global_settings;

typedef struct settings_check {
	bool	timeout;
	bool	max_clients;
	bool	buff_size;
	bool	max_events;
	bool	backlog;
}settings_check;

class configParser {

	public:

		configParser(int argc, char **argv);
		~configParser();

		Server&				getServer(const int index); // returns Server Object from Vector[index]
		ServersMap&			getServerMap();
		int					getLineToServerBlock(const int index) const;
		int					getSize() const; // returns number of Server Objects
		int					getPort(const int index) const;
		std::string			getHost(const int index) const;
		int					getUniquePorts();
		int					getUniquePorts(const int index);
		int					getTimeout() const;
		int					getBacklog() const;
		int					getBuffSize() const;
		int					getMaxEvents() const;
		int					getMaxClients() const;

		// TODO VF getters for marie
		std::string			getUrl(const int currentPort, std::string url) const; // TODO: @Valentin always put / in the beginning of path, if redirection is set - then change the URL
		std::string			getRootFolder(const int currentPort) const;
		bool				getAutoIndex(const int currentPort) const;
		std::string			getIndexFile(const int currentPort) const;
		bool				getPostAllowed(const int currentPort) const;
		bool				getDeleteAllowed(const int currentPort) const;
		bool				getGetAllowed(const int currentPort) const;
		std::vector<int>&	getPortVector() const;

		// DEBUG

		void		printServerDetails();
		void		printServerDetails(std::ofstream&);
		void		printGlobalSettings();
		void		printGlobalSettings(std::ofstream&);
		void		printLog();

	private:

		configParser();
		bool			validConfig();
		void			setDirective(Server&, const std::string&);
		void			setGlobal();
		void			validate_minimal_server_configuration(Server& server);
		// Utils String
		int				string_to_int(const std::string&);
		std::string		getToken(std::string str, int n);
		int				countToken(std::string str);
		// Validation of input string for the directives
		int				validate_directive_single(std::string str);
		int				validate_directive_multi(std::string str);

		global_settings	_settings;
		settings_check	_settings_check;
		std::ifstream	_file;
		std::string		_file_path;
		std::string		_line;
		ServersMap		_servers; // port, server
		ServersIndex	_servers_index;
		int				_context; // TODO VF method to check in which context currently
		int				_directive_line_nbr;
		IntSet			_unique_ports;

		// methods from old Server class
		bool			addStatus(Server& server, const std::string& str);
		void			addLocation(Server& server, std::string path);
		void			printServer(Server& server);
		void			printServer(Server& server, std::ofstream& file);
		void			setServerName(Server& server, const std::string& str);
		void			setErrorPage(Server& server, const std::string & str);
		void			setRoot(Server& server, const std::string &str, const std::string &route);
		void			setMethods(Server& server, const std::string &str, const std::string &route);
		void			setAutoindex(Server& server, const std::string &str, const std::string &route);
		void			setIndex(Server& server, const std::string &str, const std::string &route);
		void			setCGI(Server& server, const std::string &str, const std::string &route);
		void			setRedirect(Server& server, const std::string &str, const std::string &route);
};

#endif
