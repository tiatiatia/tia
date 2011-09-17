// TIATIATIA!

#include <iostream>
#include <vector>
#include "net.h"

using namespace std;

int communicate () {
	makesocket();
	connectto();
	//char msg[512];
	//fgets(msg,sizeof msg, stdin);
	string msg;
	getline(cin,msg);
	sendamsg(msg);
	string s = getamsg();
	cout << s << endl;

	bye();
	return 0;
}

int main(int argc, char* argv[]) {
	if(argc==2) {
		if(argv[1]=="-v") VERBOSE = true;
	}
	communicate();
	return 0;
}
