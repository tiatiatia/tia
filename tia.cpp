// TIATIATIA!

#include <iostream>
#include <vector>
#include "tiautil.h"
#include "net.h"

using namespace std;

string SHAREPATH; // path to their local folder used for sharing
int MAX_RESULTS_TO_SHOW; // maximum number of query results to show the user

/* sync() is called to send the server the client's
file information. It can be done at any time without harm */
int sync() {
	connectToTIA(); // handles all connection to TIA, contained in net.h
	string fileinfo = listdir(SHAREPATH.c_str()); // find what's in SHAREPATH
	sendamsg(fileinfo); // send file info to TIA server
	bye(); // close the connection
	return 0;
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
	stringstream ss;
	ss << serverAnswer;
	string parsed;
	while(getline(ss,parsed)) { // as long as there's still a filename to read in
		Filenames.push_back(parsed);
		getline(ss,parsed);
		IPs.push_back(parsed);
	}		// NOTE: IPs[i] corresponds to Filenames[i]
	cout << "Found " << IPs.size() << " results. Type the number of the file you'd like to download.\n\n";
	cout << "Number\t\tFilename\t\tIP\n";
	for(int i=0; i < IPs.size() && i < MAX_RESULTS_TO_SHOW ; i++) {  // displays results
		cout << i+1 << "\t\t" << Filenames[i] << "\t\t" << IPs[i] << endl;
	}
	int fileToGet; // read in which one they'd like to download
	cin >> fileToGet;
	getFromClient(IPs[fileToGet-1], Filenames[fileToGet-1]);
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
	sync(); // Sync files in SHAREPATH with TIA server
	string input;
	bool quit = false;
	cout << "Type a file name to search for it in the database. Type \"quit\" to exit." << endl;
	while(!quit) {
		getline(cin, input);
		if(input == "quit") quit = true;
		else request(input);
	}
	return 0;
}
