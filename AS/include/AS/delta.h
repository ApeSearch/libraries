#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include "./vector.h"

// LEB128 Encode
// Encode 7 bits at a time in the format:
// 1xxxxxxx 1xxxxxxx 0xxxxxxx
// Where 1 indicates there is another byte following, 0 is the last byte
APESEARCH::vector<uint8_t> encodeDelta(size_t tokenLoc){
    APESEARCH::vector<uint8_t> encodedBytes;
    uint8_t byte;

    std::cout << tokenLoc << ' ';

    while (tokenLoc > 127) {
        byte = tokenLoc | 0x80;
        encodedBytes.emplace_back(byte);
        tokenLoc >>= 7;
    }

    encodedBytes.emplace_back(tokenLoc);

    std::cout << encodedBytes.size() << std::endl;
    return encodedBytes;
}

size_t decodeDelta(APESEARCH::vector<uint8_t> deltas){
    size_t addedDeltas = 0, index = 0;

    int mask = 0x7F;

    int shift = 0;

    while(true) {
        uint8_t byte = deltas[index++];
        addedDeltas |= (byte & mask) << shift;
        if(!(~mask & byte))
            break;
        shift += 7;
    }

    return addedDeltas;
}