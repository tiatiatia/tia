// tiaserver.cpp is the main file for running the tia server

#include <iostream>
#include <vector>
#include "net.h"

using namespace std;

int communicate() {
	makesocket();
	bindlisten();
	sigaction();
		
	while(1) {
		acceptcon();

		if(!fork()) {
			close(sockfd);
			string message = getamsg();
			cout << message << endl;
			string line;
			getline(cin, line);
			sendamsg(line);
			close(newfd);
			exit(0);
		}
		close(newfd);
	}
	
	bye();
	return 0;
}

int main(int args, char* argv[]) {
	if(args==2) {
		if(argv[1]=="-v") VERBOSE = true;	
	}	
	communicate();
	return 0;
}
