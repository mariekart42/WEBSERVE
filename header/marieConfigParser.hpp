#ifndef MARIE_CONFIG_PARSER_HPP
#define MARIE_CONFIG_PARSER_HPP

#include "utils.h"

// DON'T USE!! it's for me for testing
#define PORT1 1010
#define PORT2 2020
#define PORT3 3030
#define PORT4 5050

#define CLIENT_BODY 8000
#define ROOT_FOLDER "root"
#define AUTO_INDEX true
#define INDEX_FILE "index.html"
#define POST_ALLOWED true
#define GET_ALLOWED true
#define DELETE_ALLOWED true
// -----------------------------------


struct locationData
{
    int statusCode;

    std::string root;
    //- path where to locate the path specified in location to

    std::vector<std::string> _indexFiles;
    //- provide all index files as strings in vector
    //- eg. index.html index.php ...

    bool allowGet;
    bool allowPost;
    bool allowDelete;
    //- parse "allow_methods" and set bools to true or false

    bool autoindex;
    //- on/off -> true/false

};


struct serverData
{
        int port;
        //- digits after "listen"

        std::string serverName;
        //-  string after "server_name" ONLY ONE!
        //-  IF no servername provided set _serverName to localhost

        int clientBodySize;
        //- if not provided: set to 8.000
        //- if smaller than 1.000 or bigger than 1.000.000 display error on console && set _error to true



        std::map<std::string, locationData> location;
        //- for each location element in config, create map
        //- key is path of location
        //- value is struct locationData

        // location["images"] = location.root;


};


class MarieConfigParser
{
    private:
        bool _startServer;

        std::string _errorMsg;
        // in case of error, init appropriate error message that
        // gets printed to console in main.cpp

        std::vector<serverData> _serverData;
        //- for each server create one vector element
        //  and init the data in serverData



    public:
        MarieConfigParser();
        ~MarieConfigParser();


        std::vector<int> getPortVector();
        //- return vector of all Ports used in config file
        //- you need to check if all provided ports are different
        //--> if not write error message to _errorMsg && set _startServer to false

        bool startServer();
        //- returns bool value of _startServer;

        std::string getErrorMsg();
        //- return variable _errorMsg


        std::string getUrl(int port, std::string url);
        //- search in _serverData vector for the element where port is equal to my provided port
        //- compare location paths with start of my provided url (url usually has more sub-folders than location path)
        // ! locations can have paths with more than one folder
        //- change first element of location path with path that is specified in locations root path
        //- eg you receive url: /change/path/00.png
        //- a location in server is specified as:
        //      location /change/path
        //      {
        //          root this/is/a/new
        //      }
        //-> if the exact path "/change/path" exist in my provided url,
        //   return url: this/is/a/new/path/00.png
        // ! only "/change" gets exchanged with locations root path, not the sub-folders after the first pathname

        std::string getRootFolder(int port);
        //- similar to getUrl, search server with provided port
        //- return in there defined root folder
        //- this syntax "root/folder/"

        int getClientBodysize(int port);
        //- return client bodysize

        bool getAutoIndex(int port);
        //- return _autoindex true/false

        std::string getIndexFile(int port);
        //- return first element from vector _indexFiles
        //- if no index file provided in configfile
        //--> return empty string!


        bool getPostAllowed(int port);
        bool getGetAllowed(int port);
        bool getDeleteAllowed(int port);

};


#endif //WEBSERVERRR_MARIECONFIGPARSER_HPP
