

#pragma once


#ifndef MAPREDUCE_H_AS
#define MAPREDUCE_H_AS

#include <unordered_map>
#include <string>

class MapReduce
{
    size_t numMapJobs;
    size_t numReduceJobs;
    std::unordered_map<std::string> workers;
};







#endif