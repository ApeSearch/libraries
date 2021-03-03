
#pragma once

#ifndef UNIQUE_PTR_APE_SEARCH
#define UNIQUE_PTR_APE_SEARCH

#include "algorithms.h"

namespace APESEARCH
{
    template<typename T>
    class unique_ptr
    {
        public:
            unique_ptr() : rawPtr(nullptr) {}

            explicit unique_ptr(T *_ptr) : rawPtr(_ptr) {}
            ~unique_ptr() { delete rawPtr; }


            // Binds to nullptr for clarity
            unique_ptr(std::nullptr_t) : rawPtr(nullptr) {}
            unique_ptr& operator=(std::nullptr_t)
               {
               reset(); // Deletes data inside
               return *this;
               } // end operator=()


            // Move constructor
            unique_ptr( unique_ptr&& other) noexcept
            {
                other.swap( *this ); // Swaps other allowing other to delete object 
            }
            // Move operator
            unique_ptr& operator=( unique_ptr&& other ) noexcept
               {
                reset(); // delete any remaining pointers
                other.swap( *this ); // swap (now nullptr) with other and inherit values
                return *this; 
                // other goes out of scope
               }

            template<typename S>
            unique_ptr(unique_ptr<S> && other )
            {
                unique_ptr<T> temp( other.release() ); // Move U's pointer into temp (U now has a nullptr )
                temp.swap( *this ); // move other's pointer into T
            }

            template<typename S>
            unique_ptr& operator=( unique_ptr<S>&& other )
               {
                unique_ptr<T> temp( other.release() );
                temp.swap( *this );
                return *this;
               } // end move operator()

               // Delete compiler generated copy constructor/operator
               unique_ptr( const unique_ptr& )=delete;
               unique_ptr& operator=(const unique_ptr& )=delete;


            // Returns raw pointer, giving up self-management.
            T* release() noexcept
               {
                T *rawData = nullptr;
                APESEARCH::swap( rawData, rawPtr );
                return rawData;
               } // end release()
            
            
            // Accessing data directly (as if it were a pointer)
            T* operator->() const { return rawPtr; }
            T& operator*() const { return *rawPtr; }


            T* get() { return rawPtr; }
            explicit operator bool() const { return rawPtr; } // to treat unique_ptr as a bool if not nullptr

            void swap( unique_ptr &uniPtr ) noexcept
               {
               APESEARCH::swap( rawPtr, uniPtr.rawPtr);
               }

            void reset()
               {
                T *temp = release();
                delete temp;
               }




        private:
            T *rawPtr;
    };
    template<typename T>
    void swap(unique_ptr<T>& lhs, unique_ptr<T>& rhs)
       {
       lhs.swap( rhs );
       }





} // end namespace


#endif // end UNIQUE_PTR_APE_SEARCH