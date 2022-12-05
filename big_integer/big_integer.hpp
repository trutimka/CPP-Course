#pragma once

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 public:
  BigInt() = default;
  explicit BigInt(std::string str);
  BigInt(int64_t num);
  BigInt(const BigInt& other);
  BigInt& operator=(const BigInt& other);
  friend BigInt operator+(BigInt first, BigInt second);
  friend BigInt operator-(BigInt first, BigInt second);
  friend BigInt operator*(BigInt first, BigInt second);
  friend BigInt operator/(BigInt first, BigInt second);
  friend BigInt operator%(BigInt first, BigInt second);
  BigInt& operator+=(BigInt obj);
  BigInt& operator-=(BigInt obj);
  BigInt& operator*=(BigInt obj);
  BigInt& operator/=(BigInt obj);
  BigInt& operator%=(BigInt obj);

  friend bool operator==(const BigInt& k_first, const BigInt& k_second) const;
  friend bool operator!=(const BigInt& k_first, const BigInt& k_second) const;
  friend bool operator<=(BigInt first, BigInt second);
  friend bool operator>=(BigInt first, BigInt second);
  friend bool operator<(BigInt first, BigInt second);
  friend bool operator>(BigInt first, BigInt second);

  BigInt operator-();
  BigInt& operator++();
  BigInt operator++(int);
  BigInt& operator--();
  BigInt operator--(int);

  friend std::ostream& operator<<(std::ostream& out, const BigInt& obj);
  friend std::istream& operator>>(std::istream& in, BigInt& obj);

  size_t Size();

 private:
  std::vector<int> numbers_;
  int minus_ = 0;
  const int kBase = 1e9;
  const int kNumDigs = 9;
  void RemoveZeros();
  void Reverse();
};