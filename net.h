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
#include <fstream>
#include <sstream>
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
//#include "tiautil.h"
#define SERVERPORT "6969"
#define CLIENTPORT "9696"
//#define SERVERIP "140.103.108.179"  // spencer
//#define SERVERIP "140.103.108.174"  // 3714
//#define SERVERIP "140.103.108.226"	//trisha
//#define SERVERIP "140.103.47.21"	//killian
#define FOLDERNAME "./share/"
#define SERVERIP "127.0.0.1"
#define BACKLOG 100

using namespace std;

bool VERBOSE = false;

struct addrinfo hints, *res;
int newfd, sockfd, status;
char ip4con[INET_ADDRSTRLEN];
char ip6con[INET6_ADDRSTRLEN];
int portnumcon;
struct sockaddr_storage them;
int addr_size = sizeof them;
int portnum;
int sizesa = sizeof(struct sockaddr_storage);
char ip4[INET_ADDRSTRLEN];
char ip6[INET6_ADDRSTRLEN];
char inmsg[4096];
char *truaddr;

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void bye() // Things to handle when trying to exit
{
	close(sockfd);
	close(newfd);
	freeaddrinfo(res);
}

void makeTIAsocket(string ip) // Create the socket, refered to by sockfd. Pass a client's IP, or pass SERVERIP
{
	newfd = -1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	string port = ip == SERVERIP ? SERVERPORT : CLIENTPORT;
	if((status=getaddrinfo(ip.c_str(), port.c_str(), &hints, &res))!=0) {
		if(VERBOSE) fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(status));
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd==-1) {
		if(VERBOSE) fprintf(stderr, "Error creating socket.\n");
		else fprintf(stderr, "Could not connect to the TIA server.\n");
	bye(); exit(1); }
}

void makeServerSocket() // Create the socket, refered to by sockfd. Used to listen for other client connections
{
	newfd = -1; // initialize newfd
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if((status=getaddrinfo( NULL, CLIENTPORT, &hints, &res))!=0) {
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
	makeTIAsocket(SERVERIP);
	connectto();
}

// Connects to a TIA client machine, taking the IP address as the parameter
void connectToClient(string clientIP) {
	makeTIAsocket(clientIP);
	connectto();	
}

// Gets a message from the connected machine. Returns the message as a string
string getamsg()
{
	int bytes_got;
	string returnstring;
	if(newfd == -1) // if not in "server mode", accepting another client
		bytes_got = recv(sockfd, inmsg, sizeof inmsg, 0);
	else bytes_got = recv(newfd, inmsg, sizeof inmsg, 0); // newfd must be used
	bool endmsg = false;
	while(bytes_got>0) {
		if (inmsg[bytes_got-1] == '\r')
		{
			endmsg = true;
			inmsg[bytes_got-1]='\0';
		}
		else
		{
		inmsg[bytes_got] = '\0';
		}
		returnstring += (char*)inmsg;
		if(VERBOSE) printf("Receiving %d of %d bytes.\n", (int)strlen(inmsg), bytes_got);
		if (endmsg) break;
		bytes_got = recv(sockfd, inmsg, sizeof inmsg, 0);
	}

	if(bytes_got==-1) {
		if(VERBOSE) fprintf(stderr, "Error: %s\n", strerror(errno));
		else printf("Error connecting to the TIA server.\n");
	bye(); exit(1); }
	return returnstring;
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
	if(sockfd==-1) {
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

// Sends a message to the connected machine, taking the string as a parameter
void sendamsg(string inputstring)
{
	inputstring+='\r';
	int len, bytes_sent;
	const char* outmsg = inputstring.c_str();
	len = strlen(outmsg);
	if(newfd==-1) // check if we're in server mode
	bytes_sent=send(sockfd, outmsg, len, 0);
	else bytes_sent=send(newfd, outmsg, len, 0);
	if(bytes_sent==-1) {
			if(VERBOSE) fprintf(stderr, "Error sending.\n");
				else printf("Error connecting to the TIA server.\n");
	bye(); exit(1); }
	if(VERBOSE) printf("Sent %d of %d bytes.\n", bytes_sent, len);
}
void getafile()
{
	stringstream heraldreader;
	string herald = getamsg();
	long filesize;
	string filename;
	fstream filewriter;
	int bytes_got;
	heraldreader << herald;
	getline(heraldreader, filename);
	heraldreader >> filesize;
	filename = FOLDERNAME + filename;
	if (VERBOSE) cout << "attempting to write " << filesize << "bytes \
	to " << filename << "..." << endl;
	try
	{
		filewriter.open(filename.c_str(), ios_base::out);
		while (filesize > 0)
		{
			bytes_got = recv(sockfd, inmsg, sizeof inmsg, 0);
			cout << "bytes_got: " << bytes_got << endl;
			filesize -= bytes_got;
			filewriter.write(inmsg, bytes_got);
		}
		filewriter.close();
		if (VERBOSE) cout << "successfully received file" << endl;
	}
	catch (exception e)
	{
	cerr << "error writing file: " << e.what() << endl;
	}
}
void sendafile(string filename)
{
	string trufilename = FOLDERNAME + filename;
	fstream filereader;
	long filesize;
	FILE * sFile;
	char outmsg[4096];
	int bytes_read;
	int bytes_sent;
	int len;
	stringstream longconverter;
	try
	{
		if (VERBOSE) cout << "attempting to send the file " << trufilename 
		<< "..." << endl;
		sFile = fopen(trufilename.c_str(),"r");
		fseek(sFile,0,SEEK_END);
		filesize = ftell(sFile);
		fclose(sFile);
		if (VERBOSE) cout << trufilename << " is " << filesize << " bytes long" 
		<< endl;
		longconverter << filename << '\n' << filesize << '\n';
		sendamsg(longconverter.str());
		filereader.open(trufilename.c_str(), ios_base::in);
		/*while (filesize > 0)
		{
			filereader.get(outmsg, sizeof outmsg, NULL);
			bytes_read = filereader.gcount();
			cout << "bytes_read: " << bytes_read << endl;
			filesize -= bytes_read;
			bytes_sent=send(sockfd, outmsg, bytes_read, 0);
			if(bytes_sent==-1) {
			if(VERBOSE) fprintf(stderr, "Error sending.\n");
			bye(); exit(1); }
			if(VERBOSE) printf("Sent %d of %d bytes.\n", bytes_sent, bytes_read);
		}*/
		while (filereader.good())
		{
			filereader.read(outmsg, sizeof outmsg);
			bytes_read = filereader.gcount();
			cout << "bytes_read: " << bytes_read << endl;
			bytes_sent=send(newfd, outmsg, bytes_read, 0);
			if(bytes_sent==-1) {
			if(VERBOSE) fprintf(stderr, "Error sending.\n");
			bye(); exit(1); }
			if(VERBOSE) printf("Sent %d of %d bytes.\n", bytes_sent, bytes_read);
		}
		if (VERBOSE) cout << "File " << trufilename << " successfully sent" << endl;
		
	}
	catch(exception e)
	{
	cerr << "error sending or reading file: " << e.what() << endl;
	}
}

// Kills child processess
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
	makeServerSocket();
	bindlisten();
	sigaction();
}
