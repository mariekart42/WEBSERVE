#include "../header/client.hpp"

Client::Client(int serverSocket):
    clientAddressLen(sizeof(clientAddress))
{
    initClientSocket(serverSocket);

}

Client::~Client()
{
    printf("Closing connection...\n");
    close(clientSocket);
}


void Client::initClientSocket(int serverSocket)
{
    std::cout << YEL " . . . Waiting for connection" RESET << std::endl;

    // accept() makes program sleep/stop until it finds connection
    // on success, accept() creates new socket that can receive and send data
    //	(old socket waits for new connections)
    // also: accept() fills data about clientAddress
    clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressLen);
    if (clientSocket < 0)
        exitWithError("accept() failed");

    std::cout << YEL " . . . Waiting for connection" RESET << std::endl;
    char address_buffer[100];

    // writes hostname output to clientAddress
    // writes service name to address_buffer
    // NI_NUMERICHOST let us see hostname as IP address
    getnameinfo((struct sockaddr*)&clientAddress,
                clientAddressLen, address_buffer, sizeof(address_buffer), nullptr, 0, NI_NUMERICHOST);

    std::cout << YEL " . . . Reading Request" RESET << std::endl;
    char request[1024];
    int bytes_received = recv(clientSocket, request, 1024, 0);
    if (bytes_received <= 0)
        exitWithError("unable to read client request!");

    std::cout << "Received " << bytes_received << " bytes" << std::endl;
// with recv we get HTTP Request Line  (HTTP method (GET, POST, ..), requested URL and HTTP version)
    std::cout << "Received data:\n";
    printf(GRN"%s\n", request);


    std::cout << YEL " . . . Sending Response" RESET << std::endl;

    std::string responsePre = readFile("/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html");

    std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nLocal time is: ";
    int bytes_sent = send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
    std::cout << "Sent " << bytes_sent << " of " << strlen(response.c_str()) << " bytes" << std::endl;
//	std::string response2 = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    send(clientSocket, responsePre.c_str(), strlen(responsePre.c_str()), 0);




}

