/* TIA server
* This program creates a listening socket on the port specified in netserver.h 
* The server will create a list of files stored in a given client's "./share" 
* folder and stores it in a folder called "./Addresses". If a search request is 
* given, then the server searches through ./Addresses and sends out the results
*/
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "servernet.h"
#include "tiautil.h"

using namespace std;

int communicate() {
	startServer();
	
	while(1) {
		
		for(int i=0; i<2; i++)
		{
			if(i==1){
				acceptcon();
				if(!fork()) {
					close(sockfd); // child process doesn't need the original socket
					string message = getamsg();
					stringstream messageholder;
					messageholder << message;
					string messageheader;
					getline(messageholder,messageheader);
					if (messageheader.find("bacon")!= string::npos)
					{ //synchronization information-- update the file database
						string ipaddress = getIpAddr();
						string datavalues = messageholder.str();
						writeString(datavalues,"./Addresses/" + ipaddress);
						//addString(getIpAddr()+"\n", "./connectedClients");
					}
					if (messageheader.find("cheese")!=string::npos)
					{ // search request-- initiate search
						string searchstr;
						getline(messageholder, searchstr);
						string searchresults = searchFiles(searchstr, getIpAddr());
						sendamsg(searchresults);
					}
					if(messageheader.find("alive")!=string::npos)
					{
						removeString(getIpAddr(), "./connectedClients");	//avoid duplicates
						addString(getIpAddr()+"\n", "./connectedClients");
					}
					if(messageheader.find("later")!=string::npos)
					{
						removeString(getIpAddr(), "./connectedClients");
					}
					close(newfd);
					exit(0);
				}
			}
			else{
				if(!fork()) {
					while(true) {
						cout << "Removing clients in 60 seconds..." << endl;
						clock_t endwait;
						int seconds = 60;
						endwait = clock()+seconds*CLOCKS_PER_SEC;
						while(clock() < endwait) {}
						writeString("", "./connectedClients");
					}
					close(newfd);
					exit(0);
				}
			}
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

