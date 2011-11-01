#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <regex.h>
using namespace std;
#define NUMVARS 2
static string VARS[NUMVARS] = {"sharefolder","serverip"};
extern string SHAREPATH;
extern string FOLDERNAME;

void commandhandler(int varnum, string varstring)
{// This function maintains a list of variables to be modified in the config file and specifies
 // the proper action to be taken with such variable modifications.
	switch(varnum)
	{
		case 0:
		FOLDERNAME = varstring;
		break;
		case 1:
		SERVERIP = varstring;
		break;
	}
}
void getConfig(){
//This function attempts to parse the contents of "tia.config". If contents are
//successfully parsed, then the appropriate properties are altered
// Otherwise, default settings are loaded.
	char data[1000];
	string inliner;
	string parseline;
	int rc;
	regex_t myregex;
	regmatch_t myregmatch[2];
	fstream filereader;
	try
		{
			if (VERBOSE) cout << "Reading from file tia.config..." << endl;
			filereader.open("tia.config", ios_base::in);
			while (!filereader.eof())
			{
				rc = regcomp(&myregex, "^\\s*[^#]*", 0);
				getline( filereader, inliner);
				rc = regexec(&myregex, inliner.c_str(), 1 , &myregmatch[0],0);
				if (!rc && (int)myregmatch[0].rm_eo!= 0)
				{
					int diff = (int)myregmatch[0].rm_eo;
					cout << diff << endl;
					parseline = inliner.substr(0, diff);
					for(int i=0;i<NUMVARS;i++)
					{
					string tempstr=VARS[i] + "='(.*)'";
					rc = regcomp(&myregex, tempstr.c_str(), REG_EXTENDED);
					rc = regexec(&myregex, parseline.c_str(), 2, &myregmatch[0],REG_EXTENDED);
					if (!rc) commandhandler(i,parseline.substr(myregmatch[1].rm_so,myregmatch[1].rm_eo - myregmatch[1].rm_so));
					}
				}
			}
			filereader.close();
		}
	catch(exception e)
		{
			cerr << "File operation on file tia.config failed: " << e.what() << endl;
		}
}