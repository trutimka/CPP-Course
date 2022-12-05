#include "big_integer.hpp"

int HelpLoop(int num, std::string str) {
  int tmp = 0;
  const int kCf = 10;
  for (int k = 0; k < num; ++k) {
    tmp += (str[k] - '0');
    if (k != num - 1) {
      tmp *= kCf;
    }
  }
  return tmp;
}

BigInt::BigInt(std::string str) {
  int iii = 0;
  if (str.size() >= 2 && str[0] == '-') {
    minus_ = 1;
    str = str.substr(minus_, str.size() - minus_);
    if (str[0] == '0') {
      minus_ = 0;
      numbers_.emplace_back(0);
      return;
    }
  }
  std::reverse(str.begin(), str.end());
  int num_of_strs = str.size() / kNumDigs;
  for (int j = 0; j < num_of_strs; ++j) {
    std::string tmp_s = str.substr(iii, kNumDigs);
    std::reverse(tmp_s.begin(), tmp_s.end());
    i += kNumDigs;
    numbers_.emplace_back(HelpLoop(kNumDigs, tmp_s));
  }
  int last = str.size() % kNumDigs;
  if (last != 0) {
    std::string tmp_s = str.substr(iii, last);
    std::reverse(tmp_s.begin(), tmp_s.end());
    numbers_.emplace_back(HelpLoop(last, tmp_s));
  }
  RemoveZeros();
}
BigInt::BigInt(int64_t num) {
  uint64_t temp = num;
  if (num < 0) {
    minus_ = 1;
    temp = -num;
  }
  if (temp == 0) {
    numbers_.push_back(0);
  }
  while (temp != 0) {
    numbers_.push_back(temp % kBase);
    temp /= kBase;
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
BigInt operator+(BigInt first, BigInt second) {
  first += second;
  return first;
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
BigInt operator-(BigInt first, BigInt second) {
  first -= second;
  return first;
}

BigInt& BigInt::operator*=(BigInt obj) {
  std::vector<long long> vec(numbers_.size() + obj.numbers_.size(), 0);
  for (size_t i = 0; i < numbers_.size(); i++) {
    for (size_t j = 0; j < obj.numbers_.size(); ++j) {
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
BigInt operator*(BigInt first, BigInt second) {
  first *= second;
  return first;
}
int BinSearch(int left, int right, BigInt& obj, BigInt& temp_digit) {
  while (right - left > 1) {
    int mid = l + (right - left) / 2;
    if (obj * BigInt(mid) <= temp_digit) {
      left = mid;
    } else {
      right = mid;
    }
  }
  return left;
}
BigInt& BigInt::operator/=(BigInt obj) {
  int temp_minus = (int)(minus_ != obj.minus_);
  minus_ = 0;
  obj.minus_ = 0;
  if (*this < obj) {
    *this = BigInt(0);
    return *this;
  }
  BigInt temp_digit;
  BigInt ans;
  size_t jjj = 0;
  while (temp_digit < obj) {
    temp_digit.numbers_.insert(temp_digit.numbers_.begin(),
                               numbers_[numbers_.size() - jjj - 1]);
    ++jjj;
  }
  while (jjj <= numbers_.size()) {
    int tmp = BinSearch(0, kBase, obj, temp_digit);
    ans.numbers_.push_back(tmp);
    temp_digit -= obj * tmp;
    if (j < numbers_.size()) {
      temp_digit.numbers_.insert(temp_digit.numbers_.begin(),
                                 numbers_[numbers_.size() - jjj - 1]);
    }
    ++jjj;
  }
  ans.Reverse();
  *this = ans;
  minus_ = temp_minus;
  return *this;
}
BigInt operator/(BigInt first, BigInt second) {
  first /= second;
  return first;
}

BigInt& BigInt::operator%=(BigInt obj) {
  int temp_minus = minus_;
  minus_ = 0;
  obj.minus_ = 0;
  *this = *this - ((*this / obj) * obj);
  minus_ = temp_minus;
  RemoveZeros();
  return *this;
}
BigInt operator%(BigInt first, BigInt second) {
  first %= second;
  return first;
}

bool operator==(const BigInt& k_first, const BigInt& k_second) {
  if (k_first.numbers_.size() != k_second.numbers_.size()) {
    return false;
  }
  if (k_first.minus_ != k_second.minus_) {
    return false;
  }
  for (size_t i = 0; i < k_first.numbers_.size(); ++i) {
    if (k_first.numbers_[i] != k_second.numbers_[i]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const BigInt& k_first, const BigInt& k_second) {
  return !(k_first == k_second);
}
bool operator<=(BigInt first, BigInt second) {
  return (first == second || first < second);
}
bool operator>=(BigInt first, BigInt second) { return !(first < second); }
bool operator<(BigInt first, BigInt second) {
  if (first.minus_ == 1 && second.minus_ == 0) {
    return true;
  }
  if (first.minus_ == 0 && second.minus_ == 1) {
    return false;
  }
  if (first.minus_ == 1 && second.minus_ == 1) {
    return (-second < -first);
  }
  if (first.numbers_.size() < second.numbers_.size()) {
    return true;
  }
  if (first.numbers_.size() > second.numbers_.size()) {
    return false;
  }
  if (first == second) {
    return false;
  }
  for (int i = first.numbers_.size() - 1; i >= 0; --i) {
    if (first.numbers_[i] > second.numbers_[i]) {
      return false;
    }
    if (first.numbers_[i] < second.numbers_[i]) {
      return true;
    }
  }
  return true;
}
bool operator>(BigInt first, BigInt second) { return !(first <= second); }

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
    int num = obj.numbers_[i];
    if (num == 0) {
      num = 1;
    }
    const int kCf = 10;
    while (num < obj.kBase / kCf) {
      out << '0';
      num *= kCf;
    }
    out << obj.numbers_[i];
  }
  return out;
}
std::istream& operator>>(std::istream& vin, BigInt& obj) {
  std::string str;
  vin >> str;
  BigInt num(str);
  obj = num;
  return vin;
}

size_t BigInt::Size() { return numbers_.size(); }

void BigInt::RemoveZeros() {
  if (numbers_.size() == 1) {
    return;
  }
  int tmp = numbers_.size() - 1;
  while (tmp > 0) {
    if (numbers_[i] == 0) {
      numbers_.pop_back();
    } else {
      break;
    }
    --tmp;
  }
  if (numbers_.size() == 1 && numbers_[0] == 0) {
    minus_ = 0;
    return;
  }
}

void BigInt::Reverse() {
  std::reverse(numbers_.begin(), numbers_.end());
  const int kCf = 10;
  for (size_t i = 0; i < numbers_.size() - 1; ++i) {
    bool flag = false;
    while (numbers_[i] < kBase / kCf) {
      int digit = numbers_[i + 1] % kCf;
      numbers_[i + 1] /= kCf;
      if (digit == 0) {
        flag = true;
        continue;
      }
      while (digit <= numbers_[i]) {
        digit *= kCf;
      }
      if (flag) {
        digit *= kCf;
        flag = false;
      }
      digit += numbers_[i];
      numbers_[i] = digit;
    }
  }
  RemoveZeros();
}