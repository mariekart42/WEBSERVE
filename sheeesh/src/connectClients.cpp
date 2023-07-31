#include "../header/connectClients.hpp"

ConnectClients::ConnectClients():
    _clientSocket(), _clientAddressLen(sizeof(clientAddress)),
    clientAddress(), pollFds(), v_fdList()
{}


ConnectClients::~ConnectClients()
{
    printf("Closing client connection...\n");
    close(_clientSocket);
}


void ConnectClients::initFdList(int serverSocket)
{
    pollFds.fd = serverSocket;
    pollFds.events = POLLIN; // Monitoring for incoming data (readable)
    v_fdList.push_back(pollFds);
}



void ConnectClients::clientResponded(int serverSocket)
{
    // At least one file descriptor has an event
    for (size_t i = 0; i < MAX_USERS; ++i)
    {

        if (v_fdList[i].revents & POLLIN)
        {
            std::cout << YEL " . . . Accepting Connection from Client" RESET << std::endl;
            _clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &_clientAddressLen);
            if (_clientSocket < 0)
                exitWithError("Failed to init client Socket [EXIT]");

            // Data is available for reading on the corresponding file descriptor
            char buffer[MAX_USERS]; // Adjust the buffer size according to your needs
            ssize_t bytesRead = read(_clientSocket, buffer, sizeof(buffer));
//            ssize_t bytesRead = read(v_fdList[i].fd, buffer, sizeof(buffer));

            if (bytesRead > 0)
            {
                std::cout << "DATA [" << bytesRead << "] from Client: \n" GRN << buffer << RESET<< std::endl;

                // Handle the received data here
                // 'buffer' contains the received data, and 'bytesRead' is the number of bytes received
                // For example, you can process the data, send a response back to the client, etc.
                serverResponse obj(buffer, _clientSocket);
            } else if (bytesRead == 0)
            {
                std::cout << "Connection closed by client, handle here!!" << std::endl;
//                // Connection closed by the client
//                // Handle the disconnection if needed
                close(v_fdList[i].fd);
                v_fdList.erase(v_fdList.begin() + i);
                --i; // To compensate for the element removed from the vector
            } else
            {
                exitWithError("error while reading data from client with read()");
            }
        }

    }

//    exitWithError("DEBUG: end of connectClient()");
    
}

void TESTWEBSITE(int clientSocket)
{

    std::string responsePre = readFile("/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html");

    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nLocal time is: ";
    int bytes_sent = send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
    std::cout << "Sent " << bytes_sent << " of " << strlen(response.c_str()) << " bytes" << std::endl;
//	std::string response2 = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    send(clientSocket, responsePre.c_str(), strlen(responsePre.c_str()), 0);

}

void ConnectClients::connectClients(int serverSocket)
{
    initFdList(serverSocket);

    while (true)
    {
        switch (poll(v_fdList.data(), MAX_USERS, -1))
        {
            case -1:
                exitWithError("Failed to poll [EXIT]");
                break;
            case 0:
                std::cout << "poll returned 0, how to handle??" << std::endl;
                break;
            default:
                clientResponded(serverSocket);
        }
    }

}