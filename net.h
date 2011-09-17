// net.h: Function declarations for networking
// TIATIATIA!

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>			// CLIENT  8======>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#define SERVERPORT "6969"
#define CLIENTPORT "9696"
#define SERVERIP "140.103.108.188"

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
		fprintf(stderr, "Error getting addrinfo: %s\n", gai_strerror(status));
	bye(); exit(1); }
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(sockfd==-1) {
		fprintf(stderr, "Error creating socket.\n");
	bye(); exit(1); }
}

connectto() // Open the connection
{
	if( ((struct sockaddr*)res->ai_addr)->sa_family==AF_INET)       // ip4
	{       
		inet_ntop(AF_INET, &(((struct sockaddr_in*)res->ai_addr)->sin_addr), ip4con, INET_ADDRSTRLEN);
		portnumcon = ntohs( ((struct sockaddr_in*)res->ai_addr)->sin_port );
		printf("Connecting to %s:%d...\n",ip4con,portnumcon);
	} else {
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)res->ai_addr)->sin6_addr), ip6con, INET6_ADDRSTRLEN);
		portnumcon = ntohs( ((struct sockaddr_in6*)res->ai_addr)->sin6_port );
		printf("Connecting to %s:%d...\n",ip6con,portnumcon);
	}
	
	if((status=connect(sockfd, res->ai_addr, res->ai_addrlen))!=0) {
		fprintf(stderr, "Error connecting: %s\n", gai_strerror(status));
	bye(); exit(1); }
	
	if((status=getpeername(sockfd, (struct sockaddr*)&them, &sizesa))!=0) {
		fprintf(stderr,"Error getting peer name: %s\n", gai_strerror(errno));
	bye(); exit(1); }
	if(((struct sockaddr*)&them)->sa_family==AF_INET)	// ip4
	{
		inet_ntop(AF_INET, &(((struct sockaddr_in*)(&them))->sin_addr), ip4, INET_ADDRSTRLEN);
		portnum = ntohs( ((struct sockaddr_in*)(&them))->sin_port );
		printf("Connected to %s:%d\n",ip4,portnum);
	} else {
		inet_ntop(AF_INET6, &(((struct sockaddr_in6*)(&them))->sin6_addr), ip6, INET6_ADDRSTRLEN);
		portnum = ntohs( ((struct sockaddr_in6*)(&them))->sin6_port );
		printf("Connected to %s:%d\n",ip6,portnum);
	}
}

void getamsg()
{
	int bytes_got;
	bytes_got = recv(sockfd, inmsg, sizeof inmsg, 0);
	char ch;
	while ((ch = getchar()) != '\n' && ch != EOF);

	if(bytes_got==-1) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
	bye(); exit(1); }
	inmsg[bytes_got] = '\0';
	printf("%s",inmsg);
	printf("Printed %d of %d bytes.\n", (int)strlen(inmsg), bytes_got);
}

void sendamsg(char *outmsg)
{
	int len, bytes_sent;
	len = strlen(outmsg);
	bytes_sent=send(sockfd, outmsg, len, 0);
	if(bytes_sent==-1) {
			fprintf(stderr, "Error sending.\n");
	bye(); exit(1); }
	printf("Sent %d of %d bytes.\n", bytes_sent, len);

}

