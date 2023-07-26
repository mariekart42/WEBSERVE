#include "../header/tcpServer.hpp"

tcpServer::tcpServer(){}

tcpServer::tcpServer(std::string ipAddress, int port):
	myIpAddress(ipAddress),
	myPort(port),
	mySocket(),
	myNewSocket(),
	myIncomingMessage(),
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

	// just here so it compiles
	myIncomingMessage = 0;
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

int tcpServer::startServer()
{
	memset(&mySocketAddress, 0, sizeof(mySocketAddress));

	// AF_INET  indicates that the socket will be used to communicate over IPv4 networks
	mySocketAddress.ai_family = AF_INET;

	// SOCK_STREAM  indicates that we use TCP socket
	mySocketAddress.ai_socktype = SOCK_STREAM;

	// AI_PASSIVE  tells getaddrinfo() that we wanna set up the address to any available network interface
	mySocketAddress.ai_flags = AI_PASSIVE;


	// getaddrinfo() generates address thats suitable for bind()
	struct addrinfo *bindAddress;
	getaddrinfo(0, std::to_string(myPort).c_str(), &mySocketAddress, &bindAddress);  // prolly generates leak -> use of freeaddrinfo

// /*DEBUG*/ std::cout << "\tai_family: " << bindAddress->ai_family << "\n\tai_sockettype: " << bindAddress->ai_socktype << "\n\tai_protocol: " << bindAddress->ai_protocol << std::endl;

	std::cout << YEL " . . . Creating Socket" RESET << std::endl;
	// socket function returns file descriptor representing the socket -> mySocket
	mySocket = socket(bindAddress->ai_family, bindAddress->ai_socktype, bindAddress->ai_protocol);	 //domain, type, protocol
	if (mySocket < 0)
	{
		exitWithError("Cannot create socket");
		return FAILURE;
	}

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
	struct sockaddr_storage clientAddress; 
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

	std::cout << YEL " . . . Sending Response" RESET << std::endl;
	const char* response = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nLocal time is: ";
	int bytes_sent = send(socketClient, response, strlen(response), 0);
	std::cout << "Sent " << bytes_sent << " of " << strlen(response) << " bytes" << std::endl;
	// printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));


	time_t timer;
time(&timer);
char *time_msg = ctime(&timer);
bytes_sent = send(socketClient, time_msg, strlen(time_msg), 0);
printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));
printf("Closing connection...\n");
close(socketClient);

	return SUCCESS;
	// MAIN LOOP SHIT STARTS HERE
}





std::string tcpServer::buildResponse()
{
	std::string htmlFile = "<!DOCTYPE html><html lang=\"en\"><body><h1> HOME </h1><p> Hello from your Server :) </p></body></html>";
	std::ostringstream ss;
	ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << htmlFile.size() << "\n\n"
		<< htmlFile;

	return ss.str();
}