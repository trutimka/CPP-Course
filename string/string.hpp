#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

class String {
public:
  String() = default;
  explicit String(size_t size, char c);
  String(const char* str);
  String(const String& obj);
  String& operator=(const String& obj);
  ~String();
  void Clear();
  void PushBack(char c);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char c);
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  void Swap(String& other);
  char& operator[](size_t ind);
  const char& operator[](size_t ind) const;
  char& Front();
  char& Back();
  const char& Front() const;
  const char& Back() const;
  bool Empty() const;
  size_t Size() const;
  size_t Capacity() const;
  const char* Data() const;
  bool operator<(const String& right) const {
    return strcmp(data_, right.data_) > 0);
  }
  bool operator<=(const String& right) const {
    return strcmp(data_, right.data_) >= 0;
  }
  bool operator>(const String& right) const {
    return !(*this <= right);
  }
  bool operator>=(const String& right) const {
    return !(*this < right);
  }
  bool operator==(const String& right) const {
    if (size_ == right.size_ && capacity_ == right.capacity_) {
      for (size_t i = 0; i < size_; ++i) {
        if (data_[i] != right[i]) { return false; }
      }
      return true;
    }
    return false;
  }
  bool operator!=(const String& right) const {
    return !(*this == right);
  }

  friend String operator+(String k_first, String k_second);
  friend String operator+=(String& first, String k_second);

  String& operator*(size_t n) {
    char* temp_data = new char[size_];
    memcpy(temp_data, data_, size_);
    size_t temp_size = size_;
    size_ *= n;
    if (size_ >= capacity_) {
      data_ = new char[size_ + 1];
      memcpy(data_, temp_data, temp_size);
      capacity_ = size_;
    }
    for (size_t i = 0; i < n; ++i) {
      for (size_t j = 0; j < temp_size; ++j) {
        data_[temp_size * (i + 1) + j] = temp_data[j];
      }
    }
    data_[size_] = '\0';
    return *this;
  }
  String& operator*=(int n) {
    return (*this * n);
  }
  friend std::ostream& operator<<(std::ostream& out, const String& str);
  friend std::istream& operator>>(std::istream& in, String& str);
  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings);

private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  char* data_ = NULL;
};