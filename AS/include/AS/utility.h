

#ifndef UTILITIES_H_APE_SEARCH
#define UTILITIES_H_APE_SEARCH

#include <utility>

namespace APESEARCH
{

template<typename First, typename Second>
struct pair {
    pair() = default;
    pair(First _first, Second _second) : first_obj(_first), second_obj(_second) {}
    pair(First _first) : first_obj(_first) {} // If we don't care about the second obj
    pair make_pair(First&& _first, Second&& _second)
    {
       return pair{std::forward<First>(_first), std::forward<Second>(_second)};
    }
    First& first() {
       return first_obj;
    } // end first()
    Second& second() {
       return second_obj;
    } // end second()

    First first_obj;
    Second second_obj;
};

} // end namespace APESEARCH

#endif // end UTILITIES_H_APE_SEARCH