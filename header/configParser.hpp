#ifndef CONFIGPARSER_CLASS_H
#define CONFIGPARSER_CLASS_H

#include "utils.h"

// Global settings
#define CGI_TIMEOUT		1500
#define TIMEOUT			0
#define MAX_CLIENTS		100
#define BODY_SIZE		9000
#define BODY_SIZE_MIN	2000
#define BODY_SIZE_MAX	10000000
#define MAX_EVENTS		100
#define BACKLOG			42
#define ROOT			"root"
#define INDEX			""
#define DEFAULT_CONF	"default.conf"

// Response codes
#define DEFAULTWEBPAGE		2001
#define DIRECTORY_LIST		2002
#define OK		            200
#define FILE_SAVED			201
#define FILE_DELETED		204
#define BAD_REQUEST			400
#define FORBIDDEN			403
#define NOT_FOUND			404
#define METHOD_NOT_ALLOWED	405
#define REQUEST_TOO_BIG		413
#define INTERNAL_ERROR		500
#define PATH_DEFAULTWEBPAGE		"root/index.html"
#define PATH_FILE_SAVED			"root/PATH_FILE_SAVED.html"
#define PATH_FILE_DELETED		"root/PATH_FILE_DELETED.html"
#define PATH_BAD_REQUEST		"error/400.html"
#define PATH_FORBIDDEN			"error/403.html"
#define PATH_404_ERRORWEBSITE	"error/404.html"
#define PATH_METHOD_NOT_ALLOWED	"error/405.html"
#define PATH_REQUEST_TOO_BIG	"error/413.html"
#define PATH_500_ERRORWEBSITE	"error/500.html"


typedef std::vector<std::string> StringVector;
typedef std::map<int,std::string> IntStringMap;
typedef std::map<std::string,int> StringIntMap;
typedef std::vector<int> IntVector;
typedef std::set<int> IntSet;


enum context
{
	GLOBAL,
	SERVER,
	LOCATION
};

typedef struct location
{
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
	IntStringMap		_error_map; // response-code : path
	StringLocationMap	_routes; // route : location_struct
	StringVector		_routes_vector; // unsorted

	// internal
	StringIntMap		_status; // directive-key : line-value
	int					_server_nbr;
	int					_server_line_nbr;
}Server;

typedef std::map<int,Server> ServersMap;
typedef std::vector<Server> ServersIndex;

typedef struct global_settings
{
	int	timeout;
	int	max_clients;
	int	body_size;
	int	max_events;
	int	backlog;
}global_settings;

typedef struct settings_check
{
	bool	timeout;
	bool	max_clients;
	bool	body_size;
	bool	max_events;
	bool	backlog;
}settings_check;

typedef struct RequestData
{
	std::string	_host;
	int			_port;
	std::string	_url;
	std::string	_filename;
}RequestData;

class configParser
{
	public:
		configParser();
		~configParser();

		bool				setData(const std::string& url, const std::string& host, int port);
		bool				validConfig(int argc, char **argv);

		// server specific
		const std::string	getUrl();
		bool				getAutoIndex();
		const std::string	getIndexFile(); // returns empty string if not set
		bool				getPostAllowed();
		bool				getDeleteAllowed();
		bool				getGetAllowed();
		StringVector&		getCgiExtensions();
		int					getBodySize(int incoming_port); // returns body-size from server with port
		IntVector&			getPortVector();
		IntStringMap&		getErrorMap();
		// global settings
		int			get_timeout() const;
		int			get_backlog() const;


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
		IntStringMap	_default_error_map;
		StringVector	_empty_string_vector;

		// temp
		std::string		_current_route;

		Server & getServer(int port);
		void			parse_request_data();
		int				string_to_int(const std::string&);
		std::string		getToken(const std::string& str, int n);
		int				countToken(const std::string& str);
		bool			validate_cgi(const std::string& str);
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
		std::string		prepend_forward_slash(const std::string str) const;
		bool			check_route_exist(Server& server, const std::string& route);
		RouteIterator	return_route(Server& server, const std::string& route);
		bool			hasMethod(StringVector& methods, std::string method) const;
		void			create_port_vector();
		void			create_default_error_map();
		void			check_path_traversal(const std::string path);
		bool			check_file(const std::string path);
		bool			RequestedLocationExist();
		std::string		remove_leading_character(const std::string str, char c);
		std::string		handle_redirection(const std::string route, Server& server);
		void			printServerDetails();
		void			printServerDetails(std::ofstream&);
		void			printGlobalSettings();
		void			printGlobalSettings(std::ofstream&);
		void			printServer(Server& server);
		void			printServer(Server& server, std::ofstream& file);
		void			printLog();
};

#endif
