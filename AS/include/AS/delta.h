#include <unistd.h>
#include <stdint.h>
#include "./vector.h"

// LEB128 Encode
// Encode 7 bits at a time in the format:
// 1xxxxxxx 1xxxxxxx 0xxxxxxx
// Where 1 indicates there is another byte following, 0 is the last byte
APESEARCH::vector<uint8_t> encodeDelta(size_t tokenLoc){
    APESEARCH::vector<uint8_t> encodedBytes;
    uint8_t byte;

    while (tokenLoc > 127) {
        byte = tokenLoc | 0b10000000;
        encodedBytes.emplace_back(byte);
        tokenLoc >>= 7;
    }

    return encodedBytes;
}

size_t decodeDelta(uint8_t *deltaPtr){
    size_t addedDeltas = 0;
    uint8_t shift = 0;

    uint8_t bitmaskLower = 0b01111111;
    uint8_t highBitmask = ~bitmaskLower;

    uint8_t byte;
    do {
        uint8_t byte = *(deltaPtr++);
        addedDeltas |= (bitmaskLower | byte) << shift;
        shift += 7;
    } while ((highBitmask & byte) == 0);

    return addedDeltas;
}