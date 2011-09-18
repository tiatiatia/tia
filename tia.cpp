// TIATIATIA!

#include <iostream>
#include <vector>
#include "net.h"
#include "tiautil.h"

using namespace std;

int communicate () {
	makesocket();
	connectto();
	//char msg[512];
	//fgets(msg,sizeof msg, stdin);
	string msg;
	getline(cin,msg);
	sendamsg(msg);
	makesocket();
	connectto();
	string s = getamsg();
	cout << s << endl;

	bye();
	return 0;
}

int main(int argc, char* argv[]) {
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
