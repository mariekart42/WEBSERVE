#include "../header/Response.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

Response::Response(const std::vector<uint8_t>& input, int clientSocket, const std::string& url)
{
    _info._postInfo._input = input;
    _info._clientSocket = clientSocket;
    _info._statusCode = 200;
    _info._url = url;

}

Response::~Response()
{
}

void Response::deleteFile()
{

std::cout << "file to deleet: " << _info._url << std::endl;
    if (_info._url == FAILURE)
        mySend(FILE_DELETED_FAIL);

    if (std::remove((UPLOAD_FOLDER + _info._url).c_str()) != 0)// changed UPLOAD_FOLDER to ROOT_FOLDER
        std::cout << "Error deleting the file." << std::endl;
    else
        std::cout << "File deleted successfully." << std::endl;
    std::cout << "wanna delete something" << std::endl;
    mySend(FILE_DELETED);

}



std::string Response::generateList(const std::string& rootFolder, const std::string& currentFolder = "")
{
    std::string filePaths;

    std::string folderPath = rootFolder + "/" + currentFolder;
    DIR* dir = opendir(folderPath.c_str());

    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            std::string itemName = entry->d_name;

            if (itemName != "." && itemName != "..") {
                std::string itemPath = folderPath + "/" + itemName;
                struct stat itemStat;

                if (stat(itemPath.c_str(), &itemStat) == 0) {
                    if (S_ISDIR(itemStat.st_mode)) {
                        // Recurse into subfolder
                        std::string subfolderPaths = generateList(rootFolder, currentFolder + "/" + itemName);
                        filePaths += subfolderPaths;
                    } else if (S_ISREG(itemStat.st_mode)) {
                        std::string linkPath = currentFolder.empty() ? itemName : currentFolder.substr(1) + "/" + itemName;
                        filePaths += "\""+linkPath+"\",";
                    }
                }
            }
        }
        closedir(dir);
    }

    return filePaths;
}





int Response::getDirectoryIndexPage()
{

//    std::string start69 = "<!DOCTYPE html>"
//                          "<html lang=\"en\">"
//                          "<head>"
//                          "    <meta charset=\"UTF-8\">"
//                          "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
//                          "    <title>Index of /</title>"
//                          "    <style>"
//                          "        .delete-button {\n"
//                          "            background-color: red;\n"
//                          "            color: white;\n"
//                          "            border: none;\n"
//                          "            padding: 5px 10px;\n"
//                          "            cursor: pointer;\n"
//                          "        }\n"
//                          "        \n"
//                          "        /* Add space between file path and delete button */\n"
//                          "        .spacer {\n"
//                          "            margin-left: 10px; /* Adjust spacing as needed */\n"
//                          "        }"
//                          "    </style>"
//                          "</head>"
//                          "<body>"
//                          "    <div class=\"container\">"
//                          "        <h1>Index of /</h1>"
//                          "        <div id=\"fileItems\"></div>"
//                          "    </div>"
//                          "    <script>"
//                          "        const hostname = window.location.hostname;"
//                          "        const port = window.location.port;"
//                          "        const filePaths = [";
//    std::string middle69 = generateList(ROOT_FOLDER, std::basic_string<char>());
//    std::string end69 = "\"u/can't/delete/me\""
//                        "        ];"
//                        "        const fileItemsContainer = document.getElementById(\"fileItems\");"
//                        "        filePaths.forEach(filePath => {"
//                        "            const fileItem = document.createElement(\"div\");"
//                        "            fileItem.className = \"item folder\";"
//                        "            const fileLink = document.createElement(\"a\");"
//                        "            fileLink.href = filePath;"
//                        "            fileLink.textContent = filePath;"
//                        "            const deleteButton = document.createElement(\"button\");"
//                        "            deleteButton.className = \"delete-button\";"
//                        "            deleteButton.textContent = \"Delete\";"
//                        "            deleteButton.addEventListener(\"click\", function() {"
//                        "                confirmDelete(filePath);"
//                        "            });"
//                        "            const resultMessage = document.createElement(\"p\");"
//                        "            resultMessage.className = \"result-message\";"
//                        "            fileItem.appendChild(fileLink);"
//                        "fileItem.appendChild(spacer); // Add spacer"
//                        "            fileItem.appendChild(deleteButton);"
//                        "            fileItem.appendChild(resultMessage);"
//                        "            fileItemsContainer.appendChild(fileItem);"
//                        "        });"
//                        "        function confirmDelete(filePath) {"
//                        "            const confirmation = confirm(`Are you sure you want to delete the file at path: ${filePath}?`);"
//                        "            if (confirmation) {"
//                        "                fetch(`http://${hostname}:${port}/${customEncodeURIComponent(filePath)}`, {"
//                        "                    method: \"DELETE\""
//                        "                })"
//                        "                    .then(response => {"
//                        "                        const resultMessage = document.querySelector(`[href=\"${filePath}\"]`).nextSibling;"
//                        "                        if (response.ok)"
//                        "                            resultMessage.textContent = \" File deleted successfully\";"
//                        "                        else {"
//                        "                            resultMessage.textContent = \" Error, file does not exist\";"
//                        "                        }"
//                        "                    })"
//                        "                    .catch(error => {"
//                        "                        console.error(\"An error occurred:\", error);"
//                        "                    });"
//                        "            }"
//                        "        }"
//                        "        function customEncodeURIComponent(uri) {"
//                        "            return encodeURIComponent(uri).replace(/%2F/g, \"/\").replace(/^upload\\//, \"\");"
//                        "        }"
//                        "    </script>"
//                        "</body>"
//                        "</html>";

    std::string start69 = "<!DOCTYPE html>\n"
                          "<html lang=\"en\">\n"
                          "<head>\n"
                          "    <meta charset=\"UTF-8\">\n"
                          "    <meta name=\"viewport\" content=\"\"width=device-width, initial-scale=1.0\"\">\n"
                          "    <title>Index of /</title>\n"
                          "    <link rel=\"stylesheet\" href=\"styles/styleIndex.css\">"
                          "</head>\n"
                          "<body>\n"
                          "    <div class=\"background-image\"></div>\n"
                          "    <div class=\"container\">\n"
                          "            <h1>Index of  /</h1><br>\n"
                          "            <div id=\"fileItems\"></div>\n"
                          "        </div>\n"
                          "    <script>\n"
                          "        const hostname = window.location.hostname;\n"
                          "        const port = window.location.port;\n"
                          "        const filePaths = [";
    std::string middle69 = generateList(ROOT_FOLDER, std::basic_string<char>());
    std::string end69 ="\"something/else\"];"
            "    </script>\n"
                        "    <script src=\"scripts/script.js\"></script>\n"
                        "</body>\n"
                        "</html>";
    std::string result = start69 + middle69 + end69;
    for (size_t i = 0; i < result.length(); ++i)
        _file.push_back(static_cast<uint8_t>(result[i]));

    return (DIRECTORY_LIST);
}

void Response::sendIndexPage()
{
    // if there is index.html in root folder
    if (INDEX)
    {
        if (Request::fileExists("index.html", ROOT_FOLDER))
            return (mySend(DEFAULTWEBPAGE));
        else
            return (mySend(ERROR_INDEXFILE));

    }
    else if (AUTOINDEX)
        return (mySend(getDirectoryIndexPage()));
    else
        return (mySend(FORBIDDEN));

}


void Response::sendRequestedFile()
{
    if (_info._url == INDEX_PAGE)
        return (sendIndexPage());

    struct stat s = {};

    if (stat((ROOT_FOLDER + _info._url).c_str(), &s) == 0)
    {
        if (IS_FOLDER)  //-> LATER if config is parsed
        {
            mySend(6969);
            std::cout << RED"ERROR: Cant handle Folders jet, do if config parser is done"RESET<< std::endl;
        }
        else if (IS_FILE)
        {
            _file = readFile(ROOT_FOLDER + _info._url);
            if (_file.empty())   // if file doesn't exist
                mySend(404);
            mySend(200);
        }
        else
        {
            mySend(500);
            std::cout << RED"ERROR: unexpected Error in sendRequestedFile()"RESET << std::endl;   // LATER WRITE IN ERROR FILE
        }
    }
    else
        mySend(404);
}


std::string Response::getContentType()
{
   if (_info._url.find('.') != std::string::npos)
   {
       size_t startPos = _info._url.find_last_of('.');
       size_t endPos = _info._url.size();

       std::string fileExtension;

       if (endPos != std::string::npos)
           fileExtension = (_info._url.substr(startPos + 1, endPos - (startPos)));
       else
           fileExtension = (_info._url.substr(startPos));
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
        _info._fileContentType = "text/html";

        if (statusCode == FILE_SAVED)
        {
            statusCode = 201;
            _file = readFile(PATH_FILE_SAVED);
        }
        else if (statusCode == FILE_DELETED)
        {
            statusCode = 204;
            _file = readFile(PATH_FILE_DELETED);
        }
        else if (statusCode == DIRECTORY_LIST)
        {
            statusCode = 200;
        }
        else if (statusCode == FILE_DELETED_FAIL)
        {
            statusCode = 404;
            _file = readFile(PATH_FILE_DELETED_FAIL);
        }
        else if (statusCode == DEFAULTWEBPAGE)
        {
            statusCode = 200;
            _file = readFile(PATH_DEFAULTWEBSITE);
        }
        else if (statusCode == FORBIDDEN)
        {
            statusCode = 403;
            _file = readFile(PATH_FORBIDDEN);
        }
        else if (statusCode == ERROR_INDEXFILE)
        {
            statusCode = 404;
            _file = readFile(PATH_ERROR_INDEXFILE);
        }
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
        {
            _file = readFile(PATH_500_ERRORWEBSITE);
            std::cout << RED"ERROR: status code not defined in mySend()"RESET << std::endl;
        }
    }
    else
    {
//        // can't find file extension
       _info._fileContentType = getContentType();
        if (_info._fileContentType == FAILURE)
            mySend(404);
    }
    std::string header = getHeader(statusCode);

    send(_info._clientSocket, header.c_str(), header.size(), 0);
    send(_info._clientSocket, (std::string(_file.begin(), _file.end())).c_str(), _file.size(), 0);
}


std::string Response::getHeader(int statusCode)
{
    std::string header;

    header = "HTTP/1.1 " + std::to_string(statusCode) + " " +
            ErrorResponse::getErrorMessage(statusCode) + "\r\nConnection: close\r\n"
                                                         "Content-Type: "+_info._fileContentType+"\r\n"
                                                                                           "Content-Length: " + std::to_string(_file.size()) + "\r\n\r\n";
    return header;
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






// ^ ^ ^  GET   ^ ^ ^

// --- --- --- --- ---

// v v v  POST  v v v



bool Response::uploadFile(const std::string& contentType, const std::string& boundary, std::ofstream *outfile)
{
    if (contentType == "multipart/form-data")
        return saveRequestToFile(*outfile, boundary);
    else if (contentType == "application/x-www-form-urlencoded")
        urlDecodedInput();
    return false;
}


bool Response::saveRequestToFile(std::ofstream &outfile, const std::string& boundary)
{
    std::string convert(_info._postInfo._input.begin(), _info._postInfo._input.end());
    std::string startBoundary = "--"+boundary+"\r\n";
    std::string endBoundary = "\r\n--"+boundary+"--";
    std::vector<uint8_t>::iterator startPos69 = _info._postInfo._input.begin();
    std::vector<uint8_t>::iterator endPos69 = _info._postInfo._input.end();
    bool endOfFile = false;

    size_t posStartBoundary = convert.find(startBoundary);
    size_t posEndBoundary = convert.find(endBoundary);

    if (NO_DATA_TO_UPLOAD)
        return true;

    if (posStartBoundary != std::string::npos)  // cut header and put stuff afterward to outfile
    {
        size_t bodyPos = convert.find("\r\n\r\n", (posStartBoundary + startBoundary.size() + 2)) + 4;

        if (bodyPos == std::string::npos)   // not the beginning of body content
            return true;

        startPos69 += bodyPos;
        if (posEndBoundary != std::string::npos)    // found last boundary
        {
            endOfFile = true;
            endPos69 = _info._postInfo._input.begin() + posEndBoundary;
        }
    }
    else if (posEndBoundary != std::string::npos)    // found last boundary
    {
        endPos69 = _info._postInfo._input.begin() + posEndBoundary;
        endOfFile = true;
    }
    std::vector<uint8_t>::iterator it;
    for (it = startPos69; it != endPos69; it++)
        outfile << *it;
    if (endOfFile)
    {
        outfile.close();
        mySend(FILE_SAVED);
        return false;
    }
    return true;
}


