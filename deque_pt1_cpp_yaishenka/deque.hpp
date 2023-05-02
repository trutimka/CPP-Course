
#pragma once
#include <cstring>
#include <iostream>
#include <vector>

template <typename T>
class Deque {
 public:
  Deque() = default;
  Deque(size_t count);
  Deque(size_t count, const T& value);
  ~Deque();
  Deque(const Deque<T>& other);
  Deque<T>& operator=(const Deque<T>& other);
  size_t size() const noexcept;
  bool empty() const noexcept;
  T& operator[](size_t index);
  const T& operator[](size_t index) const;
  T& at(size_t index);
  const T& at(size_t index) const;
  void push_back(const T& value);
  void push_front(const T& value);
  void pop_back();
  void pop_front();

  template <bool IsConst>
  class common_iterator;
  template <typename Iter>
  class rev_iterator;
  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = rev_iterator<common_iterator<false>>;
  using const_reverse_iterator = rev_iterator<common_iterator<true>>;

  iterator begin() { return iterator(&arr_, temp_vec_str_, temp_str_); }
  iterator end() { return iterator(&arr_, temp_vec_fnsh_, temp_fnsh_); }
  const_iterator cbegin() const {
    return const_iterator(&arr_, temp_vec_str_, temp_str_);
  }
  const_iterator cend() const {
    return const_iterator(&arr_, temp_vec_fnsh_, temp_fnsh_);
  }
  reverse_iterator rbegin() { return reverse_iterator(end() - 1); }
  reverse_iterator rend() { return reverse_iterator(begin() - 1); }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend() - 1);
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin() - 1);
  }
  void insert(iterator iter, const T& value);
  void erase(iterator iter);

 private:
  void set_array(std::vector<T*>& new_vec, const Deque<T>& other, size_t vec,
                 size_t ind);
  void set_array(std::vector<T*>& new_vec, size_t count, const T& value,
                 size_t cnt);
  void clean_array(std::vector<T*>& arr, size_t temp_str, size_t temp_vec_str,
                   size_t ind, size_t vec);
  std::vector<T*> arr_;
  static const size_t kConstCnt = 1e5;
  size_t size_ = 0;
  size_t capacity_ = 0;
  size_t temp_vec_str_ = 0;  // первый вектор, в котором есть элементы
  size_t temp_vec_fnsh_ = 0;  // последний вектор, в котором есть элементы
  size_t temp_str_ = 0;  // первый элемент (указывает на элемент)
  size_t temp_fnsh_ =
      0;  // последний элемент (указывает на следующую ячейку после всего дека)
};

template <typename T>
void Deque<T>::clean_array(std::vector<T*>& arr, size_t temp_str,
                           size_t temp_vec_str, size_t ind, size_t vec) {
  for (size_t i = temp_str; i < kConstCnt; ++i) {
    (arr[temp_vec_str] + i)->~T();
  }
  for (size_t i = temp_vec_str + 1; i < vec; ++i) {
    for (size_t j = 0; j < kConstCnt; ++j) {
      (arr[i] + j)->~T();
    }
  }
  for (size_t i = 0; i < ind; ++i) {
    (arr[vec] + i)->~T();
  }
}
template <typename T>
void Deque<T>::set_array(std::vector<T*>& new_vec, const Deque<T>& other,
                         size_t vec, size_t ind) {
  try {
    if (other.temp_vec_str_ == other.temp_vec_fnsh_) {
      for (ind; ind < other.temp_fnsh_; ++ind) {
        new (new_vec[vec] + ind) T(other.arr_[vec][ind]);
      }
    } else {
      for (ind; ind < kConstCnt; ++ind) {
        new (new_vec[vec] + ind) T(other.arr_[vec][ind]);
      }
      ++vec;
      for (vec; vec < other.temp_vec_fnsh_; ++vec) {
        for (ind = 0; ind < kConstCnt; ++ind) {
          new (new_vec[vec] + ind) T(other.arr_[vec][ind]);
        }
      }
      for (ind = 0; ind < other.temp_fnsh_; ++ind) {
        new (new_vec[vec] + ind) T(other.arr_[vec][ind]);
      }
    }
  } catch (...) {
    if (vec == other.temp_vec_str_) {
      for (size_t i = other.temp_str_; i < ind; ++i) {
        (new_vec[other.temp_vec_str_] + i)->~T();
      }
    } else {
      clean_array(new_vec, other.temp_str_, other.temp_vec_str_, ind, vec);
    }
    throw;
  }
}

template <typename T>
void Deque<T>::set_array(std::vector<T*>& new_vec, size_t count, const T& value,
                         size_t cnt) {
  size_t temp_vec = 0;
  size_t temp_ind = 0;
  try {
    if (count < kConstCnt) {
      for (temp_ind = 0; temp_ind < count; ++temp_ind) {
        new (new_vec[1] + temp_ind) T(value);
      }
    } else {
      for (temp_vec = 1; temp_vec < new_vec.size() - 2; ++temp_vec) {
        for (temp_ind = 0; temp_ind < kConstCnt; ++temp_ind) {
          new (new_vec[temp_vec] + temp_ind) T(value);
        }
      }
      for (temp_ind = 0; temp_ind < count - kConstCnt * (cnt - 1); ++temp_ind) {
        new (new_vec[temp_vec] + temp_ind) T(value);
      }
    }
  } catch (...) {
    if (count < kConstCnt) {
      for (size_t i = temp_str_; i < temp_ind; ++i) {
        (new_vec[temp_vec_str_] + i)->~T();
      }
    } else {
      clean_array(new_vec, temp_str_, temp_vec_str_, temp_ind, temp_vec);
    }
    throw;
  }
}
template <typename T>
Deque<T>::Deque(size_t count) {
  size_t cnt = count / kConstCnt;
  if (count % kConstCnt != 0 || cnt == 0) {
    ++cnt;
  }
  arr_.resize(cnt + 2);
  try {
    for (size_t i = 0; i < arr_.size(); ++i) {
      arr_[i] = reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]);
      memset(arr_[i], T(), kConstCnt * sizeof(T));
    }
  } catch (...) {
    for (size_t i = 0; i < arr_.size(); ++i) {
      delete[] reinterpret_cast<int8_t*>(arr_[i]);
    }
    throw;
  }
  capacity_ = kConstCnt * arr_.size();
  temp_vec_str_ = 1;
  temp_vec_fnsh_ = arr_.size() - 2;
  size_ = count;
  temp_str_ = 0;
  temp_fnsh_ = count % kConstCnt;
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value) {
  size_t cnt = (count % kConstCnt != 0 || (count / kConstCnt) == 0)
                   ? (count / kConstCnt + 1)
                   : (count / kConstCnt);
  arr_.resize(cnt + 2);
  for (size_t i = 0; i < arr_.size(); ++i) {
    arr_[i] = reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]);
  }
  try {
    set_array(arr_, count, value, cnt);
  } catch (...) {
    for (size_t i = 0; i < arr_.size(); ++i) {
      delete[] reinterpret_cast<int8_t*>(arr_[i]);
    }
    throw;
  }
  capacity_ = kConstCnt * arr_.size();
  temp_vec_str_ = 1;
  temp_vec_fnsh_ = arr_.size() - 2;
  temp_fnsh_ = count - kConstCnt * (cnt - 1);
  size_ = count;
}

template <typename T>
Deque<T>::~Deque() {
  if (temp_vec_fnsh_ == temp_vec_str_) {
    for (size_t i = temp_str_; i < temp_fnsh_; ++i) {
      (arr_[temp_vec_str_] + i)->~T();
    }
  } else {
    for (size_t i = temp_str_; i < kConstCnt; ++i) {
      (arr_[temp_vec_str_] + i)->~T();
    }
    for (size_t i = temp_vec_str_ + 1; i < temp_vec_fnsh_; ++i) {
      for (size_t j = 0; j < kConstCnt; ++j) {
        (arr_[i] + j)->~T();
      }
    }
    for (size_t i = 0; i < temp_fnsh_; ++i) {
      (arr_[temp_vec_fnsh_] + i)->~T();
    }
  }
  for (size_t i = 0; i < arr_.size(); ++i) {
    delete[] reinterpret_cast<int8_t*>(arr_[i]);
  }
}

template <typename T>
Deque<T>::Deque(const Deque<T>& other) {
  size_ = other.size_;
  capacity_ = other.capacity_;
  temp_str_ = other.temp_str_;
  temp_fnsh_ = other.temp_fnsh_;
  temp_vec_str_ = other.temp_vec_str_;
  temp_vec_fnsh_ = other.temp_vec_fnsh_;
  arr_.resize(other.arr_.size());
  for (size_t i = 0; i < arr_.size(); ++i) {
    arr_[i] = reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]);
  }
  try {
    set_array(arr_, other, other.temp_vec_str_, other.temp_str_);
  } catch (...) {
    for (size_t i = 0; i < arr_.size(); ++i) {
      delete[] reinterpret_cast<int8_t*>(arr_[i]);
    }
    throw;
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  if (&other == this) {
    return *this;
  }
  std::vector<T*> new_arr(other.arr_.size());
  for (size_t i = 0; i < new_arr.size(); ++i) {
    new_arr[i] = reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]);
    // memcpy(new_arr[i], other.arr_[i], kConstCnt * sizeof(T));
  }
  try {
    set_array(new_arr, other, other.temp_vec_str_, other.temp_str_);
  } catch (...) {
    for (size_t i = 0; i < arr_.size(); ++i) {
      delete[] reinterpret_cast<int8_t*>(new_arr[i]);
    }
    throw;
  }
  for (size_t i = 0; i < arr_.size(); ++i) {
    for (size_t j = 0; j < kConstCnt; ++j) {
      (arr_[i] + j)->~T();
    }
  }
  for (size_t i = 0; i < arr_.size(); ++i) {
    delete[] reinterpret_cast<int8_t*>(arr_[i]);
  }
  size_ = other.size_;
  capacity_ = other.capacity_;
  temp_str_ = other.temp_str_;
  temp_fnsh_ = other.temp_fnsh_;
  temp_vec_str_ = other.temp_vec_str_;
  temp_vec_fnsh_ = other.temp_vec_fnsh_;
  arr_.resize(other.arr_.size());
  for (size_t i = 0; i < arr_.size(); ++i) {
    arr_[i] = new_arr[i];
  }
  return *this;
}

template <typename T>
bool Deque<T>::empty() const noexcept {
  return size_ == 0;
}

template <typename T>
size_t Deque<T>::size() const noexcept {
  return size_;
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
  size_t full_vec = index / kConstCnt;
  if (index % kConstCnt + temp_str_ >= kConstCnt) {
    ++full_vec;
    return (arr_[temp_vec_str_ + full_vec]
                [index % kConstCnt + temp_str_ - kConstCnt]);
  }
  return (arr_[temp_vec_str_ + full_vec][index % kConstCnt + temp_str_]);
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size_) {
    throw std::out_of_range("");
  }
  return (*this)[index];
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  size_t full_vec = index / kConstCnt;
  if (index % kConstCnt + temp_str_ >= kConstCnt) {
    ++full_vec;
    return (arr_[temp_vec_str_ + full_vec]
                [index % kConstCnt + temp_str_ - kConstCnt]);
  }
  return (arr_[temp_vec_str_ + full_vec][index % kConstCnt + temp_str_]);
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size_) {
    throw std::out_of_range("");
  }
  return (*this)[index];
}

template <typename T>
void Deque<T>::push_back(const T& value) {
  if (size_ < capacity_ &&
      (temp_vec_fnsh_ != arr_.size() - 1 || temp_fnsh_ != kConstCnt)) {
    bool is_transfered = false;
    if (temp_fnsh_ == kConstCnt) {
      temp_fnsh_ = 0;
      ++temp_vec_fnsh_;
      is_transfered = true;
    }
    try {
      new (arr_[temp_vec_fnsh_] + temp_fnsh_) T(value);
    } catch (...) {
      if (is_transfered) {
        --temp_vec_fnsh_;
        temp_fnsh_ = kConstCnt;
      }
      throw;
    }
    ++temp_fnsh_;
    ++size_;
  } else {
    try {
      arr_.push_back(reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]));
      temp_fnsh_ = 0;
      if (capacity_ != 0) {
        ++temp_vec_fnsh_;
      }
      new (arr_[temp_vec_fnsh_] + temp_fnsh_) T(value);
    } catch (...) {
      delete[] reinterpret_cast<int8_t*>(arr_[temp_vec_fnsh_]);
      temp_fnsh_ = kConstCnt;
      --temp_vec_fnsh_;
      arr_.pop_back();
      throw;
    }
    ++temp_fnsh_;
    capacity_ += kConstCnt;
    ++size_;
  }
}

template <typename T>
void Deque<T>::push_front(const T& value) {
  if (size_ < capacity_ && (temp_str_ != 0 || temp_vec_str_ != 0)) {
    bool is_transfered = false;
    if (temp_str_ == 0) {
      temp_str_ = kConstCnt;
      --temp_vec_str_;
      is_transfered = true;
    }
    --temp_str_;
    try {
      new (arr_[temp_vec_str_] + temp_str_) T(value);
    } catch (...) {
      if (is_transfered) {
        ++temp_vec_str_;
        temp_str_ = 0;
      }
      throw;
    }
    ++size_;
  } else {
    try {
      arr_.insert(arr_.begin(),
                  reinterpret_cast<T*>(new int8_t[kConstCnt * sizeof(T)]));
      temp_str_ = kConstCnt - 1;
      new (arr_[temp_vec_str_] + temp_str_) T(value);
    } catch (...) {
      delete[] reinterpret_cast<int8_t*>(arr_[temp_vec_str_]);
      arr_.erase(arr_.begin());
      temp_str_ = 0;
      throw;
    }
    ++temp_vec_fnsh_;
    capacity_ += kConstCnt;
    ++size_;
  }
}

template <typename T>
void Deque<T>::pop_back() {
  if (size_ != 0) {
    (arr_[temp_vec_fnsh_] + temp_fnsh_ - 1)->~T();
    --temp_fnsh_;
    --size_;
    if (temp_fnsh_ == 0) {
      --temp_vec_fnsh_;
      temp_fnsh_ = kConstCnt;
    }
  }
}

template <typename T>
void Deque<T>::pop_front() {
  if (size_ != 0) {
    (arr_[temp_vec_str_] + temp_str_)->~T();
    ++temp_str_;
    --size_;
    if (temp_str_ == kConstCnt) {
      temp_str_ = 0;
      ++temp_vec_str_;
    }
  }
}

template <typename T>
void Deque<T>::insert(iterator iter, const T& value) {
  if (size_ == 0 || iter == this->end()) {
    this->push_back(value);
    return;
  }
  Deque<T> deq;
  iterator temp_it = begin();
  try {
    for (size_t i = 0; i < size_; ++i) {
      if (temp_it != iter) {
        deq.push_back((*this)[i]);
      } else {
        deq.push_back(value);
        --i;
      }
      ++temp_it;
    }
    *this = deq;
  } catch (...) {
    while (!deq.empty()) {
      deq.pop_front();
    }
    throw;
  }
}
template <typename T>
void Deque<T>::erase(iterator iter) {
  Deque<T> deq;
  iterator temp_it = begin();
  try {
    for (size_t i = 0; i < size_; ++i) {
      if (temp_it != iter) {
        deq.push_back((*this)[i]);
      }
      ++temp_it;
    }
    *this = deq;
  } catch (...) {
    while (!deq.empty()) {
      deq.pop_front();
    }
    throw;
  }
}

template <typename T>
template <bool IsConst>
class Deque<T>::common_iterator {
 public:
  using value_type = std::conditional_t<IsConst, const T, T>;
  using type_vec =
      std::conditional_t<IsConst, const std::vector<T*>, std::vector<T*>>;
  using pointer = value_type*;
  using iterator_category = std::random_access_iterator_tag;
  using reference = value_type&;
  using difference_type = ptrdiff_t;

  common_iterator() = default;
  common_iterator(type_vec* arr, size_t temp_vec, size_t temp_item) {
    arr_ = arr;
    temp_vec_ = temp_vec;
    temp_item_ = temp_item;
  }
  common_iterator(const common_iterator& other) = default;
  common_iterator& operator=(const common_iterator& other) = default;
  ~common_iterator() = default;

  reference operator*() { return (*arr_)[temp_vec_][temp_item_]; }
  const T& operator*() const { return (*arr_)[temp_vec_][temp_item_]; }
  pointer operator->() { return &((*arr_)[temp_vec_][temp_item_]); }
  const T* operator->() const { return &((*arr_)[temp_vec_][temp_item_]); }

  common_iterator<IsConst>& operator++() {
    if (temp_item_ < kConstCnt - 1) {
      ++temp_item_;
      return *this;
    }
    ++temp_vec_;
    temp_item_ = 0;
    return *this;
  }
  common_iterator<IsConst>& operator--() {
    if (temp_item_ > 0) {
      --temp_item_;
      return *this;
    }
    --temp_vec_;
    temp_item_ = kConstCnt - 1;
    return *this;
  }
  common_iterator<IsConst> operator++(int) {
    common_iterator<IsConst> temp = *this;
    ++(*this);
    return temp;
  }
  common_iterator<IsConst> operator--(int) {
    common_iterator<IsConst> temp = *this;
    --(*this);
    return temp;
  }

  common_iterator<IsConst> operator+(int num) const {
    if (num < 0) {
      return *this - (-num);
    }
    size_t vec = temp_vec_;
    size_t item = temp_item_;
    if (num + temp_item_ < kConstCnt) {
      item = num + temp_item_;
      return common_iterator<IsConst>(arr_, vec, item);
    }
    ++vec;
    item = temp_item_ + num - kConstCnt;
    return common_iterator<IsConst>(arr_, vec, item);
  }
  common_iterator<IsConst>& operator+=(int num) {
    if (num < 0) {
      return *this -= (-num);
    }
    *this = *this + num;
    return *this;
  }

  common_iterator<IsConst> operator-(int num) const {
    if (num < 0) {
      return *this + (-num);
    }
    size_t item = temp_item_;
    size_t vec = temp_vec_;
    if ((int)temp_item_ - (int)num >= 0) {
      item = temp_item_ - num;
      return common_iterator<IsConst>(arr_, vec, item);
    }
    --vec;
    item = temp_item_ - num + kConstCnt;
    return common_iterator<IsConst>(arr_, vec, item);
  }
  common_iterator<IsConst>& operator-=(int num) {
    if (num < 0) {
      return *this += (-num);
    }
    *this = *this - num;
    return *this;
  }

  bool operator==(const common_iterator<IsConst>& other) const {
    return (temp_vec_ == other.temp_vec_ && temp_item_ == other.temp_item_);
  }
  bool operator!=(const common_iterator<IsConst>& other) const {
    return !(*this == other);
  }
  bool operator<(const common_iterator<IsConst>& other) const {
    if (*this == other || arr_ != other.arr_) {
      return false;
    }
    return ((temp_vec_ < other.temp_vec_) ||
            (temp_vec_ == other.temp_vec_ && temp_item_ < other.temp_item_));
  }
  bool operator>(const common_iterator<IsConst>& other) const {
    return (!(*this == other) && !(*this < other));
  }
  bool operator<=(const common_iterator<IsConst>& other) const {
    return !(*this > other);
  }
  bool operator>=(const common_iterator<IsConst>& other) const {
    return !(*this < other);
  }

  difference_type operator-(
      const Deque<T>::common_iterator<IsConst>& other) const {
    if (*this == other) {
      return 0;
    }
    if (*this < other) {
      return -(other - *this);
    }
    if (temp_vec_ == other.temp_vec_) {
      return temp_item_ - other.temp_item_;
    }
    size_t cnt = (temp_vec_ - other.temp_item_ - 1) * kConstCnt +
                 (kConstCnt - other.temp_item_) + temp_item_;
    return cnt;
  }

 private:
  type_vec* arr_ = nullptr;
  size_t temp_vec_ = 0;
  size_t temp_item_ = 0;
};

template <typename T>
template <typename Iter>
class Deque<T>::rev_iterator {
 public:
  using value_type = typename Iter::value_type;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = typename Iter::difference_type;
  using reference = typename Iter::reference;
  using pointer = typename Iter::pointer;
  explicit rev_iterator(Iter iter) { iter_ = iter; }
  rev_iterator(const rev_iterator& other) = default;
  rev_iterator& operator=(const rev_iterator& other) = default;
  ~rev_iterator() = default;

  reference operator*() { return *iter_; }
  const T& operator*() const { return *iter_; }
  pointer operator->() { return &(*iter_); }
  const T* operator->() const { return &(*iter_); }

  rev_iterator& operator++() {
    --iter_;
    return *this;
  }
  rev_iterator& operator--() {
    ++iter_;
    return *this;
  }
  rev_iterator operator++(int) {
    rev_iterator temp = *this;
    ++(*this);
    return temp;
  }
  rev_iterator operator--(int) {
    rev_iterator temp = *this;
    --(*this);
    return temp;
  }

  rev_iterator operator+(int num) const {
    if (num < 0) {
      return *this - (-num);
    }
    Iter tmp = iter_;
    tmp -= num;
    return rev_iterator(tmp);
  }
  rev_iterator& operator+=(int num) {
    iter_ -= num;
    return *this;
  }

  rev_iterator operator-(int num) const {
    if (num < 0) {
      return *this + (-num);
    }
    Iter tmp = iter_;
    tmp += num;
    return rev_iterator(tmp);
  }
  rev_iterator& operator-=(int num) {
    iter_ += num;
    return *this;
  }

  bool operator==(const rev_iterator& other) const {
    return (iter_ == other.iter_);
  }
  bool operator!=(const rev_iterator& other) const { return !(*this == other); }
  bool operator<(const rev_iterator& other) const {
    return (iter_ > other.iter_);
  }
  bool operator>(const rev_iterator& other) const {
    return (!(*this == other) && !(*this < other));
  }
  bool operator<=(const rev_iterator& other) const { return !(*this > other); }
  bool operator>=(const rev_iterator& other) const { return !(*this < other); }

  difference_type operator-(const rev_iterator& other) {
    return other.iter_ - iter_;
  }

 private:
  Iter iter_;
};
