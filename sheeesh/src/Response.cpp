#include "../header/Response.hpp"

//serverResponse::serverResponse(int clientSocket /*Enum method, std::string url, char *body  */):
//    _statusCode(), _contentType(), _contentLength(), _respondFile()
//{
////    std::cout << "SERVER REQUEST CONSTRUCTOR" << std::endl;
//}




Response::Response(const Request& request, int clientSocket):
    _HTTPMethod(request.getHTTPMethod()),
    _url(request.getURL()), _clientSocket(clientSocket),
    _statusCode(request.getStatusCode()),
    _body(request.getBody()),
    _responseFile()
{
//    ?? SOME BULLSHIT HERE LOL
// prolly in REQUEST with find function --->> test
//    switch (request.getHTTPMethod())
//    {
//        case GET:
//            _HTTPMethod = M_GET;
//            break;
//        case POST:
//            _HTTPMethod = M_POST;
//            break;
//        case DELETE:
//            _HTTPMethod = M_DELETE;
//            break;
//        default:
//            _HTTPMethod = M_error;
//    }
    _contentType = getContentType();    // NOW HC -> CHANGE LATER
    std::cout << "METHOD: " << _HTTPMethod << std::endl;
}

Response::~Response()
{
    delete _body;
}
Response::Response()
{
}


// TODO:
//      - get content(Type, Length, data) for Header!!!
//      - if GET:
//          > check for existence of Folder/File
//          > send requested File


std::string Response::getContentType()
{
    return ("html");
}




std::string getHeader(int statusCode, const std::string& contentType, int contentLength)
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(statusCode) + " " +
            ErrorResponse::getErrorMessage(statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: text/"+contentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(contentLength) + "\r\n\r\n";
    return header;
}

void Response::sendDefaultWebpage() const
{
    char *file;

    file = readFile(PATH_DEFAULTWEBSITE);
    if (file == nullptr)
    {
        char *errorFile = readFile(PATH_500_ERRORWEBSITE);
        std::string errorHeader = getHeader(500, "html", strlen(errorFile));
        send(_clientSocket, errorHeader.c_str(), errorHeader.size(), 0);
        send(_clientSocket, errorFile, strlen(errorFile), 0);
        std::cout << RED"ERROR: unexpected Error, path to defaultWebsite wrong or no defaultWebsite provided"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        return;
    }

    std::string header = getHeader(200, "html", strlen(file));

//    std::cout << GRN"== DEBUG == Header default webpage:\n["RESET << header << GRN"]"RESET << std::endl;
//    std::cout << GRN"== DEBUG ==File size: should [724]  is ["RESET << strlen(file) << GRN"]"RESET << std::endl;

    send(_clientSocket, header.c_str(), header.size(), 0);
    send(_clientSocket, file, strlen(file), 0);
    free(file);
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


void Response::sendRequestedFile()
{
    struct stat s = {};
    std::string header;
    char *file = nullptr;

    if (stat((SITE_FOLDER + _url).c_str(), &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            // it's a Folder  -  LATER
            file = readFile(PATH_HANDLEFOLDERSLATER);
            header = getHeader(6969, "html", strlen(file));
            send(_clientSocket, header.c_str(), header.size(), 0);
            send(_clientSocket, file, strlen(file), 0);
            std::cout << RED"Cant handle Folders jet, do if config parser is done"RESET << std::endl;
        }
        else if (s.st_mode & S_IFREG)
        {
            // it's a file
            file = readFile(SITE_FOLDER + _url);
            if (!file)   // if file doesn't exist
            {
                // error 404
                char *errorFile = readFile(PATH_404_ERRORWEBSITE);
                header = getHeader(404, "html", strlen(errorFile));
                send(_clientSocket, header.c_str(), header.size(), 0);
                send(_clientSocket, errorFile, strlen(errorFile), 0);
                std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
                free(errorFile);
                return;
            }

            header = getHeader(200, getContentType(), strlen(file));
            send(_clientSocket, header.c_str(), header.size(), 0);
            send(_clientSocket, file, strlen(file), 0);
        }
        else
        {
            // error 500
            file = readFile(PATH_500_ERRORWEBSITE);
            std::string errorHeader = getHeader(500, "html", strlen(file));
            send(_clientSocket, errorHeader.c_str(), errorHeader.size(), 0);
            send(_clientSocket, file, strlen(file), 0);
            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
    {
        // error 404
        file = readFile(PATH_404_ERRORWEBSITE);
        header = getHeader(404, "html", strlen(file));
        send(_clientSocket, header.c_str(), header.size(), 0);
        send(_clientSocket, file, strlen(file), 0);
        std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
    }
    free(file);
}