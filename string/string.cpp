#include "string.hpp"

String::String(size_t size, char c) : size_(size), capacity_(size) {
  data_ = new char[size + 1];
  for (size_t i = 0; i < size; ++i) {
    data_[i] = c;
  }
  data_[size] = '\0';
}
String::String(const char* str) {
  capacity_ = strlen(str);
  size_ = capacity_;
  data_ = new char[capacity_ + 1];
  memcpy(data_, str, size_);
  data_[size_] = '\0';
}
String::String(const String& obj) {
  size_ = obj.size_;
  capacity_ = obj.capacity_;
  delete[] data_;
  data_ = new char[capacity_ + 1];
  memcpy(data_, obj.data_, capacity_);
  data_[capacity_] = '\0';
}
String& String::operator=(const String& obj) {
  size_ = obj.size_;
  capacity_ = obj.capacity_;
  delete[] data_;
  data_ = new char[capacity_ + 1];
  memcpy(data_, obj.data_, capacity_);
  data_[capacity_] = '\0';
  return *this;
}
String::~String() { delete[] data_; }
void String::Clear() { size_ = 0; }
void String::PushBack(char c) {
  if (size_ >= capacity_) {
    if (capacity_ == 0) {
      capacity_ = 1;
    }
    capacity_ *= 2;
    char* temp = data_;
    data_ = new char[capacity_ + 1];
    memcpy(data_, temp, size_);

    delete[] temp;
    data_[size_] = c;
    ++size_;
  } else {
    data_[size_] = c;
    ++size_;
  }
  data_[size_] = '\0';
}
void String::PopBack() {
  if (size_ != 0) {
    --size_;
    data_[size_] = '\0';
  }
}
void String::Resize(size_t new_size) {
  if (new_size > capacity_) {
    char* temp = data_;
    data_ = new char[new_size];
    memcpy(data_, temp, size_);
    delete[] temp;
    capacity_ = new_size;
  }
  for (size_t i = size_; i < new_size; ++i) {
    data_[i] = '\0';
  }
  size_ = new_size;
}
void String::Resize(size_t new_size, char c) {
  if (new_size >= capacity_) {
    char* temp = data_;
    data_ = new char[new_size + 1];
    memcpy(data_, temp, size_);
    delete[] temp;
    capacity_ = new_size;
  }
  for (size_t i = size_; i < new_size; ++i) {
    data_[i] = c;
  }
  size_ = new_size;
  data_[new_size] = '\0';
}
void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    char* temp = data_;
    data_ = new char[new_cap + 1];
    memcpy(data_, temp, size_);
    delete[] temp;
    capacity_ = new_cap;
    data_[size_] = '\0';
  }
}
void String::ShrinkToFit() {
  capacity_ = size_;
  char* temp = data_;
  data_ = new char[capacity_ + 1];
  memcpy(data_, temp, capacity_);
  data_[capacity_] = '\0';
  delete[] temp;
}
void String::Swap(String& other) {
  char* temp = data_;
  data_ = other.data_;
  other.data_ = temp;
  size_t temp_size = size_;
  size_t temp_cap = capacity_;
  size_ = other.size_;
  capacity_ = other.capacity_;
  other.size_ = temp_size;
  other.capacity_ = temp_cap;
}

char& String::operator[](size_t ind) { return data_[ind]; }
const char& String::operator[](size_t ind) const { return data_[ind]; }

char& String::Front() { return data_[0]; }
char& String::Back() { return data_[size_ - 1]; }
const char& String::Front() const { return data_[0]; }
const char& String::Back() const { return data_[size_ - 1]; }

bool String::Empty() const { return size_ == 0; }
size_t String::Size() const { return size_; }
size_t String::Capacity() const { return capacity_; }
const char* String::Data() const { return data_; }

String operator+(String k_first, String k_second) {
  String str;
  str.size_ = k_first.size_ + k_second.size_;
  str.capacity_ = str.size_;
  str.data_ = new char[str.size_ + 1];
  memcpy(str.data_, k_first.data_, k_first.size_);
  for (size_t i = k_first.size_; i < str.size_; ++i) {
    str.data_[i] = k_second.data_[i - k_first.size_];
  }
  return str;
}
String operator+=(String& first, String k_second) {
  size_t temp_size = first.size_;
  first.size_ += k_second.size_;
  if (first.size_ >= first.capacity_) {
    char* temp_data = first.data_;
    first.data_ = new char[first.size_ + 1];
    first.capacity_ = first.size_;
    memcpy(first.data_, temp_data, temp_size);
    delete[] temp_data;
  }
  for (size_t i = 0; i < k_second.size_; ++i) {
    first.data_[i + temp_size] = k_second.data_[i];
  }
  first.data_[first.size_] = '\0';
  return first;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size_; ++i) {
    out << str.data_[i];
  }
  return out;
}

std::istream& operator>>(std::istream& in, String& str) {
  char c;
  while (in.get(c)) {
    str.PushBack(c);
  }
  return in;
}

friend String operator*(String k_first, int n) {
  String str;
  str.size_ = k_first.size_ * n;
  str.capacity_ = str.size_;
  str.data_ = new char[str.size_ + 1];
  memcpy(str.data_, k_first.data_, k_first.size_);
  for (size_t i = 0; i < n; ++i) {
    for (size_t j = 0; j < k_first.size_; ++j) {
      str.data_[k_first.size_ * (i + 1) + j] = k_first.data_[j];
    }
  }
  str.data_[size_] = '\0';
  return str;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> vec;
  size_t start = 0;
  for (size_t i = start; i < size_; ++i) {
    size_t j = 0;
    for (; j < delim.size_; ++j) {
      if ((i + j) >= size_ || data_[i + j] != delim.data_[j]) {
        break;
      }
    }
    if (j == delim.size_) {
      char* temp_data = new char[i - start + 1];
      for (size_t k = start; k < i; ++k) {
        temp_data[k - start] = data_[k];
      }
      temp_data[i - start] = '\0';
      String str(temp_data);
      delete[] temp_data;
      vec.push_back(str);
      start = i + j;
    }
  }
  if (start != size_) {
    char* data = new char[size_ - start + 1];
    memcpy(data, data_ + start, size_ - start);
    data[size_ - start] = '\0';
    delete[] data;
    vec.push_back(String(data));
  }
  return vec;
}

String String::Join(const std::vector<String>& strings) {
  String str;
  if (!strings.empty()) {
    str = strings[0];
  }
  for (size_t i = 1; i < strings.size(); ++i) {
    str += (*this);
    str += strings[i];
  }
  return str;
}