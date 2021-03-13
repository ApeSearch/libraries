#include <netdb.h> // For addrinfo
#include <stdio.h> // perror
#include <string.h> // memset
#pragma once

// RAII wrapper for addrinfo struct
struct Address {
    Address() {}

    // ai_family specifies the address family: which restricts the kind of addresses to the same type.
    // i.ei AF_UNIX are UNIX sockets, AF_IPS, IPX. Bluetooh has AF_BLUETOOTH.
    // AF_INET6 is for v6 addresses (Use AF_INET as it's the safest option)
    Address(char *Host, char *Port, int addrFamily = AF_INET, 
          int sockType = SOCK_STREAM, int transportProtocol = IPPROTO_TCP) : valid( true ) 
    {
        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family = addrFamily;
        hints.ai_socktype = sockType;
        hints.ai_protocol = transportProtocol;
        // If this does not return 0, the DNS was unable to resolve the hostname:port
        // and so we can assume it's invalid
        if (getaddrinfo( Host, (Port == nullptr) ? "80": Port, &hints, &info ) != 0 ) 
        {
            //TODO use exception handling
            perror("Error inside Address Constructor occured");
            valid = false;
        } // end if
        head = info;
    }

    ~Address() 
    {
        if (valid)
            freeaddrinfo( head );
    }

    struct addrinfo *info, *head, hints ;
    bool valid;

    // PrintAddressInfo as defined in class
    //Used only in regression testing methods
    //friend std::ostream &operator<<( std::ostream &os, const Address & addr);
}; // end Address