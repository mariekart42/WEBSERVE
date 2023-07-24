#include "tcpServer.hpp"

tcpServer::tcpServer(){}

tcpServer::tcpServer(std::string ipAddress, int port):
	myIpAddress(ipAddress),
	myPort(port),
	mySocket(),
	myNewSocket(),
	myIncomingMessage(),
	mySocketAddress(),
	mySocketAddressLen(sizeof(mySocketAddress)),
	myServerMessage(buildResponse())
{
	if (startServer() == FAILURE)
		startServerError();
	else
		startListen();
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
	ss << "Failed to start server with PORT: " << ntohs(mySocketAddress.sin_port);
	logg(ss.str());
}

int tcpServer::startServer()
{
	// AF_INET  indicates that the socket will be used to communicate over IPv4 networks
	mySocketAddress.sin_family = AF_INET;

	// htons  convert unsigned short int from host byte order to network byte order
	mySocketAddress.sin_port = htons(myPort);

	// INADDR_ANY  represents the special IPv4 address "0.0.0.0" 
	// -> listens on all available network interfaces (IP addresses) on the host machine
	mySocketAddress.sin_addr.s_addr = INADDR_ANY;

	// SOCK_STREAM  indicates a TCP socket
	// socket function returns file descriptor representing the socket -> mySocket
	mySocket = socket(AF_INET, SOCK_STREAM, 0);
	if (mySocket < 0)
	{
		exitWithError("Cannot create socket");
		return FAILURE;
	}

	// bind  specify the address and port that the server socket will listen on for incoming connections
	if (bind(mySocket, (sockaddr *)&mySocketAddress, mySocketAddressLen) < 0)
	{
		exitWithError("Cannot connect socket to address");
		return FAILURE;
	}
	return SUCCESS;
}


void tcpServer::startListen()
{
	// listen function puts created socket into a passive listening state
	// -> allows server to accept() incoming client connections
	if (listen(mySocket, 20) < 0)
	{
		exitWithError("Socket listen failed");
	}


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