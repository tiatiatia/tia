// TIATIATIA!

#include <iostream>
#include <vector>
#include "tiautil.h"
#include "net.h"

using namespace std;

string SHAREPATH;


void syncWithTIA() {
	string fileinfo = listdir(SHAREPATH.c_str()); // find what's in SHAREPATH
	sendamsg(fileinfo); // send file info to TIA server
}

int communicate () {
	connectToTIA();
	/*string msg;
	getline(cin,msg);
	sendamsg(msg);*/
	syncWithTIA();
	
	//string s = getamsg();
	//cout << s << endl;

	bye();
	return 0;
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
	communicate(); // begin main communication
	return 0;
}
