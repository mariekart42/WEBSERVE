
#include "../header/Request.hpp"
#include <vector>
Request::Request(const std::vector<uint8_t>& clientData):
    _tmp(std::string(clientData.begin(), clientData.end())), _statusCode()
{}

Request::~Request() {}


/* extracts last file of URL and identifies what content it contains
 *  eg. found  ".pdf"  ==>  returns  "application/pdf"          */
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

/* only for POST && multipart
 * checks for multipart and returs the difference between
 * Content-Length and size of input                     */
size_t Request::getBytesLeft(const std::string& contentType, const std::string& boundary)
{
    if (contentType.compare(0, 19, "multipart/form-data") == 0)
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

                // search for end of header
                size_t headerSize = _tmp.find("\r\n\r\n") + 4;


                return (requestLen - (_tmp.size() - headerSize));
            }
        }
        exitWithError("unexpected error: unable do get Content-Lenght");
        return -1;  // error
    }
    return 0;
}


/* only for POST && multipart
 * extracts the filename defined in the header of the body      */
std::string Request::getFileName(const std::string& contentType, const std::string& prevFileName)
{
    if (!prevFileName.empty() && prevFileName.compare(0, 13, "not_found_yet") != 0)
        return prevFileName;    // correct filename was already found
    // std::cout<<"multipart/form-data;"<<std::endl;


    // std::cout<<contentType<<std::endl;
    if (contentType.compare(0, 34, "multipart/form-data") == 0)
    {
        // std::cout<<"IN HEEERE"<<std::endl;
        size_t foundPos = _tmp.find("filename=\"");

        if (foundPos != std::string::npos)
        {
            size_t endPos = _tmp.find("\"\r\n", foundPos);
            if (endPos != std::string::npos)
            {
                std::string fileName = _tmp.substr(foundPos + 10, (endPos) - (foundPos + 10));

                std::cout << GRN"DEBUG: filename: " << fileName << ""RESET<< std::endl;
                return fileName;
            }
            std::cout << "DEBUG: no filename found in POST request" << std::endl;
            return (&"tmpFileForSocket_" [ random()]);
        }
        // exitWithError("unexpected error: unable do get filename");
        std::cout << "prolly first chunk of multipart, wait for filename"<<std::endl;
        return "not_found_yet";  // error
    }

    // WHAT IF POST REQUEST BUT NOT multipart/form-data?

    return FAILURE; // not failure but we don't consider filename if not POST
}




/* only for POST 
 * returns the value as string after Content-Type:      */
std::string Request::getContentType()
{
    size_t foundPos = _tmp.find("Content-Type: ");

    if (foundPos != std::string::npos)
    {
        size_t endPos = _tmp.find(";", foundPos);// CHanged from ; to \r
        if (endPos != std::string::npos)
        {
            std::string contentType = _tmp.substr(foundPos + 14, endPos - (foundPos + 14));

            std::cout << GRN"DEBUG: Content-Type: " << contentType << ""RESET<< std::endl;
            return contentType;
        }
    }
    return FAILURE;
}


/* only for POST && multipart
 * extracts boundary for multipart      */
std::string Request::getBoundary()
{
    size_t foundPos = _tmp.find("multipart/form-data; boundary=");

    if (foundPos != std::string::npos)
    {
        size_t endPos = _tmp.find("\r", foundPos);
        if (endPos != std::string::npos)
        {
            std::string contentType = _tmp.substr(foundPos + 30, endPos - (foundPos + 30));

            // std::cout << GRN"DEBUG: Boundary: " << contentType << RESET""<<std::endl;
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


    // MAKE URL LOWERCASE HERE
    if (endPos != std::string::npos)
        return (_tmp.substr(startPos, endPos - (startPos)));
    else
        return (_tmp.substr(startPos));
}


int Request::getStatusCode() const
{
    return _statusCode;
}




//std::ofstream *Request::getOutfile(std::string filename)
//{
//    if (filename.compare(0, 13, "not_found_yet") == 0)
//
//    return new std::ofstream (filename.c_str(), std::ofstream::out | std::ofstream::trunc);
//}





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