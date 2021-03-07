
#include "../include/AS/algorithms.h"

//------------------------------------------------------------------------------------------------
//
//                                  Algorithms Definition 
//
//------------------------------------------------------------------------------------------------

int strIcmp(const char *a, const char *b) {
    int letterA, letterB;

    do {
        // Accounts for non-2's complement as well as 2's coplmenet platforms.
        // Since +0 and -0 would fold to a unsigned 0 but in reality only +0 should be converted to 0.
        letterA = *((unsigned char *)a++);
        letterB = *((unsigned char *)b++);
        letterA = tolower(toupper(letterA));
        letterB = tolower(toupper(letterB));
    }
    while ( letterA == letterB && letterA != '\0');
    return letterA - letterB;
} // end strIcmp()