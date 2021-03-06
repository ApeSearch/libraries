

#pragma once

#ifndef THREAD_H_APESEARCH
#define THREAD_H_APESEARCH

namespace APESEARCH
{

class thread
{
public:
    //class id;
    typedef pthread_t native_handle_type;

    thread() noexcept;
    template <class F, class ...Args> explicit thread(F&& f, Args&&... args);
    ~thread();

    thread(const thread&) = delete;
    thread(thread&& t) noexcept;

    thread& operator=(const thread&) = delete;
    thread& operator=(thread&& t) noexcept;

    void swap(thread& t) noexcept;

    bool joinable() const noexcept;
    void join();
    void detach();
    id get_id() const noexcept;
    native_handle_type native_handle();

    // Returns number of concurrent htreads supported by implementation
    static unsigned hardware_concurrency() noexcept { return 0; }
};

void swap(thread& x, thread& y) noexcept;

/* Omitted from our library since it's not really necessary for our purposes
class thread::id
{
public:
    id() noexcept;
};

bool operator==(thread::id x, thread::id y) noexcept;
bool operator!=(thread::id x, thread::id y) noexcept;
bool operator< (thread::id x, thread::id y) noexcept;
bool operator<=(thread::id x, thread::id y) noexcept;
bool operator> (thread::id x, thread::id y) noexcept;
bool operator>=(thread::id x, thread::id y) noexcept;
*/

template<class charT, class traits>
basic_ostream<charT, traits>&
operator<<(basic_ostream<charT, traits>& out, thread::id id);

// Certain Static Functions bounded by "this_thread"
namespace this_thread
{

//thread::id get_id() noexcept;

void yield() noexcept;

template <class Clock, class Duration>
void sleep_until(const chrono::time_point<Clock, Duration>& abs_time);

template <class Rep, class Period>
void sleep_for(const chrono::duration<Rep, Period>& rel_time);

}  // this_thread

}  // APESEARCH

#endif // THREAD_H_APESEARCH