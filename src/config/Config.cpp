#include "Config.hpp"
// Had to initialize the map with the configuration keys in
// respect to member variables

Configuration::Configuration()
    : serverPort(8080),
      rootDirectory(""),
      serverHost(""),
      serverNames(""),
      path500ErrorWebsite(""),
      path404ErrorWebsite(""),
      pathForbidden(""),
      pathErrorIndexFile(""),
      pathHandleFoldersLater(false) {
    configKeys["Server.Port"] = { "Server.Port", &serverPort, ConfigKey::Type::INT };
    configKeys["Server.RootDirectory"] = { "Server.RootDirectory", &rootDirectory, ConfigKey::Type::STRING };
    configKeys["Server.Host"] = { "Server.Host", &serverHost, ConfigKey::Type::STRING };
    configKeys["Server.ServerNames"] = { "Server.ServerNames", &serverNames, ConfigKey::Type::STRING };
    configKeys["Server.Path500ErrorWebsite"] = { "Server.Path500ErrorWebsite", &path500ErrorWebsite, ConfigKey::Type::STRING };
    configKeys["Server.Path404ErrorWebsite"] = { "Server.Path404ErrorWebsite", &path404ErrorWebsite, ConfigKey::Type::STRING };
    configKeys["Server.PathForbidden"] = { "Server.PathForbidden", &pathForbidden, ConfigKey::Type::STRING };
    configKeys["Server.PathErrorIndexFile"] = { "Server.PathErrorIndexFile", &pathErrorIndexFile, ConfigKey::Type::STRING };
    configKeys["Server.PathHandleFoldersLater"] = { "Server.PathHandleFoldersLater", &pathHandleFoldersLater, ConfigKey::Type::BOOL };
      }

bool Configuration::loadFromFile(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) {
        std::cout << "Failed to open configuration file: " << filename << std::endl;
        file.close();
        return false;
    }

    std::map<std::string, std::string> configMap;
    std::string line;
    std::string currentSection;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Check if the line is a section header
        if (line[0] == '[' && line[line.size() - 1] == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }

        // Extract key-value pairs within a section
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, ' ') && std::getline(iss, value)) {
            std::string configKey = currentSection + "." + key;
            configMap[configKey] = value;
        }
    }

    file.close();

    // Extract relevant configuration values
    for (const auto& configKey : configKeys) {
        extractConfigValue(configMap, configKey.first);
    }

    return true;
}


void Configuration::extractConfigValue(const std::map<std::string, std::string>& configMap, const std::string& key) const {
    auto it = configMap.find(key);
    if (it != configMap.end()) {
        const ConfigKey& configKey = configKeys.at(key);
        std::string value = it->second;

        switch (configKey.type) {
            case ConfigKey::Type::INT:
                *static_cast<int*>(configKey.value) = std::stoi(value);
                break;
            case ConfigKey::Type::STRING:
                *static_cast<std::string*>(configKey.value) = value;
                break;
            case ConfigKey::Type::BOOL:
                *static_cast<bool*>(configKey.value) = (value == "true");
                break;
        }
    }
}


// Getter functions for configuration data
int Configuration::getServerPort() const {
    return serverPort;
}

std::string Configuration::getRootDirectory() const {
    return rootDirectory;
}

std::string Configuration::getServerHost() const {
    return serverHost;
}

std::string Configuration::getServerNames() const {
    return serverNames;
}

std::string Configuration::getPath500ErrorWebsite() const {
    return path500ErrorWebsite;
}

std::string Configuration::getPath404ErrorWebsite() const {
    return path404ErrorWebsite;
}

std::string Configuration::getPathForbidden() const {
    return pathForbidden;
}

std::string Configuration::getPathErrorIndexFile() const {
    return pathErrorIndexFile;
}

bool Configuration::getPathHandleFoldersLater() const {
    return pathHandleFoldersLater;
}




// Test Code for you Marie to load : 

// bool Response::loadConfigFromFile(const std::string& filename) {
//     return config.loadFromFile(filename);
// }