
#pragma once
#ifndef AS_UNIQUE_MMAP_H
#define AS_UNIQUE_MMAP_H

#include "File.h"
#include <cassert>
#include <cstring>
#include <cstdint>
#include <exception>
#include <cerrno> // for errno
#include <string>
#include <sys/mman.h> // mmap
#include "algorithms.h"

//------------------------------------------------------------------------------------------------
//
//                                  unique_mmap Definition 
//
//------------------------------------------------------------------------------------------------
/*
 * A RAII for mmap. It ensures that at the end of the lifetime of this object, an mmapped file is guaranteed to
 * deallocate and free any acquired resources.
 * 
 *         map: Pointer that holds the memory mapped region.
 * bytesMapped: The amount of bytes that was mapped into memory. Mainly used for calling munmap so that
 *              the object knows how many bytes is needed to be freed.
 *       file: A file object. If such an object is passed into the constructor, unique_mmap becomes
 *              responsible for the deallocation ( closing of the file object ).
*/


class unique_mmap
   {
   void *map;
   size_t bytesMapped;
   APESEARCH::File file;

public:
   class failure : public std::exception
      {
      friend class unique_mmap;
   protected:
      int error_number;
      int error_offset;
      std::string error_message;

   public:
      explicit failure( const std::string& msg, int err_num, int err_off )
         : error_number( err_num ), error_offset( err_off ), error_message( msg ) {}

      explicit failure( const std::string&& msg, int err_num, int err_off )
         : error_number( err_num ), error_offset( err_off ), error_message( std::move( msg ) ) {}
      
      virtual ~failure() throw () {}

      virtual const char *what() const throw()
         {
         return error_message.c_str();
         }

      virtual int getErrorNumber() const throw() 
         {
         return error_number;
         }

      virtual int getErrorOffset() const throw()
         {
         return error_offset;
         }

      static constexpr int LENGTHZERO = 100; 
      };
   
   unique_mmap() : map ( nullptr ) {}

   /*
    * FYI:
    * addr: asks which address in which one would like to map pointer to.
    *       If NULL, the kernel chooses the address (page aligned ofc)
    *       in which to linked the mmap to.
    *       O.W., the kenrel takes it as a hint and places the mapping in
    *       a page boundary, ( which is alwys within the arena ( linux specifies:
    *       /proc/sys/vm/mmap_min_addr )).
    *       It may be the cas tha a mapping already exists in which the kernel
    *       decides to pick an address at its own discretion.
    * 
    * length: The amount of bytes after offset in which mmap initializes the values
    *       ,i.e. creating an appropriate virtual page. ( likely initialized as non-resident ).
    * 
    * prot: Describes desired memory protection of mapping ( of which must not conflict with open mode 
    *       of file ).
    *       Following: PROT_EXEC: Pages may be executed ( forked )
    *       Prot_READ: pages may be read
    *       PROT_WRITE: Pages may be written
    *       PROT_NONE: Pages may be not accessed
    * 
    * flags: Determines whether updates to mapping are visible to other processes mapping same region
    *        and whether updates are carried through to underlying file (interesting ).
    * 
    * offset: The location in file in which mmap is started from
    *        ( this must be a multiple of page size which can be returned by sysconf(_SC_PAGE_SIZE)
   */
   unique_mmap( void *addr, std::size_t length, int prot, int flags, int fd, off_t offset ) : bytesMapped( length )
      {
      if ( !length )
         {
         close( fd );
         throw failure( " Error in unique_mmap constructor. length must be greater than zero! ", 
            failure::LENGTHZERO, 0 );
         } // end if
      map = mmap( addr, length, prot, flags, fd, offset );

      if ( map == MAP_FAILED )
         {
         close( fd );
         perror( "Error mmapping file" );
         throw failure( " Mmapping failed... ", errno, 0 );
         }
      }

   // Give a pointer anywhere in virtual address space...
   unique_mmap( std::size_t length, int prot, int flags, int fd, off_t offset ) : 
      unique_mmap( 0, length, prot, flags, fd, offset ) {}

   unique_mmap( void *addr, std::size_t length, int prot, int flags, off_t offset, APESEARCH::File&& fd )
      : bytesMapped( length ), file( std::move( fd ) )
      {
      if ( !length )
         {
         throw failure( " Error in unique_mmap constructor. length must be greater than zero! ", 
            failure::LENGTHZERO, 0 );
         } // end if
      map = mmap( addr, length, prot, flags, file.getFD(), offset );

      if ( map == MAP_FAILED )
         {
         perror( "Error mmapping file" );
         throw failure( " Mmapping failed... ", errno, 0 );
         }
      }
   
   unique_mmap( unique_mmap&& other ) : map( other.map ), bytesMapped( other.bytesMapped ), file( std::move( other.file ) )
      {
      other.map = nullptr;
      }

   unique_mmap& operator=( unique_mmap&& other )
      {
      swap( other );
      return *this;
      }

   unique_mmap( const unique_mmap& ) = delete;
   unique_mmap& operator=( const unique_mmap& ) = delete;

   void swap( unique_mmap& other )
      {
      APESEARCH::swap( map, other.map );
      APESEARCH::swap( bytesMapped, other.bytesMapped );
      APESEARCH::swap( file, other.file );
      }

   /* 
    *   Though the region is automatically unmapped when the process is terminated.  On
    *   the other hand, closing the file descriptor does not unmap the
    *   region.
    *   Note: this destructor needs to be made allowalbe to throw in case munmap fails.
   */
   ~unique_mmap( ) noexcept(false)
      {
      if ( map &&  munmap( map, bytesMapped ) == -1 )
         {
         perror( "err un-mapping the file " );
         throw failure( "error unmapping file", errno, 0 );
         } // end if
      } // end ~unique_mmap(0)

   inline void *get() const
      {
      return map;
      } // end getPointer()
   };


#endif