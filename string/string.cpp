#include "string.hpp"

String::String(size_t size, char c) : size_(size), capacity_(size) {
  data_ = new char[size + 1];
  for (size_t i = 0; i < size; ++i) {
    data_[i] = c;
  }
  data_[size] = '\0';
}
String::String(char const* str) {
  capacity_ = strlen(str);
  size_ = capacity_;
  data_ = new char[capacity_ + 1];
  for (size_t i = 0; i < capacity_; ++i) {
    data_[i] = str[i];
  }
  data_[size_] = '\0';
}
String::String(const String& obj) {
  size_ = obj.size_;
  capacity_ = obj.capacity_;
  delete[] data_;
  data_ = new char[capacity_ + 1];
  for (size_t i = 0; i < capacity_; ++i) {
    data_[i] = obj.data_[i];
  }
  data_[capacity_] = '\0';
}
String& String::operator=(const String& obj) {
  size_ = obj.size_;
  capacity_ = obj.capacity_;
  delete[] data_;
  data_ = new char[capacity_ + 1];
  for (size_t i = 0; i < capacity_; ++i) {
    data_[i] = obj.data_[i];
  }
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
    if (temp != NULL) {
      memcpy(data_, temp, size_);
    }

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
  delete[] str.data_;
  str.data_ = new char[str.size_ + 1];
  memcpy(str.data_, k_first.data_, k_first.size_);
  for (size_t i = k_first.size_; i < str.size_; ++i) {
    str.data_[i] = k_second.data_[i - k_first.size_];
  }
  return str;
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

String operator*(String k_first, int n) {
  if (n != 0) {
    String str;
    str.size_ = k_first.size_ * n;
    str.capacity_ = str.size_;
    delete[] str.data_;
    str.data_ = new char[str.size_ + 1];
    memcpy(str.data_, k_first.data_, k_first.size_);
    for (int i = 0; i < n - 1; ++i) {
      for (size_t j = 0; j < k_first.size_; ++j) {
        str.data_[k_first.size_ * (i + 1) + j] = k_first.data_[j];
      }
    }
    str.data_[str.size_] = '\0';
    return str;
  }
  return String();
}

String operator*=(String& k_first, int n) {
  char* temp_data = new char[k_first.size_];
  memcpy(temp_data, k_first.data_, k_first.size_);
  size_t temp_size = k_first.size_;
  k_first.size_ *= n;
  if (k_first.size_ >= k_first.capacity_) {
    k_first.data_ = new char[k_first.size_ + 1];
    memcpy(k_first.data_, temp_data, temp_size);
    k_first.capacity_ = k_first.size_;
  }
  for (int i = 0; i < n; ++i) {
    for (size_t j = 0; j < temp_size; ++j) {
      k_first.data_[temp_size * (i + 1) + j] = temp_data[j];
    }
  }
  k_first.data_[k_first.size_] = '\0';
  delete[] temp_data;
  return k_first;
}

std::vector<String> String::Split(const String& delim) {
  if (this->Empty()) {
    return {""};
  }
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
      vec.push_back(String(temp_data));
      delete[] temp_data;
      start = i + j;
    }
  }
  if (this->ChangeEnd(start) != "-1") {
    vec.push_back(this->ChangeEnd(start));
  }
  return vec;
}

String String::ChangeEnd(size_t start) {
  if (start <= size_) {
    char* data = new char[size_ - start + 1];
    for (size_t i = start; i < size_; ++i) {
      data[i - start] = data_[i];
    }
    data[size_ - start] = '\0';
    String str(data);
    delete[] data;
    return str;
  }
  return String("-1");
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
  delete[] data_;
  return str;
}