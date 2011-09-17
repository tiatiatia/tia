/**************************************************************
 * A simpler and shorter implementation of ls(1)
 * ls(1) is very similar to the DIR command on DOS and Windows.
 **************************************************************/
#include <iostream>
#include <sstream>
#include <string>
#include <dirent.h>
using namespace std;


string listdir(const char *path) {
// Takes in a directory name and returns
// a list of file names separated by newline characters
  stringstream filelist;
  struct dirent *entry;
  DIR *dp;
 
  dp = opendir(path);
  if (dp == NULL) {
    perror("opendir");
    return NULL;
  }
 
  while((entry = readdir(dp)))
  {
    filelist << entry->d_name;
    filelist << '\n';
    }
  closedir(dp);
  string output = filelist.str();
  output = output.erase(0,5);
  return output;
}