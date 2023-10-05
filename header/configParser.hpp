#ifndef CONFIGPARSER_CLASS_H
#define CONFIGPARSER_CLASS_H

#include "utils.h"

// Global settings
#define CGI_TIMEOUT		1000
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
#define GATEWAY_TIMEOUT	    504
#define PATH_DEFAULTWEBPAGE		"root/index.html"
#define PATH_FILE_SAVED			"root/PATH_FILE_SAVED.html"
#define PATH_FILE_DELETED		"root/PATH_FILE_DELETED.html"
#define PATH_BAD_REQUEST		"error/400.html"
#define PATH_FORBIDDEN			"error/403.html"
#define PATH_404_ERRORWEBSITE	"error/404.html"
#define PATH_METHOD_NOT_ALLOWED	"error/405.html"
#define PATH_REQUEST_TOO_BIG	"error/413.html"
#define PATH_500_ERRORWEBSITE	"error/500.html"
#define PATH_GATEWAY_TIMEOUT	"error/504.html"


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
		StringVector	_cgi;
		std::string		_path;
		std::string		_root;
		std::string		_index;
		StringIntMap	_status;
		StringVector	_methods;
		std::string		_redirect;
		std::string		_autoindex;

}location;

typedef std::map<std::string,location> StringLocationMap;
typedef StringLocationMap::iterator RouteIterator;

typedef struct Server{
	int					_port;
	int					_body_size;
	std::string			_host;
	StringVector		_server_name;
	IntStringMap		_error_map; // response-code : path
	StringVector		_routes_vector; // unsorted
	StringLocationMap	_routes; // route : location_struct

	// internal
	int					_server_nbr;
	int					_server_line_nbr;
	StringIntMap		_status; // directive-key : line-value
}Server;

typedef std::map<int,Server> ServersMap;
typedef std::vector<Server> ServersIndex;

typedef struct global_settings
{
	int	timeout;
	int	backlog;
	int	body_size;
	int	max_events;
	int	max_clients;
}global_settings;

typedef struct settings_check
{
	bool	timeout;
	bool	backlog;
	bool	body_size;
	bool	max_events;
	bool	max_clients;
}settings_check;

typedef struct RequestData
{
	int			_port;
	std::string	_host;
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
		bool				getAutoIndex();
		bool				getPostAllowed();
		bool				getDeleteAllowed();
		bool				getGetAllowed();
		int					getBodySize(int incoming_port); // returns body-size from server with port
		IntVector&			getPortVector();
		std::string	        getUrl();
		std::string	        getIndexFile(); // returns empty string if not set
		IntStringMap&		getErrorMap();
		StringVector&		getCgiExtensions();
		const std::string&	getCurrentRoute() const; // necessary for debug

		// global settings
		int			get_timeout() const;
		int			get_backlog() const;


	private:
		int				_directive_line_nbr;
		int				_context;
		IntSet			_unique_ports_sorted;
		IntVector		_unique_ports;
		ServersMap		_servers;
		std::string		_line;
		std::string		_file_path;
		RequestData		_request_data;
		ServersIndex	_servers_index;
		IntStringMap	_default_error_map;
		StringVector	_empty_string_vector;
		std::ifstream	_file;
		settings_check	_settings_check;
		global_settings	_settings;

		// temp
		std::string		_current_route;

		void			        parse_request_data();
		void			        validate_minimal_server_configuration(Server&);
		bool			        addStatus(Server&, const std::string&);
		void			        addLocation(Server&, const std::string&);
		void			        setGlobal();
		void			        setDirective(Server&, const std::string&);
		void			        setServerName(Server&, const std::string&) const;
		void			        setErrorPage(Server&, const std::string&);
		void			        setRoot(Server&, const std::string&, const std::string&);
		void			        setMethods(Server&, const std::string&, const std::string&) const;
		void			        setAutoindex(Server&, const std::string&, const std::string&) const;
		void			        setIndex(Server&, const std::string&, const std::string&);
		void			        setCGI(Server&, const std::string&, const std::string&) const;
		void			        setRedirect(Server&, const std::string&, const std::string&);
		bool			        check_route_exist(Server&, const std::string&);
		void			        create_port_vector();
		void			        create_default_error_map();
		bool			        RequestedLocationExist();
		void			        printServerDetails();
		void			        printServerDetails(std::ofstream&);
		void			        printGlobalSettings() const;
		void			        printLog();
		void			        printGlobalSettings(std::ofstream&) const;
		Server&                 getServer(int);
		static int				string_to_int(const std::string&);
		static int				countToken(const std::string&);
		static int				validate_directive_single(const std::string&);
		static int				validate_directive_multi(const std::string&);
		static bool			    validate_cgi(const std::string&);
		static bool			    hasMethod(StringVector&, const std::string&) ;
		static bool			    check_file(const std::string&);
		static void			    check_path_traversal(const std::string&);
		static void			    printServer(Server&);
		static void			    printServer(Server&, std::ofstream&);
		std::string		        handle_redirection(std::string, Server&) const;
		static std::string		getToken(const std::string&, int);
		static std::string		prepend_forward_slash(const std::string&) ;
		static std::string	    remove_leading_character(const std::string&, char c);
		static RouteIterator	return_route(Server&, const std::string&);
};

#endif
