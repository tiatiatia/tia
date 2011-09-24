// tiaserver.cpp is the main file for running the tia server

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "net.h"
#include "tiautil.h"

using namespace std;

int communicate() {
	startServer();
	
	while(1) {
		acceptcon();

		if(!fork()) {
			close(sockfd); // child process doesn't need the original socket
			string message = getamsg();
			stringstream messageholder;
			messageholder << message;
			string messageheader;
			getline(messageholder,messageheader);
			if (messageheader.find("bacon")!= string::npos)
			{
				string ipaddress = getIpAddr();
				string datavalues = messageholder.str();
				writeString(datavalues,"./Addresses/" + ipaddress); 
			}
			if (messageheader.find("cheese")!=string::npos)
			{
				string searchstr;
				getline(messageholder, searchstr);
				string searchresults = searchFiles(searchstr);
				sendamsg(searchresults);
			}
			close(newfd);
			exit(0);
		}
		close(newfd);
	}
	
	bye();
	return 0;
}

int main(int argc, const char *argv[]) {
	if(argc==2) {
		string argstring = argv[1];
		if(argstring.find("-v")!= string::npos) 
		{
		VERBOSE = true;
		cout << "Verbose mode initiated" << endl;
		}
	}	
	communicate();
	return 0;
}
