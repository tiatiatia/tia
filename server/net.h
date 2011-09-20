// server's net.h

/**********************************************
How to use this file:
startServer(); prepares the machine's 6969 port
for receiving connections.
acceptcon(); accepts a new connection.
getamsg() waits for a message, returns it as a string
sendamsg(string msg) sends a message to the client connected
bye(); cleans up by closing the sockets
**********************************************/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#define BACKLOG 100
#define SERVERPORT "6969"

using namespace std;

bool VERBOSE = false;

struct addrinfo hints, *res;
int sockfd, newfd, status;
struct sockaddr_storage them;
int addr_size = sizeof them;
char ip4[INET_ADDRSTRLEN];
char ip6[INET6_ADDRSTRLEN];
char *truaddr;

void closesockfd() { close(sockfd); }
void closenewfd() { close(newfd); }

void bye()
{
//	close(sockfd);
//	close(newfd);
	freeaddrinfo(res);
}

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void makesocket()
{
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status=getaddrinfo( NULL, SERVERPORT, &hints, &res))!=0) {
		if(VERBOSE) fprintf(stderr, "Error: %s\n", gai_strerror(status));
		else fprintf(stdin, "Sorry, could not start server.");
	bye(); exit(1); }
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd==-1) {
		if(VERBOSE) fprintf(stderr, "Error creating socket.\n");
		else fprintf(stdin, "Sorry, could not start server.");
	bye(); exit(1); }
	int yes = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1) {
		if(VERBOSE) perror("setsockopt error\n");
		else fprintf(stdin, "Sorry, could not start server.");
		exit(1); }
}

void bindlisten()
{
	if((status=bind(sockfd, res->ai_addr, res->ai_addrlen))!=0) {
		if(VERBOSE) fprintf(stderr, "Error binding: %s.\n", gai_strerror(errno));
		else fprintf(stdin, "Sorry, could not start server.");
	bye(); exit(1); }
	if((status=listen(sockfd, BACKLOG))!=0) {
		if(VERBOSE) fprintf(stderr, "Error listening.\n");
		else fprintf(stdin, "Sorry, could not start server.");
	bye(); exit(1); }
	printf("Server successfully started. Waiting for connections...\n");
}

void acceptcon()
{
	newfd = accept(sockfd, (struct sockaddr*)&them, &((socklen_t)addr_size));
	if(newfd==-1) {
		if(VERBOSE) fprintf(stderr, "Error accepting\n");
		else fprintf(stdin, "Sorry, could not accept connection.");
	bye(); exit(1); }
	if( ((struct sockaddr*)&them)->sa_family==AF_INET)
	{
		inet_ntop(AF_INET, &(((struct sockaddr_in*)(&them))->sin_addr), ip4, INET_ADDRSTRLEN);
		if(VERBOSE) printf("Recieved ip4 connection from %s\n", ip4);
		truaddr = ip4;
	} else {
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(&them))->sin6_addr), ip6, INET6_ADDRSTRLEN);
		if(VERBOSE) printf("Recieved ip6 connection from %s\n", ip6);
		truaddr = ip6;
	}
}

string getamsg()
{
	int bytes_got;
	char inmsg[4096];
	string returnstring;
	bytes_got = recv(newfd, inmsg, sizeof inmsg, 0);
	while(bytes_got > 0)
	{
//		clreol(stdin);
		inmsg[bytes_got] = '\0';
		returnstring += (char*)inmsg;
		if(VERBOSE) printf("Receiving %d of %d bytes.\n", (int)strlen(inmsg), bytes_got);
		bytes_got = recv(newfd, inmsg, sizeof inmsg, 0);
	}
	
	if(bytes_got==-1) {
		if(VERBOSE) fprintf(stderr, "Error getting message: %s\n", strerror(errno));
		else fprintf(stderr, "Error getting message.");
	bye(); exit(1); }
	return returnstring;
}

void sendamsg(string inputstring)
{
	int len, bytes_sent;
	const char* outmsg = inputstring.c_str();
	len = strlen(outmsg);
	bytes_sent=send(newfd, outmsg, len, 0);
	if(bytes_sent==-1) {
		if(VERBOSE) fprintf(stderr, "Error sending.\n");
	bye(); exit(1); }
	if(VERBOSE) printf("Sent %d of %d bytes.\n", bytes_sent, len);
}
string getIpAddr()
{
	if(truaddr == (char*)&ip4) return ip4;
	else return ip6;
}
void sigaction() {
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		if(VERBOSE) perror("sigaction error\n");
		exit(1); }
}

// Initializes the server as needed
void startServer() {
	makesocket();
	bindlisten();
	sigaction();
}

