#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <cmath>
#include <string>
#include "../../../AS/include/AS/string.h"
#include "../../../AS/include/AS/File.h"
#include "../../../AS/include/AS/mutex.h"
#include "../../../AS/include/AS/utility.h"
#include <shared_mutex>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

//-D_LARGEFILE64_SOURCE

#define NUM_OF_BITS_INT (8 * sizeof(unsigned int))
#define NUM_OF_OBJECTS 200000000
#define FILE_NAME "bloomFilter.bin"
#define FALSE_POS_RATE 0.0001

class BitArray{
    public:
        BitArray(ssize_t size, int fd) {
            ssize_t numOfInt = (size + NUM_OF_BITS_INT - 1) / NUM_OF_BITS_INT; //https://stackoverflow.com/questions/17944/how-to-round-up-the-result-of-integer-division
            numOfBits = size;
            int fileSize = lseek( fd, 0, SEEK_END );
            //ssize_t fileSize = lseek64(fd, 0, SEEK_END);
            if((long unsigned int)fileSize < numOfInt * sizeof(unsigned int))
                {
                int result = lseek( fd, int( ( numOfInt * sizeof(int) ) - 1 ), SEEK_SET );
                //ssize_t result = lseek64( fd, off64_t( ( numOfInt * sizeof(int) ) - 1 ), SEEK_SET );
                if ( result == -1 )
                    {
                    perror( "Issue with lseek while trying to stretch file" );
                    return;
                } // end if
                result = write(fd, "", 1 );
                if ( result == -1 )
                   {
                   perror( "Error writing bytes to file" );
                   return;
                   }
                //struct stat64 buf;
                //fstat( fd, &buf );
                //fstat64( fd, &buf );
                //assert( buf.st_size == fileSize );
                }
            array = (int *) mmap(NULL, numOfInt * sizeof(unsigned int), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
            
        }
        ~BitArray() {
            munmap( array, numOfBits * sizeof(char) * sizeof(int) );
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
    Bloomfilter() {
        //Determine the size of bits of our data vector, and resize
        int num_objects = NUM_OF_OBJECTS;
        double false_positive_rate = FALSE_POS_RATE;
        const APESEARCH::string name(FILE_NAME);
        //Determine number of hash functions to use]
        numOfExpectedObjects = num_objects;
        falsePostiveRate = false_positive_rate;
        size = -1 * ((numOfExpectedObjects * log(falsePostiveRate)) / (log(2) * log(2)));
        numOfHashFunc = (size / numOfExpectedObjects) * (log(2));
        APESEARCH::File file( name.cstr(), O_RDWR | O_CREAT, mode_t(0600) );
        bitSet = new BitArray(size, file.getFD());
    }

    ~Bloomfilter(){
        delete bitSet;
    }

    void insert(const APESEARCH::string& s) 
    {
        //Hash the string into two unique hashes
        lock.lock();

        APESEARCH::pair<uint64_t, uint64_t> hashPair = hash(s);
        uint64_t index;

        for(uint64_t i = 0; i < numOfHashFunc; i++){
            index = (hashPair.first() + i * hashPair.second()) % size;
            bitSet->set(index);
        }

        lock.unlock();
    }

    bool contains(const APESEARCH::string& s) {
        //Hash the string into two unqiue hashes

        //Use double hashing to get unique bit, and repeat for each hash function
        //If bit is false, we know for certain this unique string has not been inserted

        //If all bits were true, the string is likely inserted, but false positive is possible
        lock.lock_shared();

        APESEARCH::pair<uint64_t, uint64_t> hashPair = hash(s);
        uint64_t index;
        bool retVal = true;
        for(uint64_t i = 0; i < numOfHashFunc; i++){
            index = (hashPair.first() + i * hashPair.second()) % size;
            if(bitSet->test(index) == 0)
                {
                retVal = false;
                break;
                }   
        }
        lock.unlock_shared();
        return retVal;
    }

private:
    //Add any private member variables that may be neccessary
    uint64_t size;
    uint64_t numOfExpectedObjects;
    uint64_t numOfHashFunc;
    double falsePostiveRate;
    BitArray *bitSet;
    std::shared_mutex lock;


    APESEARCH::pair<uint64_t, uint64_t> hash(const APESEARCH::string& datum) {
        //Use MD5 to hash the string into one 128 bit hash, and split into 2 hashes
        unsigned char buffer[MD5_DIGEST_LENGTH];
        uint64_t hash1, hash2;

        MD5((const unsigned char *)(datum.cstr()), datum.size(), buffer );
        memcpy(&hash1, &buffer, 8 * sizeof( char ));
        memcpy(&hash2, &buffer[8], 8 * sizeof( char ));

        return {hash1, hash2};
    }
};

#endif