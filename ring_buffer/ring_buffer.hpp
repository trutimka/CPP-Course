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
    if (cur_size_ == 0) return true;
    return false;
  }

  bool TryPush(int element) {
    if (cur_size_ == size_) {
      return false;
    } else {
      buffer_[end_] = element;
      ++end_;
      end_ %= size_;
      ++cur_size_;
      return true;
    }
  }

  bool TryPop(int* element) {
    if (cur_size_ != 0) {
      *element = buffer_[begin_];
      ++begin_;
      begin_ %= size_;
      --cur_size_;
      return true;
    } else {
      return false;
    }
  }
private:
  std::vector<int> buffer_;
  size_t begin_ = 0;
  size_t end_ = 0;
  size_t size_ = 0;
  size_t cur_size_ = 0;
};