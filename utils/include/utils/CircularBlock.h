#pragma once
//https://en.wikipedia.org/wiki/C_preprocessor
//https://en.wikipedia.org/wiki/Pragma_once
//https://en.wikipedia.org/wiki/Include_guard

#include "utils/Common.h"

#include <atomic> //http://www.cplusplus.com/reference/atomic/atomic/
#include <array> //http://en.cppreference.com/w/cpp/container/array

using namespace common;

// !! Only One publisher / One Listener !!
// Could consider yielding instead of spin loop
//https://www.quora.com/What-is-the-difference-between-message-queue-pattern-and-publish-subscribe

template <typename T, size_t _BlockCapacity = 262'144>
class CircularBlock
{
public:
    static constexpr size_t CAPACITY = _BlockCapacity; //http://en.cppreference.com/w/cpp/language/constexpr
    //http://en.cppreference.com/w/cpp/language/static_assert
    
    //The suggested code for checking if a number is power of 2 is slightly different : https://stackoverflow.com/questions/600293/how-to-check-if-a-number-is-a-power-of-2
    static_assert(((CAPACITY > 0) && ((CAPACITY & (~CAPACITY + 1)) == CAPACITY)), "Block capacity must be a positive power of 2");
    
    static auto capacity()
    {
        return CAPACITY;
    }
    
    //http://www.bogotobogo.com/cplusplus/C11/C11_default_delete_specifier.php
    //https://docs.microsoft.com/en-us/cpp/cpp/explicitly-defaulted-and-deleted-functions
    CircularBlock() = default;
    ~CircularBlock() = default;
    CircularBlock(const CircularBlock&) = delete;
    CircularBlock& operator=(const CircularBlock&) = delete;
    
    void fill(T&& data)
    {
        while (size_.load(std::memory_order::memory_order_acquire) >=  CAPACITY-1) // spin loop
            if (unlikely(dontSpin_)) return;
        if (++last_ == CAPACITY) last_ = 0;
        array_[last_] = std::forward<T>(data);
        size_.fetch_add(1, std::memory_order::memory_order_release);
    }
    
    auto&& empty()
    {
        static T nodata;
        while (size_.load(std::memory_order::memory_order_acquire) == 0) // spin loop
            if (unlikely(dontSpin_)) return std::move(nodata);
        if (++first_ == CAPACITY) first_ = 0;
        size_.fetch_sub(1, std::memory_order::memory_order_release);
        return std::move(array_[first_]);
    }
    
    void dontSpin() { dontSpin_ = true; }

protected:
    bool dontSpin_ = false;
    size_t first_ = CAPACITY-1;
    size_t last_ = CAPACITY-1;
    std::atomic<size_t> size_{0UL};
    
    std::array<T, CAPACITY> array_;
};

