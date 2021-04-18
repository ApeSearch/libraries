#include "../include/AS/listdir.h"

APESEARCH::vector<APESEARCH::string> listdirAbsolute(const char * directory);

// Lists the files in a directory relative to the current working directory
APESEARCH::vector<APESEARCH::string> listdir(const char * relativePath) 
   {
   char cwd[PATH_MAX];
   getcwd(cwd, sizeof(cwd));

   const char * slash = "/";
   const char * directory = strcat(cwd, slash);
   directory = strcat(cwd, relativePath);

   return listdirAbsolute(directory);
   }

// Given an absolute path, lists the files inside that directory
APESEARCH::vector<APESEARCH::string> listdirAbsolute(const char * directory) 
   {
   APESEARCH::vector<APESEARCH::string> files;
   DIR *dir;
   struct dirent *dp;

   if ((dir = opendir (directory)) == NULL) 
      {
      perror (directory);
      return files;
      }

   while ((dp = readdir (dir)) != NULL) 
      {
      // Ignore '.' and '..' because we don't want to traverse literally everything
      if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
         {
         
         if (dp->d_type != DT_DIR)
            {
            APESEARCH::string newFile = directory;
            newFile += "/";
            newFile += dp->d_name;

            files.emplace_back(newFile);
            }
         }
      }
   delete dir;
   return files;
   }
