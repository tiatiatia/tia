// net.h: Function declarations for networking
// TIATIATIA!

/******************************************
How to use this file for networking:
connectToTIA(); connects to the TIA server machine.
The server's IP can be set with the SERVERIP macro
connectToClient(string ip); connects to a given ip
Once the connection is established, use the following:
getamsg() - get a message. Returns the message as a string
sendamsg(string tosend) - send a message.
Finally, call bye() to close sockets.
*******************************************/


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
//#include "tiautil.h"
#define SERVERPORT "6969"
#define CLIENTPORT "9696"
#define SERVERIP "140.103.108.188"
//#define SERVERIP "127.0.0.1"

using namespace std;

bool VERBOSE = false;

struct addrinfo hints, *res;
int sockfd, status;
char ip4con[INET_ADDRSTRLEN];
char ip6con[INET6_ADDRSTRLEN];
int portnumcon;
struct sockaddr_storage them;
int portnum;
int sizesa = sizeof(struct sockaddr_storage);
char ip4[INET_ADDRSTRLEN];
char ip6[INET6_ADDRSTRLEN];
char inmsg[512];

void bye() // Things to handle when trying to exit
{
	close(sockfd);
	freeaddrinfo(res);
}

void makesocket() // Create the socket, refered to by sockfd
{
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if((status=getaddrinfo(SERVERIP, SERVERPORT, &hints, &res))!=0) {
		if(VERBOSE) fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(status));
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd==-1) {
		if(VERBOSE) fprintf(stderr, "Error creating socket.\n");
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
}

void makesocket(string ip) // Create the socket, refered to by sockfd
{
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	if((status=getaddrinfo(ip.c_str(), CLIENTPORT, &hints, &res))!=0) {
		if(VERBOSE) fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(status));
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd==-1) {
		if(VERBOSE) fprintf(stderr, "Error creating socket.\n");
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
}

void connectto() // Open the connection
{
	if( ((struct sockaddr*)res->ai_addr)->sa_family==AF_INET)       // ip4
	{       
		inet_ntop(AF_INET, &(((struct sockaddr_in*)res->ai_addr)->sin_addr), ip4con, INET_ADDRSTRLEN);
		portnumcon = ntohs( ((struct sockaddr_in*)res->ai_addr)->sin_port );
		if(VERBOSE) printf("Connecting to %s:%d in ipv4...\n",ip4con,portnumcon);
	} else {
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)res->ai_addr)->sin6_addr), ip6con, INET6_ADDRSTRLEN);
		portnumcon = ntohs( ((struct sockaddr_in6*)res->ai_addr)->sin6_port );
		if(VERBOSE) printf("Connecting to %s:%d in ipv6...\n",ip6con,portnumcon);
	}
	
	if((status=connect(sockfd, res->ai_addr, res->ai_addrlen))!=0) {
		if(VERBOSE) fprintf(stderr, "Error connecting: %s\n", gai_strerror(status));
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
	
	if((status=getpeername(sockfd, (struct sockaddr*)&them, &((socklen_t)sizesa)))!=0) {
		if(VERBOSE) fprintf(stderr,"Error getting peer name: %s\n", gai_strerror(errno));
		else printf("Error connecting to the TIA server.\n");
	bye(); exit(1); }
	if(((struct sockaddr*)&them)->sa_family==AF_INET)	// ip4
	{
		inet_ntop(AF_INET, &(((struct sockaddr_in*)(&them))->sin_addr), ip4, INET_ADDRSTRLEN);
		portnum = ntohs( ((struct sockaddr_in*)(&them))->sin_port );
		if(VERBOSE) printf("Connected to %s:%d\n",ip4,portnum);
		else printf("Connected to the TIA server.\n");
	} else {
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(&them))->sin6_addr), ip6, INET6_ADDRSTRLEN);
		portnum = ntohs( ((struct sockaddr_in6*)(&them))->sin6_port );
		if(VERBOSE) printf("Connected to %s:%d\n",ip6,portnum);
		else printf("Connected to the TIA server.\n");
	}
}

// This function connects to the TIA server, as opposed to connectToClient()
void connectToTIA() { 
	makesocket();
	connectto();
}

// Connects to a TIA client machine, taking the IP address as the parameter
void connectToClient(string clientIP) {
	makesocket(clientIP);
	connectto();	
}

// Gets a message from the connected machine. Returns the message as a string
string getamsg()
{
	int bytes_got;
	bytes_got = recv(sockfd, inmsg, sizeof inmsg, 0);
	//char ch;
	//while ((ch = getchar()) != '\n' && ch != EOF);

	if(bytes_got==-1) {
		if(VERBOSE) fprintf(stderr, "Error: %s\n", strerror(errno));
		else printf("Error connecting to the TIA server.\n");
	bye(); exit(1); }
	inmsg[bytes_got] = '\0';
	if(VERBOSE) printf("Printing %d of %d bytes.\n", (int)strlen(inmsg), bytes_got);
	string returnstring(inmsg);
	return returnstring;
}

// Sends a message to the connected machine, taking the string as a parameter
void sendamsg(string inputstring)
{
	int len, bytes_sent;
	const char* outmsg = inputstring.c_str();
	len = strlen(outmsg);
	bytes_sent=send(sockfd, outmsg, len, 0);
	if(bytes_sent==-1) {
			if(VERBOSE) fprintf(stderr, "Error sending.\n");
				else printf("Error connecting to the TIA server.\n");
	bye(); exit(1); }
	if(VERBOSE) printf("Sent %d of %d bytes.\n", bytes_sent, len);
}

