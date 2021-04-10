
#include "dependencies/unit_test_framework.h"
#include "../include/AS/listdir.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using std::cout; 
using std::endl;

char cwd[PATH_MAX];

TEST(list_invalid_directory)
    {
    APESEARCH::vector files = listdirAbsolute("this/doesnt/exist");
    ASSERT_EQUAL(files.size(), 0);
    }

TEST(list_directory)
    {
    int EXPECTED_SIZE = 4;
    char cwd[PATH_MAX];

    APESEARCH::vector<APESEARCH::string> correctFiles = {
        APESEARCH::string(strcat(getcwd(cwd, sizeof(cwd)), "/unitTests/listdirTestFiles/test1.txt")),
        APESEARCH::string(strcat(getcwd(cwd, sizeof(cwd)), "/unitTests/listdirTestFiles/test2.txt")),
        APESEARCH::string(strcat(getcwd(cwd, sizeof(cwd)), "/unitTests/listdirTestFiles/test3.txt")),
        APESEARCH::string(strcat(getcwd(cwd, sizeof(cwd)), "/unitTests/listdirTestFiles/test4.txt")),
    };

    APESEARCH::vector<APESEARCH::string> files = listdir("unitTests/listdirTestFiles");
    ASSERT_EQUAL(files.size(), EXPECTED_SIZE);

    for (int i = 0; i < files.size(); ++i)
        ASSERT_EQUAL(files[i], correctFiles[i]);
    }

TEST_MAIN()