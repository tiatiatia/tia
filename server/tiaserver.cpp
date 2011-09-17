// tiaserver.cpp is the main file for running the tia server

#include <iostream>
#include <vector>
#include "net.h"

using namespace std;

int main() {
	makesocket();
	bindlisten();
	sigaction();
	
	while(1) {
		acceptcon();

		if(!fork()) {
			closesockfd();
			char* message = getamsg();
			cout << message << endl;
			closenewfd();
			exit(0);
		}
		closenewfd();
	}
	
	bye();
	return 0;
}
