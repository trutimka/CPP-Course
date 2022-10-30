#pragma once

#include <cstddef>
#include <vector>

class RingBuffer {
public:
  explicit RingBuffer(size_t capacity) {
    size_ = capacity;
    buffer_.resize(capacity);
  }

  size_t Size() const { return cur_size_; }

  bool Empty() const { 
    return cur_size_ == 0;
  }

  bool TryPush(int element) {
    if (cur_size_ == size_) {
      return false;
    }
    buffer_[end_] = element;
    ++end_;
    end_ %= size_;
    ++cur_size_;
    return true;
  }

  bool TryPop(int* element) {
    if (cur_size_ != 0) {
      *element = buffer_[begin_];
      ++begin_;
      begin_ %= size_;
      --cur_size_;
      return true;
    }
    return false;
  }
private:
  std::vector<int> buffer_;
  size_t begin_ = 0;
  size_t end_ = 0;
  size_t size_ = 0;
  size_t cur_size_ = 0;
};