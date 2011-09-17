#include <fstream>
#include <string>
#include <iostream>
using namespace std;
// This function takes a string and writes it to a file of the given name
// If the file already exists, it truncates the existing file and overwrites it.
void writeString(string content, string name){

	fstream filewriter;
	name = name + ".txt";
	try
		{
		if (VERBOSE) cout << "Writing to file " << name << "..." << endl;
		filewriter.open(name.c_str(), ios_base::in | ios_base::out | ios_base::trunc);
		filewriter << content;
		filewriter.close();
		}
	catch(exception e)
		{
			cerr << "File operation on file" << name <<" failed" << endl;
		}
}
