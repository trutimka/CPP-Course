#pragma once

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <vector>

class String {
 public:
  String();
  explicit String(size_t size, char chr);
  String(const char* str);
  String(const String& obj);
  String& operator=(const String& obj);
  ~String();
  void Clear();
  void PushBack(char chr);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char chr);
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
  char* Data();
  friend bool operator<(const String& left, const String& right);
  friend bool operator<=(const String& left, const String& right);
  friend bool operator>(const String& left, const String& right);
  friend bool operator>=(const String& left, const String& right);
  friend bool operator==(const String& left, const String& right);
  friend bool operator!=(const String& left, const String& right);

  friend String operator+(String k_first, String k_second);
  String& operator+=(String k_second);
  friend String operator*(String k_first, int num);
  friend String operator*=(String& k_first, int num);
  friend std::ostream& operator<<(std::ostream& out, const String& str);
  friend std::istream& operator>>(std::istream& vin, String& str);
  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings);

 private:
  size_t size_ = 0;
  size_t capacity_ = 0;
  char* data_ = NULL;

  String ChangeEnd(size_t start);
};