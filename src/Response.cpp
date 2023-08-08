#include <vector>
#include "../header/Response.hpp"

//serverResponse::serverResponse(int clientSocket /*Enum method, std::string url, char *body  */):
//    _statusCode(), _contentType(), _contentLength(), _respondFile()
//{
////    std::cout << "SERVER REQUEST CONSTRUCTOR" << std::endl;
//}




Response::Response(const Request &request, int clientSocket) :
        _HTTPMethod(request.getHTTPMethod()),
        _url(request.getURL()), _clientSocket(clientSocket),
        _statusCode(request.getStatusCode()),
        _body(request.getBody())
{
}

Response::~Response()
{
    delete _body;
}
Response::Response()
{
}


// TODO:
//      - get correct file extension => test requesting jpeg, txt, pdf etc


std::string Response::contentType(const std::string& fileExtension)
{
    if (fileExtension[0] == 'a') {
        if (fileExtension == "aac") return ("audio/aac");
        if (fileExtension == "abw") return ("application/x-abiword");
        if (fileExtension == "arc") return ("application/x-freearc");
        if (fileExtension == "avif") return ("image/avif");
        if (fileExtension == "avi") return ("video/x-msvideo");
        if (fileExtension == "azw") return ("application/vnd.amazon.ebook");}
    if (fileExtension[0] == 'b') {
        if (fileExtension == "bin") return ("application/octet-stream");
        if (fileExtension == "bmp") return ("image/bmp");
        if (fileExtension == "bz") return ("application/x-bzip");
        if (fileExtension == "bz2") return ("application/x-bzip2");}
    if (fileExtension[0] == 'c') {
        if (fileExtension == "cda") return ("application/x-cdf");
        if (fileExtension == "csh") return ("application/x-csh");
        if (fileExtension == "css") return ("text/css");
        if (fileExtension == "csv") return ("text/csv");}
    if (fileExtension[0] == 'd') {
        if (fileExtension == "doc") return ("application/msword");
        if (fileExtension == "docx") return ("application/vnd.openxmlformats-officedocument.wordprocessingml.document");}
    if (fileExtension[0] == 'e') {
        if (fileExtension == "eot") return ("application/vnd.ms-fontobject");
        if (fileExtension == "epub") return ("application/epub+zip");}
    if (fileExtension[0] == 'g') {
        if (fileExtension == "gz") return ("application/gzip");
        if (fileExtension == "gif") return ("image/gif");}
    if (fileExtension[0] == 'h') {
        if (fileExtension == "html") return ("text/html");
        if (fileExtension == "htm") return ("text/html");}
    if (fileExtension[0] == 'i') {
        if (fileExtension == "ico") return ("image/vnd.microsoft.icon");
        if (fileExtension == "ics") return ("text/calendar");}
    if (fileExtension[0] == 'j') {
        if (fileExtension == "jar") return ("application/java-archive");
        if (fileExtension == "jpeg") return ("image/jpeg");
        if (fileExtension == "jpg") return ("image/jpeg");
        if (fileExtension == "js") return ("text/javascript");
        if (fileExtension == "json") return ("application/json");
        if (fileExtension == "jsonld") return ("application/ld+json");}
    if (fileExtension[0] == 'm') {
        if (fileExtension == "mid") return ("audio/midi");
        if (fileExtension == "midi") return ("audio/midi");
        if (fileExtension == "mid") return ("audio/x-midi");
        if (fileExtension == "midi") return ("audio/x-midi");
        if (fileExtension == "mjs") return ("text/javascript");
        if (fileExtension == "mp3") return ("audio/mpeg");
        if (fileExtension == "mp4") return ("video/mp4");
        if (fileExtension == "mpeg") return ("video/mpeg");
        if (fileExtension == "mpkg") return ("application/vnd.apple.installer+xml");}
    if (fileExtension[0] == 'o') {
        if (fileExtension == "odp") return ("application/vnd.oasis.opendocument.presentation");
        if (fileExtension == "ods") return ("application/vnd.oasis.opendocument.spreadsheet");
        if (fileExtension == "odt") return ("application/vnd.oasis.opendocument.text");
        if (fileExtension == "oga") return ("audio/ogg");
        if (fileExtension == "ogv") return ("video/ogg");
        if (fileExtension == "ogx") return ("application/ogg");
        if (fileExtension == "opus") return ("audio/opus");
        if (fileExtension == "otf") return ("font/otf");}
    if (fileExtension[0] == 'p') {
        if (fileExtension == "png") return ("image/png");
        if (fileExtension == "pdf") return ("application/pdf");
        if (fileExtension == "php") return ("application/x-httpd-php");
        if (fileExtension == "ppt") return ("application/vnd.ms-powerpoint");
        if (fileExtension == "pptx") return ("application/vnd.openxmlformats-officedocument.presentationml.presentation");}
    if (fileExtension[0] == 'r') {
        if (fileExtension == "rar") return ("application/vnd.rar");
        if (fileExtension == "rtf") return ("application/rtf");}
    if (fileExtension[0] == 's') {
        if (fileExtension == "sh") return ("application/x-sh");
        if (fileExtension == "svg") return ("image/svg+xml");}
    if (fileExtension[0] == 't') {
        if (fileExtension == "tar") return ("application/x-tar");
        if (fileExtension == "tiff") return ("image/tiff");
        if (fileExtension == "tif") return ("image/tiff");
        if (fileExtension == "ts") return ("video/mp2t");
        if (fileExtension == "ttf") return ("font/ttf");
        if (fileExtension == "txt") return ("text/plain");}
    if (fileExtension[0] == 'v')
        if (fileExtension == "vsd") return ("application/vnd.visio");
    if (fileExtension[0] == 'w') {
        if (fileExtension == "wav") return ("audio/wav");
        if (fileExtension == "weba") return ("audio/webm");
        if (fileExtension == "webm") return ("video/webm");
        if (fileExtension == "webp") return ("image/webp");
        if (fileExtension == "woff") return ("font/woff");
        if (fileExtension == "woff2") return ("font/woff2");}
    if (fileExtension[0] == 'x') {
        if (fileExtension == "xhtml") return ("application/xhtml+xml");
        if (fileExtension == "xls") return ("application/vnd.ms-excel");
        if (fileExtension == "xlsx") return ("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
        if (fileExtension == "xml") return ("application/xml");
        if (fileExtension == "xul") return ("application/vnd.mozilla.xul+xml");}
    else {
        if (fileExtension == "zip") return ("application/zip");
        if (fileExtension == "3gp") return ("video/3gpp");
        if (fileExtension == "3g2") return ("video/3gpp2");
        if (fileExtension == "7z") return ("application/x-7z-compressed");}
    return static_cast<const char *>(nullptr);
}


std::string Response::getContentType()
{
    if (_url.find('.') != std::string::npos)
    {
        size_t startPos = _url.find_last_of('.');
        size_t endPos = _url.size();

        // from found till end next space:
        std::string fileExtension;

        if (endPos != std::string::npos)
            fileExtension = (_url.substr(startPos + 1, endPos - (startPos)));
        else
            fileExtension = (_url.substr(startPos));

        return (contentType(fileExtension));
    }
    std::cout << RED"ERROR: is File but can't detect file extension [EXIT]"RESET<<std::endl;
    exit(69);
    return "SHIT";
}




std::string getHeader(int statusCode, const std::string& contentType, int contentLength)
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(statusCode) + " " +
            ErrorResponse::getErrorMessage(statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+contentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(contentLength) + "\r\n\r\n";
    return header;
}

void Response::sendDefaultWebpage() {
//    char *file;

    _file = readFile(PATH_DEFAULTWEBSITE);
    if (_file == static_cast< std::vector<uint8_t> >(0))
    {
        _file = readFile(PATH_500_ERRORWEBSITE);
        std::string errorHeader = getHeader(500, "text/html", _file.size());
        send(_clientSocket, errorHeader.c_str(), errorHeader.size(), 0);
        send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
        std::cout << RED"ERROR: unexpected Error, path to defaultWebsite wrong or no defaultWebsite provided"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        return;
    }

    std::string header = getHeader(200, "text/html", _file.size());

//    std::cout << GRN"== DEBUG == Header default webpage:\n["RESET << header << GRN"]"RESET << std::endl;
//    std::cout << GRN"== DEBUG ==File size: should [724]  is ["RESET << strlen(file) << GRN"]"RESET << std::endl;

    send(_clientSocket, header.c_str(), header.size(), 0);
    send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
//    free(_file);
}



// TODO: INIT LATER IF GET IS DONE
void Response::POSTResponse() {std::cout << RED "POSTResponse not working now!"RESET<<std::endl;}
void Response::DELETEResponse() {std::cout << RED "DELETEResponse not working now!"RESET<<std::endl;}



void Response::GETResponse()
{
    if (_url == INDEX_PAGE)
    {
//        std::cout << GRN"== DEBUG ==  _url: should: [] ==> is: ["RESET <<_url<< GRN"]"RESET<< std::endl;
        std::cout << "DEFAULT WEBSITE" << std::endl;
        sendDefaultWebpage();
    }
    else
        sendRequestedFile();

//    send(_clientSocket, _header.c_str(), _header.size(), 0);
//    send(_clientSocket, _responseFile, strlen(_responseFile), 0);
}



void Response::sendResponse()
{
    switch (_HTTPMethod)
    {
        case M_GET:
            GETResponse();
            break;
        case M_POST:
            POSTResponse();
            break;
        case M_DELETE:
            DELETEResponse();
            break;
        default:    // SEND 500 ERROR
            exitWithError("unexpected Error: sendResponse can't identifies HTTPMethod [EXIT]");
    }
}


//int Response::getContentLen()
//{
//    // Calculate the file size by reading the entire file
//    char buffer[1024];
//    size_t fileSize = 0;
//
//    while (imageFile.read(buffer, sizeof(buffer))) {
//        fileSize += imageFile.gcount();
//    }
//
//    fileSize += imageFile.gcount(); // Add the remaining bytes
//}

void Response::sendRequestedFile()
{
    struct stat s = {};
    std::string header;
//    char *file = nullptr;

    if (stat((SITE_FOLDER + _url).c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            // it's a Folder  -  LATER
            _statusCode = 6969;

            _file = readFile(PATH_HANDLEFOLDERSLATER);
            header = getHeader(_statusCode, "text/html", _file.size());
            send(_clientSocket, header.c_str(), header.size(), 0);
            send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
            std::cout << RED"Cant handle Folders jet, do if config parser is done"RESET << std::endl;
        }
        else if (s.st_mode & S_IFREG)
        {
            // it's a file
            _file = readFile(SITE_FOLDER + _url);
            if (_file == static_cast< std::vector<uint8_t> >(0))   // if file doesn't exist
            {
                // error 404
                _statusCode = 404;

                _file = readFile(PATH_404_ERRORWEBSITE);
                header = getHeader(_statusCode, "text/html", _file.size());
                send(_clientSocket, header.c_str(), header.size(), 0);
                send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
                std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
//                free(errorFile);
                return;
            }


            header = getHeader(_statusCode, getContentType(), _file.size());
            send(_clientSocket, header.c_str(), header.size(), 0);
            send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0); //💀
        }
        else
        {
            // error 500
            _statusCode = 500;

            _file = readFile(PATH_500_ERRORWEBSITE);
            std::string errorHeader = getHeader(_statusCode, "text/html", _file.size());
            send(_clientSocket, errorHeader.c_str(), errorHeader.size(), 0);
            send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
    {
        // error 404
        _file = readFile(PATH_404_ERRORWEBSITE);
        header = getHeader(404, "text/html", _file.size());
        send(_clientSocket, header.c_str(), header.size(), 0);
        send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
        std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
    }
//    free(file);
}

std::vector<uint8_t> Response::readFile(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file)
    {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return static_cast<std::vector<uint8_t> >(0);
    }

    // Read the file content into a vector
    std::vector<uint8_t> content(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
    );

    return content;
}