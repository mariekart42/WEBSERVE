#include "utils.h"

void logg(const std::string &message)
{
	std::cout << message << std::endl;
}

void exitWithError(const std::string &msg)
{
    #ifdef LOG
		logg(RED"ERROR: " + msg + RESET);
    #endif
    std::cout << BOLDRED << msg << RESET << std::endl;
	exit(1);
}

int setNonBlocking(int fd)
{
    if (fcntl(fd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) == -1)
    {
		#ifdef LOG
			Logging::log("fcntl error, could not set flag to O_NONBLOCK", 500);
		#endif
        return -1;
	}
    return 0;
}

void printError()
{
	if (g_shutdown_flag == 0)
	{
		#ifdef INFO
				std::cout << RED << "Error: Poll function returned Error" << RESET << std::endl;
		#endif
		#ifdef LOG
				Logging::log("Error: Poll function returned Error", 500);
		#endif
	}
}

std::string myItoS(int val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

bool endsWith(const std::string& str, const std::string& end)
{
    if (str.length() >= end.length())
        return (str.compare(str.length() - end.length(), end.length(), end) == 0);
    else
        return false;
}

void signalHandler(int sigNum)
{
	if (sigNum == SIGINT || sigNum == SIGTERM)
	{
		std::cout << "\nReceived shutdown signal. Terminating webserv ..." << std::endl;
		g_shutdown_flag = 1;
	}
}

void mySignals()
{
	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
}


std::string generateList(const std::string& rootFolder, const std::string& currentFolder)
{
    std::string filePaths;

    std::string folderPath = rootFolder + "/" + currentFolder;
    DIR* dir = opendir(folderPath.c_str());

    if (dir)
    {
        struct dirent* entry;
        while ((entry = readdir(dir)) != 0) {
            std::string itemName = entry->d_name;

            if (itemName != "." && itemName != "..") {
                std::string itemPath = folderPath + "/" + itemName;
                struct stat itemStat;

                if (stat(itemPath.c_str(), &itemStat) == 0)
                {
                    if (S_ISDIR(itemStat.st_mode))
                    {
                        // Recurse into subfolder, passing the current folder
                        std::string subfolderPaths = generateList(rootFolder, currentFolder + "/" + itemName);
                        filePaths += subfolderPaths;
                    } else if (S_ISREG(itemStat.st_mode)) {
                        std::string linkPath = currentFolder + "/" + itemName;
                        filePaths += "\"" + linkPath + "\",";
                    }
                }
            }
        }
        closedir(dir);
    }
    return filePaths;
}

std::string comparerContentType(const std::string& fileExtension)
{
    static std::map<std::string, std::string> extensionToType;

    if (extensionToType.empty())
    {
        extensionToType.insert(std::make_pair("avif", "image/avif"));
        extensionToType.insert(std::make_pair("bmp", "image/bmp"));
        extensionToType.insert(std::make_pair("jpeg", "image/jpeg"));
        extensionToType.insert(std::make_pair("jpg", "image/jpeg"));
        extensionToType.insert(std::make_pair("gif", "image/gif"));
        extensionToType.insert(std::make_pair("ico", "image/vnd.microsoft.icon"));
        extensionToType.insert(std::make_pair("png", "image/png"));
        extensionToType.insert(std::make_pair("svg", "image/svg+xml"));
        extensionToType.insert(std::make_pair("tiff", "image/tiff"));
        extensionToType.insert(std::make_pair("tif", "image/tiff"));
        extensionToType.insert(std::make_pair("webp", "image/webp"));

        extensionToType.insert(std::make_pair("mp4", "video/mp4"));
        extensionToType.insert(std::make_pair("avi", "video/x-msvideo"));
        extensionToType.insert(std::make_pair("mpeg", "video/mpeg"));
        extensionToType.insert(std::make_pair("ogv", "video/ogg"));
        extensionToType.insert(std::make_pair("ts", "video/mp2t")); // cant even open (mac)
        extensionToType.insert(std::make_pair("webm", "video/webm"));
        extensionToType.insert(std::make_pair("3gp", "video/3gpp"));
        extensionToType.insert(std::make_pair("3g2", "video/3gpp2"));

        extensionToType.insert(std::make_pair("css", "text/css"));
        extensionToType.insert(std::make_pair("csv", "text/csv"));
        extensionToType.insert(std::make_pair("html", "text/html"));
        extensionToType.insert(std::make_pair("htm", "text/html"));
        extensionToType.insert(std::make_pair("ics", "text/calendar"));
        extensionToType.insert(std::make_pair("js", "text/javascript"));
        extensionToType.insert(std::make_pair("mjs", "text/javascript"));
        extensionToType.insert(std::make_pair("txt", "text/plain"));
        extensionToType.insert(std::make_pair("md", "text/plain"));

        extensionToType.insert(std::make_pair("abw", "application/x-abiword")); // dunno how to download
        extensionToType.insert(std::make_pair("arc", "application/x-freearc")); // dunno how to download
        extensionToType.insert(std::make_pair("azw", "application/vnd.amazon.ebook")); // dunno how to download
        extensionToType.insert(std::make_pair("bin", "application/octet-stream"));
        extensionToType.insert(std::make_pair("gz", "application/gzip"));
        extensionToType.insert(std::make_pair("json", "application/json"));
        extensionToType.insert(std::make_pair("pdf", "application/pdf"));
        extensionToType.insert(std::make_pair("doc", "application/msword"));
        extensionToType.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
        extensionToType.insert(std::make_pair("bz", "application/x-bzip"));
        extensionToType.insert(std::make_pair("bz2", "application/x-bzip2"));
        extensionToType.insert(std::make_pair("cda", "application/x-cdf"));
        extensionToType.insert(std::make_pair("csh", "application/x-csh"));
        extensionToType.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
        extensionToType.insert(std::make_pair("epub", "application/epub+zip"));
        extensionToType.insert(std::make_pair("jar", "application/java-archive"));
        extensionToType.insert(std::make_pair("jsonld", "application/ld+json"));
        extensionToType.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
        extensionToType.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
        extensionToType.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
        extensionToType.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
        extensionToType.insert(std::make_pair("ogx", "application/ogg"));
        extensionToType.insert(std::make_pair("php", "application/x-httpd-php"));
        extensionToType.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
        extensionToType.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
        extensionToType.insert(std::make_pair("rar", "application/vnd.rar"));
        extensionToType.insert(std::make_pair("rtf", "application/rtf"));
        extensionToType.insert(std::make_pair("sh", "application/x-sh"));
        extensionToType.insert(std::make_pair("tar", "application/x-tar"));
        extensionToType.insert(std::make_pair("vsd", "application/vnd.visio"));
        extensionToType.insert(std::make_pair("xhtml", "application/xhtml+xml"));
        extensionToType.insert(std::make_pair("xls", "application/vnd.ms-excel"));
        extensionToType.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
        extensionToType.insert(std::make_pair("xml", "application/xml"));
        extensionToType.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
        extensionToType.insert(std::make_pair("zip", "application/zip"));
        extensionToType.insert(std::make_pair("7z", "application/x-7z-compressed"));

        extensionToType.insert(std::make_pair("otf", "font/otf"));
        extensionToType.insert(std::make_pair("ttf", "font/ttf"));
        extensionToType.insert(std::make_pair("woff", "font/woff"));
        extensionToType.insert(std::make_pair("woff2", "font/woff2"));

        extensionToType.insert(std::make_pair("aac", "audio/aac"));
        extensionToType.insert(std::make_pair("mid", "audio/midi"));
        extensionToType.insert(std::make_pair("midi", "audio/midi"));
        extensionToType.insert(std::make_pair("mid", "audio/x-midi"));
        extensionToType.insert(std::make_pair("midi", "audio/x-midi"));
        extensionToType.insert(std::make_pair("mp3", "audio/mpeg"));
        extensionToType.insert(std::make_pair("oga", "audio/ogg"));
        extensionToType.insert(std::make_pair("opus", "audio/opus"));
        extensionToType.insert(std::make_pair("wav", "audio/wav"));
        extensionToType.insert(std::make_pair("weba", "audio/webm"));

		// CGI
        extensionToType.insert(std::make_pair("py", ".py"));
        extensionToType.insert(std::make_pair("pl", ".pl"));

    }

    std::map<std::string, std::string>::const_iterator it = extensionToType.find(fileExtension);
    if (it != extensionToType.end())
        return it->second;
    else
        return ("FAILURE");
}