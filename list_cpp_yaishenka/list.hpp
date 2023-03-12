#pragma once
#include <list>

template <typename T, typename Allocator = std::allocator<T>>
using List = std::list<T, Allocator>;