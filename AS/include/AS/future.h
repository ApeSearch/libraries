
#pragma once

#ifndef FUTURE_H_APESEARCH
#define FUTURE_H_APESEARCH

#include <functional>
#include <exception> // for std::excpetion_ptr
#include "condition_variable.h" // for APESEARCH::condition_variable
#include "mutex.h" // for APESERACH::mutex
#include "unique_ptr.h"
#include <utility> // for std::forward
#include <assert.h>

namespace APESEARCH
{

enum class future_errc
{
    future_already_retrieved = 1,
    promise_already_satisfied,
    no_state,
    broken_promise
};

enum class launch
{
    async = 1,
    deferred = 2,
    any = async | deferred
};

enum class future_status
{
    ready,
    timeout,
    deferred
};

template <> struct is_error_code_enum<future_errc> : public true_type { };
error_code make_error_code(future_errc e) noexcept;
error_condition make_error_condition(future_errc e) noexcept;

const error_category& future_category() noexcept;

class future_error
    : public logic_error
{
public:
    future_error(error_code ec);  // exposition only
    explicit future_error(future_errc); // C++17
    const error_code& code() const noexcept;
    const char*       what() const noexcept;
};

template <class R>
class promise
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) noexcept;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) noexcept;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) noexcept;

    // retrieving the result
    future<R> get_future();

    // setting the result
    void set_value(const R& r);
    void set_value(R&& r);
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit(const R& r);
    void set_value_at_thread_exit(R&& r);
    void set_exception_at_thread_exit(exception_ptr p);
};

template <class R>
class promise<R&>
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) noexcept;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) noexcept;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) noexcept;

    // retrieving the result
    future<R&> get_future();

    // setting the result
    void set_value(R& r);
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit(R&);
    void set_exception_at_thread_exit(exception_ptr p);
};

template <>
class promise<void>
{
public:
    promise();
    template <class Allocator>
        promise(allocator_arg_t, const Allocator& a);
    promise(promise&& rhs) noexcept;
    promise(const promise& rhs) = delete;
    ~promise();

    // assignment
    promise& operator=(promise&& rhs) noexcept;
    promise& operator=(const promise& rhs) = delete;
    void swap(promise& other) noexcept;

    // retrieving the result
    future<void> get_future();

    // setting the result
    void set_value();
    void set_exception(exception_ptr p);

    // setting the result with deferred notification
    void set_value_at_thread_exit();
    void set_exception_at_thread_exit(exception_ptr p);
};

template <class R> void swap(promise<R>& x, promise<R>& y) noexcept;

template <class R, class Alloc>
    struct uses_allocator<promise<R>, Alloc> : public true_type {};

template <class R>
class future
{
public:
    future() noexcept;
    future(future&&) noexcept;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) noexcept;
    shared_future<R> share() noexcept;

    // retrieving the value
    R get();

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class R>
class future<R&>
{
public:
    future() noexcept;
    future(future&&) noexcept;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) noexcept;
    shared_future<R&> share() noexcept;

    // retrieving the value
    R& get();

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <>
class future<void>
{
public:
    future() noexcept;
    future(future&&) noexcept;
    future(const future& rhs) = delete;
    ~future();
    future& operator=(const future& rhs) = delete;
    future& operator=(future&&) noexcept;
    shared_future<void> share() noexcept;

    // retrieving the value
    void get();

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};



template <class> class packaged_task; // undefined

template <class R, class... ArgTypes>
class packaged_task<R(ArgTypes...)>
{
public:
    typedef R result_type; // extension

private:
    std::function<result_type(ArgTypes...)> func;
    promise<result_type> pr;

    // construction and destruction
    packaged_task() noexcept;
    template <class F>
        explicit packaged_task(F&& f);
    template <class F, class Allocator>
        packaged_task(allocator_arg_t, const Allocator& a, F&& f);
    ~packaged_task();

    // no copy
    packaged_task(const packaged_task&) = delete;
    packaged_task& operator=(const packaged_task&) = delete;

    // move support
    packaged_task(packaged_task&& other) noexcept;
    packaged_task& operator=(packaged_task&& other) noexcept;
    void swap(packaged_task& other) noexcept;

    bool valid() const noexcept;

    // result retrieval
    future<R> get_future();

    // execution
    void operator()(ArgTypes... );
    void make_ready_at_thread_exit(ArgTypes...);

    void reset();
};

template <class R>
  void swap(packaged_task<R(ArgTypes...)&, packaged_task<R(ArgTypes...)>&) noexcept;

template <class R, class Alloc> struct uses_allocator<packaged_task<R>, Alloc>;



template<class T>
class shared_state
{
   APESEARCH::unique_ptr<T> value; // Holds the value which will be received
   std::exception_ptr exceptions;
   mutable APESEARCH::mutex stateMut;
   mutable APESEARCH::condition_variable cv; // Need to kick future into action
   unsigned state;
   //bool promise_valid, future_valid;
public:
   enum
   {
    constructed = 1,
    future_attached = 2,
    ready = 4,
    deferred = 8
   };

   bool has_value() const
      {
        return ( state & constructed ) || ( exceptions );
      } // end has_value
   void attach_future()
      {
       APESEARCH::unique_lock<APESEARCH::mutex> lk( stateMut );
       if ( state & future_attached )
          throw std::runtime_error("Future already obtained");
       
      }
    // Assume moveable object
   void set_value( T&& arg )
      {
        APESEARCH::unique_lock<APESEARCH::mutex> lk( stateMut );
        if ( ( state & constructed ) || ( exceptions ) )
            throw std::runtime_error( "promise has already been satisfied (routine has already finished) ");
        ::new( value.get() ) T( std::forward<T>( arg) );
        state |= constructed | ready;
        cv.notify_one(); // Notify a potentially wating future
      } // end set_value
    T move_value()
       {
       APESEARCH::unique_lock<APESEARCH::mutex> lk( stateMut );
       get_value_helper( lk ); 
       return std::move( *reinterpret_cast<T*>( value.get() ) );
       }
    T& copy_value() // Required for future<T&>
       {
       APESEARCH::unique_lock<APESEARCH::mutex> lk( stateMut );
       get_value_helper( lk );
       return *reinterpret_cast<T*>( value.get() ); 
       }

private:
    void get_value_helper(APESEARCH::unique_lock<APESEARCH::mutex> &lk)
       {
        assert( lk.owns_lock() );
       auto pred = []() -> bool { return !( state & constructed ) || ( state & deferred ) || exceptions; };
       cv.wait( lk, pred );
       if ( exceptions )
          std::rethrow_exception( exceptions );
       } // end get_value_helper()


};


/*

template <class _Rp>
_Rp
future<_Rp>::get()
{
    unique_ptr<__shared_count, __release_shared_count> __(__state_);
    __assoc_state<_Rp>* __s = __state_;
    __state_ = nullptr;
    return __s->move();
}

template <class _Rp>
_Rp&
future<_Rp&>::get()
{
    unique_ptr<__shared_count, __release_shared_count> __(__state_);
    __assoc_state<_Rp&>* __s = __state_;
    __state_ = nullptr;
    return __s->copy();
}

*/


}  // APESEARCH

/*

template <class R>
class shared_future
{
public:
    shared_future() noexcept;
    shared_future(const shared_future& rhs);
    shared_future(future<R>&&) noexcept;
    shared_future(shared_future&& rhs) noexcept;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) noexcept;

    // retrieving the value
    const R& get() const;

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class R>
class shared_future<R&>
{
public:
    shared_future() noexcept;
    shared_future(const shared_future& rhs);
    shared_future(future<R&>&&) noexcept;
    shared_future(shared_future&& rhs) noexcept;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) noexcept;

    // retrieving the value
    R& get() const;

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <>
class shared_future<void>
{
public:
    shared_future() noexcept;
    shared_future(const shared_future& rhs);
    shared_future(future<void>&&) noexcept;
    shared_future(shared_future&& rhs) noexcept;
    ~shared_future();
    shared_future& operator=(const shared_future& rhs);
    shared_future& operator=(shared_future&& rhs) noexcept;

    // retrieving the value
    void get() const;

    // functions to check state
    bool valid() const noexcept;

    void wait() const;
    template <class Rep, class Period>
        future_status
        wait_for(const chrono::duration<Rep, Period>& rel_time) const;
    template <class Clock, class Duration>
        future_status
        wait_until(const chrono::time_point<Clock, Duration>& abs_time) const;
};

template <class F, class... Args>
  future<typename result_of<typename decay<F>::type(typename decay<Args>::type...)>::type>
  async(F&& f, Args&&... args);

template <class F, class... Args>
  future<typename result_of<typename decay<F>::type(typename decay<Args>::type...)>::type>
  async(launch policy, F&& f, Args&&... args);

template <class> class packaged_task; // undefined

*/


#endif // end FUTURE_H_APESEARCH