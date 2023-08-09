#include "../header/Response.hpp"

Response::Response(const Request &request, int clientSocket) :
        _HTTPMethod(request.getHTTPMethod()),
        _url(request.getURL()), _clientSocket(clientSocket),
        _body(request.getBody())
{}

Response::~Response() {}
//Response::Response() {}






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
        std::string contentType = comparerContentType(fileExtension);
        if (contentType == "FAILURE")
            mySend(404);
        return (contentType);
    }


    std::cout << RED"ERROR: is File but can't detect file extension"RESET<<std::endl;
    return FAILURE;
}


// if statusCode 200, _file NEEDS so be initialized!!
void Response::mySend(int statusCode)
{
    if (statusCode != 200)     // WRITE FUNCTION THAT RETURNS FILE SPECIFIED ON STATUS CODE
    {
        _contentType = "text/html";

        if (statusCode == FILE_SAVED)
        {
            statusCode = 200;
            _file = readFile(PATH_FILE_SAVED);
        }
        else if (statusCode == FILE_NOT_SAVED)
        {
            statusCode = 500;
            _file = readFile(PATH_FILE_NOT_SAVED);
        }
        else if (statusCode == FILE_ALREADY_EXISTS)
        {
            statusCode = 409;
            _file = readFile(PATH_FILE_ALREADY_EXISTS);
        }
        else if (statusCode == DEFAULTWEBPAGE)
            _file = readFile(PATH_DEFAULTWEBSITE);
        else if (statusCode == 500)
            _file = readFile(PATH_500_ERRORWEBSITE);
        else if (statusCode == 404)
        {
            _file = readFile(PATH_404_ERRORWEBSITE);
            std::cout << RED"ERROR: 404 File not found"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
        else if (statusCode == 6969)
            _file = readFile(PATH_HANDLEFOLDERSLATER);
        else
            exitWithError("status code not defined in mySend() [EXIT]");
    }
    else
    {
        // can't find file extension
        _contentType = getContentType();
        if (_contentType == FAILURE)
            mySend(404);
    }
    std::string header = getHeader(statusCode);

    std::cout << "Response Hedaer:\n"GRN<<header<<""RESET<<std::endl;
    std::cout << "Response Body:\n"GRN<<(_file.data())<<""RESET<<std::endl;

    send(_clientSocket, header.c_str(), header.size(), 0);
//    send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
    send(_clientSocket, static_cast<const void*>(_file.data()), _file.size(), 0);
}


std::string Response::getHeader(int statusCode)
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(statusCode) + " " +
            ErrorResponse::getErrorMessage(statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_contentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(_file.size()) + "\r\n\r\n";
    return header;
}




// TODO: INIT LATER IF GET IS DONE
void Response::POSTResponse()
{
    std::cout << RED "POST Response!"RESET<<std::endl;

//    saveFile();
//_file = readFile(PATH_500_ERRORWEBSITE);
    saveRequestToFile();
//    mySend(500);
}



void Response::DELETEResponse() {std::cout << RED "DELETEResponse not working now!"RESET<<std::endl;}



void Response::sendResponse()
{
    switch (_HTTPMethod)
    {
        case M_GET:
            sendRequestedFile();
            break;
        case M_POST:
            POSTResponse();
            break;
        case M_DELETE:
            DELETEResponse();
            break;
        default:
            mySend(500);
            exitWithError("unexpected Error: sendResponse can't identifies HTTPMethod [EXIT]");
    }
}

void Response::sendDefaultWebpage()
{
    _file = readFile(PATH_DEFAULTWEBSITE);
    if (_file.empty())
    {
        mySend(500);
        std::cout << RED"ERROR: unexpected Error, path to defaultWebsite wrong or no defaultWebsite provided"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        return;
    }
    mySend(DEFAULTWEBPAGE);
}


void Response::sendRequestedFile()
{
    if (_url == INDEX_PAGE)
        return (sendDefaultWebpage());

    struct stat s = {};
    int statusCode = OK;

    if (stat((SITE_FOLDER + _url).c_str(), &s) == 0)
    {
        if (IS_FOLDER)  //-> LATER if config is parsed
        {
            statusCode=6969;
            exitWithError("Cant handle Folders jet, do if config parser is done");
        }
        else if (IS_FILE)
        {
            _file = readFile(SITE_FOLDER + _url);
            if (_file.empty())   // if file doesn't exist
                statusCode = 404;
        }
        else
        {
            statusCode = 500;
            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
        statusCode = 404;


    mySend(statusCode);
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



void Response::saveRequestToFile()
{
    std::string filename = "nnnew.jpeg";

    if (fileExistsInDirectory(filename) == true)
    {
        std::cout << RED"FILE ALREADY EXISTS" RESET<< std::endl;
        mySend(FILE_ALREADY_EXISTS);
        return;
    }
    std::ofstream outputFile(UPLOAD_FOLDER+filename , std::ios::binary);
    if (outputFile)
    {
        outputFile.write(reinterpret_cast<const char*>(_body.data()), _body.size());
        outputFile.close();
        std::cout << "Request bytes saved to file: " << UPLOAD_FOLDER+filename << std::endl;
        mySend(FILE_SAVED);
    }
    else
    {
        std::cout << RED"Failed to open or write to the file." RESET<< std::endl;
        mySend(FILE_NOT_SAVED);
    }
}


bool Response::fileExistsInDirectory(std::string filename)
{


    DIR* dir = opendir(UPLOAD_FOLDER);
    if (dir == nullptr) {
        std::cerr << "Error opening directory: " << strerror(errno) << std::endl;
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, filename.c_str()) == 0) {
            closedir(dir);
            return true;
        }
    }

    closedir(dir);
    return false;
}