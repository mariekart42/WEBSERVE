#ifndef MARIE_CONFIG_PARSER_HPP
#define MARIE_CONFIG_PARSER_HPP


struct locationData
{
    int statusCode;

    std::string root;
    //- path where to locate the path specified in location to

    std::vector<std::string> index;
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


class ConfigParser
{
    private:
        bool _startServer;

        std::stirng _errorMsg;
        // in case of error, init appropriate error message that
        // gets printed to console in main.cpp

        std::vector<serverData> _serverData;
        //- for each server create one vector element
        //  and init the data in serverData



    public:
        ConfigParser();
        ~ConfigParser();


        std::vector<int> getPortVector();
        //- return vector of all Ports used in config file
        //- you need to check if all provided ports are different
        //--> if not write error message to _errorMsg && set _startServer to false

        bool startServer();
        //- returns bool value of _startServer;

        std::string getErrorMsg();
        //- return variable _errorMsg

};


#endif //WEBSERVERRR_MARIECONFIGPARSER_HPP
