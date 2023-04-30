/**
 * @file smart_pointers.hpp
 * @author yaishenka
 * @date 23.04.2023
 */
#pragma once
#include <memory>

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(const Alloc& alloc, Args&&... args) {
  return std::allocate_shared<T>(alloc, std::forward<Args>(args)...);
}