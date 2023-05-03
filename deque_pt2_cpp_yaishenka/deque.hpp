
#pragma once
#include <cstring>
#include <iostream>
#include <vector>

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
 public:
  using allocator_traits = std::allocator_traits<Allocator>;
  Deque() = default;
  Deque(const Allocator& allocator) : allocator_(allocator) {}
  Deque(size_t count, const Allocator& allocator = Allocator())
      : allocator_(allocator) {
    try {
      while (size_ != count) {
        push_back_default_constr();
      }
    } catch (...) {
      clean_array();
      throw;
    }
  }
  Deque(size_t count, const T& value, const Allocator& allocator = Allocator())
      : allocator_(allocator) {
    try {
      while (size_ != count) {
        this->push_back(value);
      }
    } catch (...) {
      clean_array();
      throw;
    }
  }
  Deque(std::initializer_list<T> init, const Allocator& allocator = Allocator())
      : allocator_(allocator) {
    auto iter = init.begin();
    try {
      while (iter != init.end()) {
        this->push_back(*iter);
        ++iter;
      }
    } catch (...) {
      clean_array();
      throw;
    }
  }
  ~Deque() { clean_array(); }
  Deque(const Deque& other) {
    allocator_ = allocator_traits::select_on_container_copy_construction(
        other.allocator_);
    auto iter = other.cbegin();
    try {
      while (iter != other.cend()) {
        this->push_back(*iter);
        ++iter;
      }
    } catch (...) {
      clean_array();
      throw;
    }
  }
  Deque(Deque&& other) {
    allocator_ = other.allocator_;
    arr_.resize(other.arr_.size());
    for (size_t i = 0; i < arr_.size(); ++i) {
      arr_[i] = other.arr_[i];
      other.arr_[i] = nullptr;
    }
    size_ = other.size_;
    capacity_ = other.capacity_;
    init_ind_ = other.init_ind_;
    init_vec_ = other.init_vec_;
    fin_vec_ = other.fin_vec_;
    fin_ind_ = other.fin_ind_;
    other.size_ = 0;
    other.capacity_ = 0;
    other.init_ind_ = 0;
    other.init_vec_ = 0;
    other.fin_ind_ = 0;
    other.fin_vec_ = 0;
  }
  Deque& operator=(const Deque& other) {
    if (&other == this) {
      return *this;
    }
    Deque<T, Allocator> deq(other);
    (*this) = std::move(deq);
    if (allocator_traits::propagate_on_container_copy_assignment::value) {
      allocator_ = other.allocator_;
    }
    return *this;
  }
  Deque& operator=(Deque&& other) {
    if (allocator_traits::propagate_on_container_move_assignment::value) {
      allocator_ = other.allocator_;
    }
    clean_array();
    arr_.resize(other.arr_.size());
    for (size_t i = 0; i < arr_.size(); ++i) {
      arr_[i] = other.arr_[i];
      other.arr_[i] = nullptr;
    }
    size_ = other.size_;
    capacity_ = other.capacity_;
    init_ind_ = other.init_ind_;
    init_vec_ = other.init_vec_;
    fin_vec_ = other.fin_vec_;
    fin_ind_ = other.fin_ind_;
    other.size_ = 0;
    other.capacity_ = 0;
    other.init_ind_ = 0;
    other.init_vec_ = 0;
    other.fin_ind_ = 0;
    other.fin_vec_ = 0;
    return *this;
  }
  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }
  T& operator[](size_t index) {
    size_t full_vec = index / kConstCnt;
    if (index % kConstCnt + init_ind_ >= kConstCnt) {
      ++full_vec;
      return (arr_[init_vec_ + full_vec]
                  [index % kConstCnt + init_ind_ - kConstCnt]);
    }
    return (arr_[init_vec_ + full_vec][index % kConstCnt + init_ind_]);
  }
  const T& operator[](size_t index) const {
    size_t full_vec = index / kConstCnt;
    if (index % kConstCnt + init_ind_ >= kConstCnt) {
      ++full_vec;
      return (arr_[init_vec_ + full_vec]
                  [index % kConstCnt + init_ind_ - kConstCnt]);
    }
    return (arr_[init_vec_ + full_vec][index % kConstCnt + init_ind_]);
  }
  T& at(size_t index) {
    if (index >= size_) {
      throw std::out_of_range("out_of_range");
    }
    return (*this)[index];
  }
  const T& at(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("out_of_range");
    }
    return (*this)[index];
  }
  void push_back(const T& value) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1], value);
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (fin_ind_ == kConstCnt) {
        ++fin_vec_;
        fin_ind_ = 0;
      }
      try {
        allocator_traits::construct(allocator_, (arr_[fin_vec_] + fin_ind_),
                                    value);
      } catch (...) {
        if (fin_ind_ == 0) {
          --fin_vec_;
          fin_ind_ = kConstCnt;
        }
        throw;
      }
      ++fin_ind_;
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, new_block, value);
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.push_back(new_block);
    capacity_ += kConstCnt;
    ++size_;
    ++fin_vec_;
    fin_ind_ = 1;
  }
  void push_front(const T& value) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1], value);
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (init_ind_ == 0) {
        --init_vec_;
        init_ind_ = kConstCnt;
      }
      --init_ind_;
      try {
        allocator_traits::construct(allocator_, (arr_[init_vec_] + init_ind_),
                                    value);
      } catch (...) {
        ++init_ind_;
        if (init_ind_ == kConstCnt) {
          ++init_vec_;
          init_ind_ = 0;
        }
        throw;
      }
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, (new_block + kConstCnt - 1),
                                  value);
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.insert(arr_.begin(), new_block);
    capacity_ += kConstCnt;
    ++size_;
    init_ind_ = kConstCnt - 1;
  }
  void push_back(T&& value) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1], std::move(value));
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (fin_ind_ == kConstCnt) {
        ++fin_vec_;
        fin_ind_ = 0;
      }
      try {
        allocator_traits::construct(allocator_, (arr_[fin_vec_] + fin_ind_),
                                    std::move(value));
      } catch (...) {
        if (fin_ind_ == 0) {
          --fin_vec_;
          fin_ind_ = kConstCnt;
        }
        throw;
      }
      ++fin_ind_;
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, new_block, std::move(value));
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.push_back(new_block);
    capacity_ += kConstCnt;
    ++size_;
    ++fin_vec_;
    fin_ind_ = 1;
  }
  void push_front(T&& value) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1], std::move(value));
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (init_ind_ == 0) {
        --init_vec_;
        init_ind_ = kConstCnt;
      }
      --init_ind_;
      try {
        allocator_traits::construct(allocator_, (arr_[init_vec_] + init_ind_),
                                    std::move(value));
      } catch (...) {
        ++init_ind_;
        if (init_ind_ == kConstCnt) {
          ++init_vec_;
          init_ind_ = 0;
        }
        throw;
      }
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, (new_block + kConstCnt - 1),
                                  std::move(value));
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.insert(arr_.begin(), new_block);
    capacity_ += kConstCnt;
    ++size_;
    init_ind_ = kConstCnt - 1;
  }
  void pop_back() {
    allocator_traits::destroy(allocator_, (arr_[fin_vec_] + fin_ind_ - 1));
    --size_;
    --fin_ind_;
    if (fin_ind_ == 0 && size_ != 0) {
      fin_ind_ = kConstCnt;
      --fin_vec_;
    }
  }
  void pop_front() {
    allocator_traits::destroy(allocator_, (arr_[init_vec_] + init_ind_));
    --size_;
    ++init_ind_;
    if (init_ind_ == kConstCnt && size_ != 0) {
      init_ind_ = 0;
      ++init_vec_;
    }
  }

  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using type_vec =
        std::conditional_t<IsConst, const std::vector<T*>, std::vector<T*>>;
    using pointer = value_type*;
    using iterator_category = std::random_access_iterator_tag;
    using reference = value_type&;
    using difference_type = ptrdiff_t;

    CommonIterator() = default;
    CommonIterator(type_vec* arr, size_t temp_vec, size_t temp_item)
        : arr_(arr), vec_(temp_vec), ind_(temp_item) {}
    CommonIterator(const CommonIterator& other) = default;
    CommonIterator& operator=(const CommonIterator& other) = default;
    ~CommonIterator() = default;

    reference operator*() { return (*arr_)[vec_][ind_]; }
    const T& operator*() const { return (*arr_)[vec_][ind_]; }
    pointer operator->() { return &((*arr_)[vec_][ind_]); }
    const T* operator->() const { return &((*arr_)[vec_][ind_]); }

    CommonIterator<IsConst>& operator++() {
      if (ind_ < kConstCnt - 1) {
        ++ind_;
        return *this;
      }
      ++vec_;
      ind_ = 0;
      return *this;
    }
    CommonIterator<IsConst>& operator--() {
      if (ind_ > 0) {
        --ind_;
        return *this;
      }
      --vec_;
      ind_ = kConstCnt - 1;
      return *this;
    }
    CommonIterator<IsConst> operator++(int) {
      CommonIterator<IsConst> temp = *this;
      ++(*this);
      return temp;
    }
    CommonIterator<IsConst> operator--(int) {
      CommonIterator<IsConst> temp = *this;
      --(*this);
      return temp;
    }

    CommonIterator<IsConst> operator+(int num) const {
      if (num < 0) {
        return *this - (-num);
      }
      size_t vec = vec_;
      size_t item = ind_;
      if (num + ind_ < kConstCnt) {
        item = num + ind_;
        return CommonIterator<IsConst>(arr_, vec, item);
      }
      ++vec;
      item = ind_ + num - kConstCnt;
      return CommonIterator<IsConst>(arr_, vec, item);
    }
    CommonIterator<IsConst>& operator+=(int num) {
      if (num < 0) {
        return *this -= (-num);
      }
      *this = *this + num;
      return *this;
    }

    CommonIterator<IsConst> operator-(int num) const {
      if (num < 0) {
        return *this + (-num);
      }
      size_t item = ind_;
      size_t vec = vec_;
      if ((int)ind_ - (int)num >= 0) {
        item = ind_ - num;
        return CommonIterator<IsConst>(arr_, vec, item);
      }
      --vec;
      item = ind_ - num + kConstCnt;
      return CommonIterator<IsConst>(arr_, vec, item);
    }
    CommonIterator<IsConst>& operator-=(int num) {
      if (num < 0) {
        return *this += (-num);
      }
      *this = *this - num;
      return *this;
    }

    bool operator==(const CommonIterator<IsConst>& other) const {
      return (vec_ == other.vec_ && ind_ == other.ind_);
    }
    bool operator!=(const CommonIterator<IsConst>& other) const {
      return !(*this == other);
    }
    bool operator<(const CommonIterator<IsConst>& other) const {
      if (*this == other || arr_ != other.arr_) {
        return false;
      }
      return ((vec_ < other.vec_) || (vec_ == other.vec_ && ind_ < other.ind_));
    }
    bool operator>(const CommonIterator<IsConst>& other) const {
      return (!(*this == other) && !(*this < other));
    }
    bool operator<=(const CommonIterator<IsConst>& other) const {
      return !(*this > other);
    }
    bool operator>=(const CommonIterator<IsConst>& other) const {
      return !(*this < other);
    }

    difference_type operator-(
        const Deque<T, Allocator>::CommonIterator<IsConst>& other) const {
      if (*this == other) {
        return 0;
      }
      if (*this < other) {
        return -(other - *this);
      }
      if (vec_ == other.vec_) {
        return ind_ - other.ind_;
      }
      size_t cnt =
          (vec_ - other.ind_ - 1) * kConstCnt + (kConstCnt - other.ind_) + ind_;
      return cnt;
    }

   private:
    type_vec* arr_ = nullptr;
    size_t vec_ = 0;
    size_t ind_ = 0;
  };
  template <typename Iter>
  class RevIterator {
   public:
    using value_type = typename Iter::value_type;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = typename Iter::difference_type;
    using reference = typename Iter::reference;
    using pointer = typename Iter::pointer;
    explicit RevIterator(Iter iter) : iter_(iter) {}
    RevIterator(const RevIterator& other) = default;
    RevIterator& operator=(const RevIterator& other) = default;
    ~RevIterator() = default;

    reference operator*() { return *iter_; }
    const T& operator*() const { return *iter_; }
    pointer operator->() { return &(*iter_); }
    const T* operator->() const { return &(*iter_); }

    RevIterator& operator++() {
      --iter_;
      return *this;
    }
    RevIterator& operator--() {
      ++iter_;
      return *this;
    }
    RevIterator operator++(int) {
      RevIterator temp = *this;
      ++(*this);
      return temp;
    }
    RevIterator operator--(int) {
      RevIterator temp = *this;
      --(*this);
      return temp;
    }

    RevIterator operator+(int num) const {
      if (num < 0) {
        return *this - (-num);
      }
      Iter tmp = iter_;
      tmp -= num;
      return RevIterator(tmp);
    }
    RevIterator& operator+=(int num) {
      iter_ -= num;
      return *this;
    }

    RevIterator operator-(int num) const {
      if (num < 0) {
        return *this + (-num);
      }
      Iter tmp = iter_;
      tmp += num;
      return RevIterator(tmp);
    }
    RevIterator& operator-=(int num) {
      iter_ += num;
      return *this;
    }

    bool operator==(const RevIterator& other) const {
      return (iter_ == other.iter_);
    }
    bool operator!=(const RevIterator& other) const {
      return !(*this == other);
    }
    bool operator<(const RevIterator& other) const {
      return (iter_ > other.iter_);
    }
    bool operator>(const RevIterator& other) const {
      return (!(*this == other) && !(*this < other));
    }
    bool operator<=(const RevIterator& other) const { return !(*this > other); }
    bool operator>=(const RevIterator& other) const { return !(*this < other); }

    difference_type operator-(const RevIterator& other) {
      return other.iter_ - iter_;
    }

   private:
    Iter iter_;
  };
  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = RevIterator<CommonIterator<false>>;
  using const_reverse_iterator = RevIterator<CommonIterator<true>>;

  iterator begin() { return iterator(&arr_, init_vec_, init_ind_); }
  iterator end() { return iterator(&arr_, fin_vec_, fin_ind_); }
  const_iterator cbegin() const {
    return const_iterator(&arr_, init_vec_, init_ind_);
  }
  const_iterator cend() const {
    return const_iterator(&arr_, fin_vec_, fin_ind_);
  }
  reverse_iterator rbegin() { return reverse_iterator(end() - 1); }
  reverse_iterator rend() { return reverse_iterator(begin() - 1); }
  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(cend() - 1);
  }
  const_reverse_iterator crend() const {
    return const_reverse_iterator(cbegin() - 1);
  }
  void insert(iterator iter, const T& value) {
    Deque<T, Allocator> deq(allocator_);
    iterator itt = begin();
    try {
      while (itt != iter) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      deq.push_back(std::move(value));
      while (itt != end()) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      (*this) = std::move(deq);
    } catch (...) {
      throw;
    }
  }
  void erase(iterator iter) {
    Deque<T, Allocator> deq(allocator_);
    iterator itt = begin();
    try {
      while (itt != iter) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      ++itt;
      while (itt != end()) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      (*this) = std::move(deq);
    } catch (...) {
      throw;
    }
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] =
            allocator_traits ::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1],
                                    std::forward<Args>(args)...);
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (fin_ind_ == kConstCnt) {
        ++fin_vec_;
        fin_ind_ = 0;
      }
      try {
        allocator_traits::construct(allocator_, (arr_[fin_vec_] + fin_ind_),
                                    std::forward<Args>(args)...);
      } catch (...) {
        if (fin_ind_ == 0) {
          --fin_vec_;
          fin_ind_ = kConstCnt;
        }
        throw;
      }
      ++fin_ind_;
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, new_block,
                                  std::forward<Args>(args)...);
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.push_back(new_block);
    capacity_ += kConstCnt;
    ++size_;
    ++fin_vec_;
    fin_ind_ = 1;
  }

  template <typename... Args>
  void emplace_front(Args&&... args) {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1],
                                    std::forward<Args>(args)...);
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (init_ind_ == 0) {
        --init_vec_;
        init_ind_ = kConstCnt;
      }
      --init_ind_;
      try {
        allocator_traits::construct(allocator_, (arr_[init_vec_] + init_ind_),
                                    std::forward<Args>(args)...);
      } catch (...) {
        ++init_ind_;
        if (init_ind_ == kConstCnt) {
          ++init_vec_;
          init_ind_ = 0;
        }
        throw;
      }
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, (new_block + kConstCnt - 1),
                                  std::forward<Args>(args)...);
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.insert(arr_.begin(), new_block);
    capacity_ += kConstCnt;
    ++size_;
    init_ind_ = kConstCnt - 1;
  }

  template <typename... Args>
  void emplace(iterator iter, Args&&... args) {
    Deque<T, Allocator> deq(allocator_);
    iterator itt = begin();
    try {
      while (itt != iter) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      deq.emplace_back(std::forward<Args>(args)...);
      while (itt != end()) {
        deq.push_back(std::move(*itt));
        ++itt;
      }
      (*this) = std::move(deq);
    } catch (...) {
      throw;
    }
  }
  Allocator get_allocator() { return allocator_; }

 private:
  void push_back_default_constr() {
    if (capacity_ == 0) {
      arr_.resize(3);
      for (size_t i = 0; i < 3; ++i) {
        arr_[i] = allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
      }
      try {
        allocator_traits::construct(allocator_, arr_[1]);
      } catch (...) {
        for (size_t i = 0; i < 3; ++i) {
          allocator_traits::deallocate(allocator_, arr_[i],
                                       kConstCnt * sizeof(T));
        }
        throw;
      }
      size_ = 1;
      capacity_ = 3 * kConstCnt;
      init_vec_ = 1;
      fin_vec_ = 1;
      fin_ind_ = 1;
      return;
    }
    if (size_ < capacity_) {
      if (fin_ind_ == kConstCnt) {
        ++fin_vec_;
        fin_ind_ = 0;
      }
      try {
        allocator_traits::construct(allocator_, (arr_[fin_vec_] + fin_ind_));
      } catch (...) {
        if (fin_ind_ == 0) {
          --fin_vec_;
          fin_ind_ = kConstCnt;
        }
        throw;
      }
      ++fin_ind_;
      ++size_;
      return;
    }
    T* new_block =
        allocator_traits::allocate(allocator_, kConstCnt * sizeof(T));
    try {
      allocator_traits::construct(allocator_, new_block);
    } catch (...) {
      allocator_traits::deallocate(allocator_, new_block,
                                   kConstCnt * sizeof(T));
      throw;
    }
    arr_.push_back(new_block);
    capacity_ += kConstCnt;
    ++size_;
    ++fin_vec_;
    fin_ind_ = 1;
  }
  void clean_array() {
    if (init_vec_ != fin_vec_) {
      for (size_t i = init_ind_; i < kConstCnt; ++i) {
        allocator_traits::destroy(allocator_, (arr_[init_vec_] + i));
      }
      for (size_t i = init_vec_ + 1; i < fin_vec_; ++i) {
        for (size_t j = 0; j < kConstCnt; ++j) {
          allocator_traits::destroy(allocator_, (arr_[i] + j));
        }
      }
      for (size_t i = 0; i < fin_ind_; ++i) {
        allocator_traits::destroy(allocator_, (arr_[fin_vec_] + i));
      }
    } else {
      for (size_t i = init_ind_; i < fin_ind_; ++i) {
        allocator_traits::destroy(allocator_, (arr_[init_vec_] + i));
      }
    }
    if (capacity_ != 0) {
      for (size_t i = 0; i < arr_.size(); ++i) {
        allocator_traits::deallocate(allocator_, arr_[i],
                                     kConstCnt * sizeof(T));
      }
    }
  }

  std::vector<T*> arr_;
  Allocator allocator_;
  static const size_t kConstCnt = 1e5;
  size_t size_ = 0;
  size_t capacity_ = 0;
  size_t init_vec_ = 0;
  size_t fin_vec_ = 0;
  size_t init_ind_ = 0;
  size_t fin_ind_ = 0;
};