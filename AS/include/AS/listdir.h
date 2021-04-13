#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

#include "string.h"
#include "vector.h"

// Lists the files in a directory relative to the current working directory
APESEARCH::vector<APESEARCH::string> listdir(const char * relativePath);

// Given an absolute path, lists the files inside that directory
APESEARCH::vector<APESEARCH::string> listdirAbsolute(const char * directory);