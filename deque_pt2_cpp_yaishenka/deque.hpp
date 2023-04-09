/**
 * @file deque.hpp
 * @author yaishenka
 * @date 05.01.2023
 */
#include <deque>

template <typename T, typename Allocator = std::allocator<T>>
using Deque = std::deque<T, Allocator>;