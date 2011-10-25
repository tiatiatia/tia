// TIATIATIA!

#include <iostream>
#include <vector>
#include "tiautil.h"
#include "net.h"

using namespace std;

string SHAREPATH; // path to their local folder used for sharing
int MAX_RESULTS_TO_SHOW = 15; // maximum number of query results to show the user

/* sync() is called to send the server the client's
file information. It can be done at any time without harm */
int syncWithTIA() {
	connectToTIA(); // handles all connection to TIA, contained in net.h
	string fileinfo = listShareInfo(); // find what's in SHAREPATH
	sendamsg(fileinfo); // send file info to TIA server
	bye(); // close the connection
	return 0;
}

/* Gets the given file filetoget from the given client, ip */
void getFromClient(string ip, string filetoget)
{
	connectToClient(ip);
	sendamsg(filetoget);
	getafile();
	bye();
}

/* request() is a function that takes in a user entered
string, sends "cheese" (sentinal value indicating a request)
followed by the query to the TIA server, and receives the results.
It then prompts the user to select which of the results to attempt
to download, and tries to do it. */
void request(string query) {
	string serverQuery = "cheese\n"; // request signal
	serverQuery += query; // user entered query
	connectToTIA(); // open server connection
	sendamsg(serverQuery); // send query
	string serverAnswer = getamsg(); // get answer back from server
	bye(); // close connection
	vector<string> IPs;	// holds the parsed IPs
	vector<string> Filenames;  // holds the parsed filenames
	vector<string> FileSizes;	 // holds the file size info
	stringstream ss;
	ss << serverAnswer;
	string parsed;
	while(getline(ss,parsed)) { // as long as there's still a filename to read in
		IPs.push_back(parsed);
		getline(ss,parsed);
		string resultName = "", resultSize = "";
		int i;
		for(i=0;i<parsed.size();i++) {
			if(parsed[i]=='\t') break;
			else resultName+=parsed[i];
		}
		i++;
		for(i=0;i<parsed.size();i++) {
			if(parsed[i]=='\t') break;
			else resultSize+=parsed[i];
		}
		Filenames.push_back(resultName);
		FileSizes.push_back(resultSize);
	}		// NOTE: IPs[i] corresponds to Filenames[i]
	cout << "Found " << IPs.size() << " results. Type the number of the file you'd like to download, or \"cancel\".\n\n";
	cout << "Number\t\tFilename\t\tSize\t\tIP\n";
	for(int i=0; i < IPs.size() && i < MAX_RESULTS_TO_SHOW ; i++) {  // displays results
		cout << i+1 << "\t\t" << Filenames[i] << "\t\t" << IPs[i] << endl;
	}
	string fileToGetString; // read in which one they'd like to download
	getline(cin,fileToGetString);
	if(stripCaps(fileToGetString)=="cancel") return;
	stringstream convertToInt;
	convertToInt << fileToGetString;
	int fileToGet;
	convertToInt >> fileToGet;
	cout << "Attempting to get the file " << Filenames[fileToGet-1] << " from " << IPs[fileToGet-1] << endl;
	getFromClient(IPs[fileToGet-1], Filenames[fileToGet-1]);
}
void disconnect() {
	string serverQuery = "later";	//quit signal
	connectToTIA();	// open server connection
	sendamsg(serverQuery);	//send query
	bye();
}
 
int main(int argc, char* argv[]) {
	VERBOSE = false;
	if(argc==2) {
		string argstring = argv[1];
		if(argstring.find("-v")!= string::npos) 
		{
			VERBOSE = true;
			cout << "Verbose mode initiated" << endl;
		}
	}
	SHAREPATH = "./share/"; // initialize the sharing path to ./share/
	syncWithTIA(); // Sync files in SHAREPATH with TIA server
	if(!fork()) { // creates child process that listens for other clients needing a file
		while(true) { // main accept loop
			startServer(); // in net.h, sets up process as a server
			acceptcon();
			string fileRequested = getamsg();
			sendafile(fileRequested);
			bye();
		}
		exit(0);
	}
	string input;
	bool quit = false;
	cout << "Type a file name to search for it in the database. Type \"quit\" to exit." << endl;
	while(!quit) {
		getline(cin, input);
		if(input == "quit") {quit = true;disconnect();}
		else request(input);
	}
	return 0;
}
