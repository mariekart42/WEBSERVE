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


std::string Response::getContentType()
{
//    return ("html");

    // find last dot in url
    // -> get file extension

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

        std::cout << GRN"File Extension: "<<fileExtension<<std::endl;
        if (fileExtension == "jpeg")
            return ("image/jpeg");
        if (fileExtension == "pdf")
            return ("application/pdf");
        if (fileExtension == "mp4")
            return ("video/mp4");
        if (fileExtension == "MOV")
            return ("video/quicktime");
        return fileExtension;   // will be wrong cause wrong prefix
    }
    std::cout << GRN"SHIT: "<<std::endl;
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