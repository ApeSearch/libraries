#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <vector>
#include <cmath>
#include <string>
#include <string.h>
#include <openssl/md5.h>
#include <bits/stdc++.h>

#define NUM_OF_BITS_INT (8 * sizeof(unsigned int))

class BitArray{
    public:
        BitArray(int size){
            int numOfInt = (size + NUM_OF_BITS_INT - 1) / NUM_OF_BITS_INT; //https://stackoverflow.com/questions/17944/how-to-round-up-the-result-of-integer-division
            numOfBits = size;
            array = new int[numOfInt];
            memset(array, 0, sizeof(*array));
        }
        void set(unsigned int index){
            array[index/NUM_OF_BITS_INT] |= (1U << (index%NUM_OF_BITS_INT));
            }
        void reset(unsigned int index){
            array[index/NUM_OF_BITS_INT] &= ~(1U << (index%NUM_OF_BITS_INT));
        }
        void flip(unsigned int index){
            array[index/NUM_OF_BITS_INT] ^= (1U <<(index%NUM_OF_BITS_INT));
        }
        int test(unsigned int index){
            return ( (array[index/NUM_OF_BITS_INT] & (1U<<(index%NUM_OF_BITS_INT))) != 0 );
        }
        int size(){
            return numOfBits;
        }
    private:
        int *array;
        unsigned int numOfBits;
};



class Bloomfilter {
public:
    Bloomfilter(int num_objects, double false_positive_rate) {
        //Determine the size of bits of our data vector, and resize

        //Determine number of hash functions to use]
        numOfExpectedObjects = num_objects;
        falsePostiveRate = false_positive_rate;
        size = -1 * ((numOfExpectedObjects * log(falsePostiveRate)) / (log(2) * log(2)));
        numOfHashFunc = (size / numOfExpectedObjects) * (log(2));
        bitSet = new BitArray(size);
    }

    ~Bloomfilter(){
        delete bitSet;
    }

    void insert(const std::string& s) {
        //Hash the string into two unique hashes
        std::pair<uint64_t, uint64_t> hashPair = hash(s);
        uint64_t index;

        for(uint64_t i = 0; i < numOfHashFunc; i++){
            index = (hashPair.first + i * hashPair.second) % size;
            bitSet->set(index);
        }

    }

    bool contains(const std::string& s) {
        //Hash the string into two unqiue hashes

        //Use double hashing to get unique bit, and repeat for each hash function
        //If bit is false, we know for certain this unique string has not been inserted

        //If all bits were true, the string is likely inserted, but false positive is possible
        std::pair<uint64_t, uint64_t> hashPair = hash(s);
        uint64_t index;

        for(uint64_t i = 0; i < numOfHashFunc; i++){
            index = (hashPair.first + i * hashPair.second) % size;
            if(bitSet->test(index) == 0){
                return false;
            }
        }

        return true;   
    }

private:
    //Add any private member variables that may be neccessary
    uint64_t size;
    uint64_t numOfExpectedObjects;
    uint64_t numOfHashFunc;
    double falsePostiveRate;
    BitArray *bitSet;


    std::pair<uint64_t, uint64_t> hash(const std::string& datum) {
        //Use MD5 to hash the string into one 128 bit hash, and split into 2 hashes
        unsigned char buffer[MD5_DIGEST_LENGTH];
        uint64_t hash1, hash2;

        MD5((const unsigned char *)(datum.c_str()), datum.length(), buffer );
        memcpy(&hash1, &buffer, 8 * sizeof( char ));
        memcpy(&hash2, &buffer[8], 8 * sizeof( char ));

        return {hash1, hash2};
    }
};

#endif