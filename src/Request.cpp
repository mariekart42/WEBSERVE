
#include "../header/Request.hpp"
#include <vector>
Request::Request(const std::vector<uint8_t>& clientData):
    _tmp(std::string(clientData.begin(), clientData.end())), _statusCode()
{}

Request::~Request() {}


std::string Request::getFileContentType(const std::string& url)
{
    if (url.find('.') != std::string::npos)
    {
        size_t startPos = url.find_last_of('.');
        size_t endPos = url.size();

        // from found till end next space:
        std::string fileExtension;

        if (endPos != std::string::npos)
            fileExtension = (url.substr(startPos + 1, endPos - (startPos)));
        else
            fileExtension = (url.substr(startPos));
        std::string contentType = comparerContentType(fileExtension);
        if (contentType == "FAILURE")
            return FAILURE;
        return (contentType);
    }

    std::cout << RED"ERROR: is File but can't detect file extension"RESET<<std::endl;
    return FAILURE;
}

// CHECK IF RIGHT
size_t Request::getBytesLeft(const std::string& contentType)
{
    if (contentType.compare(0, 19, "multipart/form-data;") == 0)
    {
        size_t foundPos = _tmp.find("Content-Length: ");

        if (foundPos != std::string::npos)
        {
            size_t endPos = _tmp.find("\r\n", foundPos);
            if (endPos != std::string::npos)
            {
                std::string requestLenStr = _tmp.substr(foundPos + 15, endPos - foundPos - 15);
                size_t requestLen = static_cast<size_t>(std::strtol(requestLenStr.c_str(), nullptr, 10));

                std::cout << GRN"DEBUG: Content-Length: " << requestLen << ""RESET<< std::endl;
                return requestLen - _tmp.size();
            }
        }
        exitWithError("unexpected error: unable do get Content-Lenght");
        return -1;  // error
    }
    return 0;
}

// CHECK IF RIGHT
std::string Request::getFileName(const std::string& contentType, const std::string& prevFileName)
{
    if (!prevFileName.empty() && prevFileName.compare(0, 16, "tmpFileForSocket_") != 0)
        return prevFileName;    // correct filename was already found
    // std::cout<<"multipart/form-data;"<<std::endl;

    // std::string tmp = "multipart/form-data; boundary=----";


    // std::cout << "Content Type: " << contentType << std::endl;
    // std::cout << "Hexadecimal ASCII values of characters in the content type:" << std::endl;
    // for (std::string::size_type i = 0; i < contentType.length(); ++i) {
    //     char c = contentType[i];
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    // }
    // std::cout << std::endl;


    // std::cout << "Content Type: " << tmp << std::endl;
    // std::cout << "Hexadecimal ASCII values of characters in the content type:" << std::endl;
    // for (std::string::size_type i = 0; i < tmp.length(); ++i) {
    //     char c = tmp[i];
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    // }
    // std::cout << std::endl;




    std::cout<<contentType<<std::endl;
    if (contentType.compare(0, 34, "multipart/form-data; boundary=----") == 0)
    {
        std::cout<<"IN HEEERE"<<std::endl;
        size_t foundPos = _tmp.find("filename=");

        if (foundPos != std::string::npos)
        {
            size_t endPos = _tmp.find("\r\n", foundPos);
            if (endPos != std::string::npos)
            {
                std::string fileName = _tmp.substr(foundPos + 10, endPos - foundPos + 10);

                std::cout << GRN"DEBUG: filename: " << fileName << ""RESET<< std::endl;
                return fileName;
            }
            std::cout << "DEBUG: no filename found in POST request" << std::endl;
            return (&"tmpFileForSocket_" [ random()]);
        }
        // exitWithError("unexpected error: unable do get filename");
        std::cout << "prolly first chunk of multipart, wait for filename"<<std::endl;
        return FAILURE;  // error
    }

    // WHAT IF POST REQUEST BUT NOT multipart/form-data?

    return FAILURE; // not failure but we don't consider filename if not POST
}





std::string Request::getContentType()
{
    size_t foundPos = _tmp.find("Content-Type: ");

    if (foundPos != std::string::npos)
    {
        size_t endPos = _tmp.find("\n", foundPos);
        if (endPos != std::string::npos)
        {
            std::string contentType = _tmp.substr(foundPos + 14, endPos - (foundPos + 14));

            std::cout << GRN"DEBUG: Content-Type: " << contentType << ""RESET<< std::endl;
            return contentType;
        }
    }
    return FAILURE;
}


httpMethod Request::getHTTPMethod()
{
    if (_tmp.compare(0, 3, "GET") == 0)
        return M_GET;
    else if (_tmp.compare(0, 4, "POST") == 0)
        return M_POST;
    else if (_tmp.compare(0, 6, "DELETE") == 0)
        return M_DELETE;
    else
        return M_error;
}


std::string Request::getURL()
{
    size_t startPos = _tmp.find('/', 0) + 1;
    size_t endPos = _tmp.find(' ', startPos);

    if (endPos != std::string::npos)
        return (_tmp.substr(startPos, endPos - (startPos)));
    else
        return (_tmp.substr(startPos));
}


int Request::getStatusCode() const
{
    return _statusCode;
}

// std::vector<uint8_t> Request::getBody() const
// {
//    std::vector<uint8_t> bodyVector;

//    size_t startPos = _tmp.find("\r\n\r\n") + 4;
//    size_t endPos = _tmp.size();

//    if (endPos != std::string::npos)
//        bodyVector.insert(bodyVector.end(), _tmp.begin() + startPos, _tmp.begin() + endPos);
//    else
//        bodyVector.insert(bodyVector.end(), _tmp.begin() + startPos, _tmp.end());

//    // Print the contents of the vector (numeric values)
//    for (size_t i = 0; i < bodyVector.size(); ++i) {
//        std::cout <<GRN ""<< static_cast<int>(bodyVector[i]) << " "RESET;
//    }

//    return bodyVector;
// }

//std::vector<uint8_t> Request::getFile() const
//{
//    return (_clientData);
//}