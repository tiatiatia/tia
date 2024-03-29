/* Tiautil
* This is a collection of miscellaneous string/file-io related functions
* that are used in both TIA client and TIA server
*/
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <dirent.h>
using namespace std;

extern bool VERBOSE; //indicates whether the user is in verbose mode

string listdir(const char *path) {
// Takes in a directory name and returns
// a list of file names separated by newline characters
// used in the server to search through its IP databases
  stringstream filelist;
  struct dirent *entry;
  DIR *dp;
  dp = opendir(path);
  if (dp == NULL) {
    perror("error: directory is open");
    return NULL;
  }
  if (VERBOSE) cout << "reading filenames in directory " << path << "...";
  while((entry = readdir(dp)))
  { // read all files in directory
    filelist << entry->d_name;
    filelist << '\n';
    }
  closedir(dp);
  string output = filelist.str();
  // here we remove the troublesome dot and double dot file names
  while ((output[0] == '\n') || (output[0] == '.')) output.erase(output.begin());
  return output;
}
string stripCaps(string inputstr)
{
// this is a quick hack to replace capital letters with lowercases to
// avoid case confusion during the search function
	string notOK = "ABCDEFGHIJKLMNOPQRTSTUVXYZ";
	for (int i=0; i < inputstr.length();i++)
	{
		if (notOK.find(inputstr[i]) != string::npos)
		{
			inputstr[i]+=0x20; // A + 0x20 = a, etc.
		}
	}
	return inputstr;
}
bool clientAvailable(string clientIP)
{
// this takes a client IP address and checks connectedClients to see if the client
// is available
	//searchstr=clientIP
	string filename="./connectedClients.txt";
	fstream searchfile;
	string ipAddr;
	searchfile.open(filename.c_str(), ios_base::in);
	while(getline(searchfile,ipAddr))
	{
		if ( clientIP.find(ipAddr) != string::npos)
		{
			searchfile.close();
			return true;
		}
	}
	searchfile.close();
	return false;
}
string searchFiles(string searchstr, string clientIP)
{
// this takes a string and searches through every line of every file
// in the folder /Addresses/ in an attempt to find a full match of the string
// the results are outputted in a string of the form:
//  [filename] + '\n' + [line that matches search string] + '\n' + [filename] + ...
// note that the filename has the last 4 characters removed
// (these characters are assumed to be ".txt")
// the filename is assumed to be an IP address and the lines in the files
// are assumed to be file names, so the return value should be 
// alternating IP Address and file names
	searchstr = stripCaps(searchstr);
	fstream searchfile;
	stringstream filelist;
	filelist << listdir("./Addresses"); // get a list of files to search through
	string filename;
	string listing;
	string searchresults;
	try
	{
		if (VERBOSE) cout << "Searching database for \"" << searchstr << "\"..."<< endl;
		//getline(filelist,filename); //get rid of bacon herald message
		while(getline(filelist,filename))
		{ // search through each file
			size_t pos = filename.find(".txt");
			string ipName = filename.substr(0,pos);
			if(filename!=clientIP+".txt" && filename.size() > 4 && clientAvailable(ipName))
			{
				filename = "./Addresses/"+filename; // move directories
				searchfile.open(filename.c_str(), ios_base::in);
				filename = filename.erase(0,12); // format our file name to output
				filename.erase(filename.end() - 4,filename.end());
				while (getline(searchfile,listing))
				{// search through each line
					string namelisting = ""; // "listing" has name, size, and hash
					for(int i=0;i<listing.size();i++) // we only need the name
					{
						if(listing[i]=='\t') // once a tab is encountered, we have the name
							break;
						else namelisting+=listing[i];
					}
					namelisting = stripCaps(namelisting); 
					if ( namelisting.find(searchstr.c_str()) != string::npos)
					{ // found a match
					if (VERBOSE) cout << "Found match at "<< filename << "in file " << listing << endl;
						searchresults+=filename + '\n' + 
						listing + '\n';
					}
				}
				searchfile.close();
			}
		}
	}
	catch (exception e)
	{
	cerr << "some error: " << e.what() << endl;
	}
	return searchresults;
}

void writeString(string content, string name){
// This function takes a string and writes it to a file of the given name
// If the file already exists, it truncates the existing file and overwrites it.
// this is used by the server to update IP address files within the database
// the input should be of the form ( [received sync string], [sending IP address] )
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
			cerr << "File operation on file" << name <<" failed: " << e.what() << endl;
		}
}

void addString(string content, string name){
// This function appends a string to a file
// used by server to add connected IP addresses
	fstream filewriter;
	name = name + ".txt";
	try
	{
		if (VERBOSE) cout << "Adding to file " << name << "..." << endl;
		filewriter.open(name.c_str(), ios_base::in | ios_base::out | ios_base::ate);
		filewriter << content;
		filewriter.close();
	}
	catch(exception e)
	{
		cerr << "File operation on file" << name <<" failed: " << e.what() << endl;
	}
}

void removeString(string content, string name){
// This function takes a string and removes it from a file of the given name
// used by server to remove connected IP addresses
	string origName=name;
	name = name + ".txt";
	try
	{
		if (VERBOSE) cout << "Removing from file " << name << "..." << endl;
		ifstream fin(name.c_str());
		string fileContents,temp;
		while(fin>>temp)
		{
			if(temp==content)
			{}
			else
				fileContents+=temp+"\n";
		}
		writeString(fileContents, origName);
	}
	catch(exception e)
	{
		cerr << "File operation on file" << name <<" failed: " << e.what() << endl;
	}
}
