#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

// because we need int, string and bool values -> so using a struct to
// make the extract config value with struct and map combined

class Configuration {
public:
    Configuration();

    bool loadFromFile(const std::string& filename);

    // Getter functions for configuration data
    int getServerPort() const;
    std::string getRootDirectory() const;
    std::string getServerHost() const;
    std::string getServerNames() const;
    std::string getPath500ErrorWebsite() const;
    std::string getPath404ErrorWebsite() const;
    std::string getPathForbidden() const;
    std::string getPathErrorIndexFile() const;
    bool getPathHandleFoldersLater() const;

private:

    struct ConfigKey {
        std::string key;
        void* value;
        enum class Type { INT, STRING, BOOL } type;
    };

    int serverPort;
    std::string rootDirectory;
    std::string serverHost;
    std::string serverNames;
    std::string path500ErrorWebsite;
    std::string path404ErrorWebsite;
    std::string pathForbidden;
    std::string pathErrorIndexFile;
    bool pathHandleFoldersLater;

    std::map<std::string, ConfigKey> configKeys;
    void extractConfigValue(const std::map<std::string, std::string>& configMap, const std::string& key) const;
};

#endif // CONFIG_HPP
