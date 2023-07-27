#include "../header/tcpServer.hpp"

tcpServer::tcpServer():
	myIpAddress("0.0.0.0"),
	myPort(8080),
	mySocket(),
	myNewSocket(),
	mySocketAddress(),
	// mySocketAddressLen(sizeof(mySocketAddress)),
	myServerMessage(buildResponse())
{
    std::cout << "default constructor" << std::endl;
}

tcpServer::tcpServer(std::string &ipAddress, int port):
	myIpAddress(ipAddress),
	myPort(port),
	mySocket(),
	myNewSocket(),
	mySocketAddress(),
	// mySocketAddressLen(sizeof(mySocketAddress)),
	myServerMessage(buildResponse())
{
	// on SUCCESS, socket got created and is bound to local address
	// -> we can start listen() for connections now
	if (startServer() == SUCCESS)
		startListen();
	else
		startServerError();

	// std::cout << 
//	buildResponse();
	// just here so it compiles
}


tcpServer::~tcpServer()
{
	close(mySocket);
	close(myNewSocket);
	exit(0);
}


void tcpServer::startServerError()
{
	std::ostringstream ss;
	ss << "Failed to start server with PORT: " << std::endl;	//ntohs(mySocketAddress);
	logg(ss.str());
}

int tcpServer::initMySocket()
{
	memset(&mySocketAddress, 0, sizeof(mySocketAddress));

	// AF_INET  indicates that the socket will be used to communicate over IPv4 networks
	mySocketAddress.ai_family = AF_INET;

	// SOCK_STREAM  indicates that we use TCP socket
	mySocketAddress.ai_socktype = SOCK_STREAM;

	// AI_PASSIVE  tells getaddrinfo() that we wanna set up the address to any available network interface
	mySocketAddress.ai_flags = AI_PASSIVE;

	// getaddrinfo() generates address thats suitable for bind()
	getaddrinfo(0, std::to_string(myPort).c_str(), &mySocketAddress, &bindAddress);  // prolly generates leak -> use of freeaddrinfo

	std::cout << YEL " . . . Creating Socket" RESET << std::endl;
	// socket function returns file descriptor representing the socket -> mySocket
	mySocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 //domain, type, protocol
	if (mySocket < 0)
	{
		exitWithError("Cannot create socket");
		return FAILURE;
	}
    return SUCCESS;
}


int tcpServer::startServer()
{
    if (initMySocket() == SUCCESS)
    {
        std::cout << YEL " . . . Binding socket to local address" RESET << std::endl;
        // bind  specify the address and port that the server socket will listen on for incoming connections
        if (bind(mySocket, bindAddress->ai_addr, bindAddress->ai_addrlen) < 0)
        {
            // bind() fails if port is already used
            // ->  close program that uses that port or change port number
            exitWithError("Cannot connect socket to address");
            return FAILURE;
        }
        freeaddrinfo(bindAddress);
	    return SUCCESS;
    }
    return FAILURE;
}


int tcpServer::startListen()
{
	std::cout << YEL " . . . Listening" RESET << std::endl;
	// listen function puts created socket into a passive listening state
	// -> allows server to accept() incoming client connections
	//	  (second arg: how many client connections allowed to queue up)
	if (listen(mySocket, 10) < 0)
	{
		exitWithError("Socket listen failed");
	}

//!!// LATER PUT ALL DIS IN CLIENT CLASS

	std::cout << YEL " . . . Waiting for connection" RESET << std::endl;
	struct sockaddr_storage clientAddress = {};
	socklen_t clientLen = sizeof(clientAddress);

	// accept() makes program sleep/stop until it finds connection
	// on success, accept() creates new socket that can receive and send data
	//	(old socket waits for new connections)
	// also: accept() fills data about clientAddress
	int socketClient = accept(mySocket, (struct sockaddr*) &clientAddress, &clientLen);
	if (socketClient < 0)
	{
		exitWithError("accept() failed");
		return FAILURE;
	}

	std::cout << YEL " . . . Waiting for connection" RESET << std::endl;
	char address_buffer[100];

	// writes hostname output to clientAddress
	// writes service name to address_buffer
	// NI_NUMERICHOST let us see hostname as IP address
	getnameinfo((struct sockaddr*)&clientAddress,
               clientLen, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

	std::cout << YEL " . . . Reading Request" RESET << std::endl;
	char request[1024];
	int bytes_received = recv(socketClient, request, 1024, 0);
	if (bytes_received <= 0)
	{
		exitWithError("unable to read client request!");
		return FAILURE;
	}
	std::cout << "Received " << bytes_received << " bytes" << std::endl;
// with recv we get HTTP Request Line  (HTTP method (GET, POST, ..), requested URL and HTTP version)
	std::cout << "Received data:\n";
	printf(GRN"%s\n", request);


	std::cout << YEL " . . . Sending Response" RESET << std::endl;

    std::string responsePre = readFile("/Users/mmensing/Desktop/42CODE/WEBSHIT/sheeesh/images.html");

	std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\n\r\nLocal time is: ";
	int bytes_sent = send(socketClient, response.c_str(), strlen(response.c_str()), 0);
	std::cout << "Sent " << bytes_sent << " of " << strlen(response.c_str()) << " bytes" << std::endl;
//	std::string response2 = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
    send(socketClient, responsePre.c_str(), strlen(responsePre.c_str()), 0);


    printf("Closing connection...\n");
    close(socketClient);

    return SUCCESS;
	// MAIN LOOP SHIT STARTS HERE
}





std::string tcpServer::buildResponse()
{
	std::ostringstream ss;
	std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	ss << BLU"HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: \x1B[0m" << htmlFile.size() << "\n\n"
		<< htmlFile;

	return ss.str();
}

std::string tcpServer::readFile(const std::string &fileName)
{
    std::ifstream inputFile(fileName);

    std::cout << "DIS IS FILE: " << fileName << std::endl;

    if (!inputFile.is_open())
    {
        exitWithError("Unable to open File");
        exit(FAILURE);
    }

    // Use a stringstream to store the content of the file.
    std::stringstream buffer;
    buffer << inputFile.rdbuf();

    inputFile.close();

    std::cout << buffer.str() << std::endl;

    // Return the content as a string.
    return buffer.str();
}
