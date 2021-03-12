#include "assert.h"
#include <unistd.h>
#include <utility>
#include <iostream>

#ifndef _APESEARCH_QUERY_H
#define _APESEARCH_QUERY_H

namespace APESEARCH
{

class QueryParser {

public:
    QueryParser( char* query ) : completeQuery(query) {}

private:
    const char * completeQuery;
};

}
#endif
