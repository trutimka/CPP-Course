#include "big_integer.hpp"

int HelpLoop(int x, std::string s) {
  int n = 0;
  const int kCf = 10;
  for (int k = 0; k < x; ++k) {
    n += (s[k] - '0');
    if (k != x - 1) {
      n *= kCf;
    }
  }
  return n;
}

BigInt::BigInt(std::string& str) {
  int i = 0;
  if (str[0] == '-') {
    minus_ = 1;
    i = 1;
  }
  str = str.substr(i, str.size() - i);
  std::reverse(str.begin(), str.end());
  int num_of_strs = (str.size() - i) / kNumDigs;
  for (int j = 0; j < num_of_strs; ++j) {
    std::string s = str.substr(i, kNumDigs);
    std::reverse(s.begin(), s.end());
    i += kNumDigs;
    numbers_.emplace_back(HelpLoop(kNumDigs, s));
  }
  int last = (str.size() - minus_) % kNumDigs;
  if (last != 0) {
    std::string s = str.substr(i, last);
    std::reverse(s.begin(), s.end());
    numbers_.emplace_back(HelpLoop(last, s));
  }
  RemoveZeros();
}
BigInt::BigInt(int64_t num) {
  if (num < 0) {
    minus_ = 1;
    num = -num;
  }
  if (num == 0) {
    numbers_.push_back(0);
  }
  while (num != 0) {
    numbers_.push_back(num % kBase);
    num /= kBase;
  }
}
BigInt::BigInt(const BigInt& other) { *this = other; }
BigInt& BigInt::operator=(const BigInt& other) {
  minus_ = other.minus_;
  numbers_.resize(other.numbers_.size());
  for (size_t i = 0; i < other.numbers_.size(); ++i) {
    numbers_[i] = other.numbers_[i];
  }
  return *this;
}

BigInt& BigInt::operator+=(BigInt obj) {
  if (minus_ == obj.minus_) {
    int size_max = std::max(numbers_.size(), obj.numbers_.size());
    int size_min = std::min(numbers_.size(), obj.numbers_.size());
    numbers_.resize(size_max);
    for (int i = 0; i < size_min; ++i) {
      numbers_[i] += obj.numbers_[i];
    }
    for (int i = 0; i < size_max - 1; ++i) {
      if (numbers_[i] >= kBase) {
        numbers_[i] -= kBase;
        ++numbers_[i + 1];
      }
    }
    if (numbers_[size_max - 1] >= kBase) {
      numbers_[size_max - 1] -= kBase;
      numbers_.push_back(1);
    }
  } else if (minus_ == 0) {
    (*this) -= (-(obj));
  } else {
    *this = (obj - (-*this));
  }
  RemoveZeros();
  return *this;
}
BigInt BigInt::operator+(BigInt obj) {
  BigInt num = *this;
  num += obj;
  return num;
}

BigInt& BigInt::operator-=(BigInt obj) {
  if (obj.minus_ == 1) {
    return *this += (-obj);
  }
  if (minus_ == 1) {
    *this = -*this;
    *this += obj;
    *this = -*this;
    return *this;
  }
  if (*this < obj) {
    *this = obj - *this;
    *this = -*this;
    return *this;
  }
  int flag = 0;
  for (size_t i = 0; i < obj.numbers_.size() || flag != 0; ++i) {
    numbers_[i] -= flag + (i < obj.numbers_.size() ? obj.numbers_[i] : 0);
    flag = (int)(numbers_[i] < 0);
    if (flag != 0) {
      numbers_[i] += kBase;
    }
  }
  RemoveZeros();
  return *this;
}
BigInt BigInt::operator-(BigInt obj) {
  BigInt num = *this;
  num -= obj;
  return num;
}

BigInt& BigInt::operator*=(BigInt obj) {
  std::vector<long long> vec(numbers_.size() + obj.numbers_.size(), 0);
  for (size_t i = 0; i < numbers_.size(); i++) {
    for (int j = 0; j < obj.numbers_.size(); ++j) {
      vec[i + j] += (long long)(numbers_[i]) * (long long)(obj.numbers_[j]);
    }
  }
  numbers_.resize(vec.size());
  unsigned long long temp = 0;
  size_t cst = 0;
  for (size_t i = 0; i < numbers_.size(); ++i) {
    temp = cst + vec[i];
    numbers_[i] = temp % kBase;
    cst = temp / kBase;
  }
  minus_ = (int)(minus_ != obj.minus_);
  RemoveZeros();
  return *this;
}
BigInt BigInt::operator*(BigInt obj) {
  BigInt num = *this;
  num *= obj;
  return num;
}

BigInt& BigInt::operator/=(BigInt obj) {
  if (*this < obj) {
    *this = BigInt(0);
    return *this;
  }
  if (*this == obj) {
    *this = 1;
    return *this;
  }
  BigInt temp_max(2);
  BigInt temp_min(1);
  while (temp_max * obj <= *this) {
    temp_min = temp_max;
    temp_max *= 2;
  }

  while (temp_min * obj <= *this) {
    ++temp_min;
  }
  --temp_min;
  *this = temp_min;
  RemoveZeros();
  minus_ = (int)(minus_ != obj.minus_);
  return *this;
}
BigInt BigInt::operator/(BigInt obj) {
  BigInt num = *this;
  num /= obj;
  return num;
}

BigInt& BigInt::operator%=(BigInt obj) {
  minus_ = 0;
  obj.minus_ = 0;
  *this = *this - ((*this / obj) * obj);
  return *this;
}
BigInt BigInt::operator%(BigInt obj) {
  BigInt num = *this;
  num %= obj;
  return num;
}

bool BigInt::operator==(BigInt& second) {
  if (numbers_.size() != second.numbers_.size()) {
    return false;
  }
  if (minus_ != second.minus_) {
    return false;
  }
  for (size_t i = 0; i < numbers_.size(); ++i) {
    if (numbers_[i] != second.numbers_[i]) {
      return false;
    }
  }
  return true;
}
bool BigInt::operator!=(BigInt& second) { return !(*this == second); }
bool BigInt::operator<=(BigInt& second) {
  if (minus_ == 1 && second.minus_ == 0) {
    return true;
  }
  if (numbers_.size() < second.numbers_.size()) {
    return true;
  }
  if (numbers_.size() > second.numbers_.size()) {
    return false;
  }
  if (*this == second) {
    return true;
  }
  for (int i = numbers_.size() - 1; i >= 0; --i) {
    if (numbers_[i] > second.numbers_[i]) {
      return false;
    }
  }
  return true;
}
bool BigInt::operator>=(BigInt& second) { return !(*this < second); }
bool BigInt::operator<(BigInt& second) {
  if (minus_ == 1 && second.minus_ == 0) {
    return true;
  }
  if (numbers_.size() < second.numbers_.size()) {
    return true;
  }
  if (numbers_.size() > second.numbers_.size()) {
    return false;
  }
  if (*this == second) {
    return false;
  }
  for (int i = numbers_.size() - 1; i >= 0; --i) {
    if (numbers_[i] > second.numbers_[i]) {
      return false;
    }
  }
  return true;
}
bool BigInt::operator>(BigInt& second) { return !(*this <= second); }

BigInt BigInt::operator-() {
  if (numbers_.size() == 1 && numbers_[0] == 0) {
    return *this;
  }
  minus_ = (int)(minus_ == 0);
  return *this;
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}
BigInt BigInt::operator++(int) {
  BigInt temp = *this;
  ++(*this);
  return temp;
}
BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}
BigInt BigInt::operator--(int) {
  BigInt temp = *this;
  --(*this);
  return temp;
}

std::ostream& operator<<(std::ostream& out, const BigInt& obj) {
  if (obj.numbers_.empty()) {
    return out;
  }
  if (obj.minus_ == 1) {
    out << '-';
  }
  out << obj.numbers_[obj.numbers_.size() - 1];
  for (int i = obj.numbers_.size() - 2; i >= 0; --i) {
    int n = obj.numbers_[i];
    if (n == 0) {
      n = 1;
    }
    const int kCf = 10;
    while (n < obj.kBase / kCf) {
      out << '0';
      n *= kCf;
    }
    out << obj.numbers_[i];
  }
  return out;
}
std::istream& operator>>(std::istream& in, BigInt& obj) {
  std::string str;
  in >> str;
  BigInt num(str);
  obj = num;
  return in;
}

size_t BigInt::Size() { return numbers_.size(); }

void BigInt::RemoveZeros() {
  if (numbers_.size() == 1) {
    return;
  }
  int i = numbers_.size() - 1;
  while (i > 0) {
    if (numbers_[i] == 0) {
      numbers_.pop_back();
    } else {
      break;
    }
    --i;
  }
}