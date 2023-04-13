#pragma once
#include <iterator>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode {
    BaseNode* prev = nullptr;
    BaseNode* next = nullptr;

    BaseNode() = default;
    BaseNode(BaseNode* prev, BaseNode* next) : prev(prev), next(next) {}
  };
  struct Node : public BaseNode {
    T value;
    Node() = default;
    Node(const T& value) : value(value) {}
  };

  using allocator_traits = std::allocator_traits<Allocator>;
  using node_allocator = typename allocator_traits::template rebind_alloc<Node>;
  using node_traits = typename allocator_traits::template rebind_traits<Node>;

  BaseNode fake_node_;
  node_allocator alloc_;
  size_t size_ = 0;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using different_type = std::ptrdiff_t;
  using reference = T&;

  List() : fake_node_{&fake_node_, &fake_node_} {}
  List(size_t count, const T& value, const Allocator& alloc = Allocator()) {
    alloc_ = alloc;
    try {
      for (size_t i = 0; i < count; ++i) {
        push_back(std::move(value));
      }
    } catch (...) {
      while (size_ != 0) {
        pop_front();
      }
      throw;
    }
  }
  explicit List(size_t count, const Allocator& alloc = Allocator()) {
    alloc_ = alloc;
    size_t temp_size = 0;
    Node* node;
    try {
      try {
        node = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, node);
      } catch (...) {
        node_traits::deallocate(alloc_, node, 1);
        throw;
      }
      static_cast<BaseNode*>(node)->prev = &fake_node_;
      fake_node_.next = static_cast<BaseNode*>(node);
      for (temp_size = 1; temp_size < count; ++temp_size) {
        Node* temp;
        try {
          temp = node_traits::allocate(alloc_, 1);
          node_traits::construct(alloc_, temp);
        } catch (...) {
          node_traits::deallocate(alloc_, temp, 1);
          throw;
        }
        static_cast<BaseNode*>(temp)->prev = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(node)->next = static_cast<BaseNode*>(temp);
        node = temp;
      }
      static_cast<BaseNode*>(node)->next = &fake_node_;
      fake_node_.prev = static_cast<BaseNode*>(node);
    } catch (...) {
      while (temp_size != 0) {
        --temp_size;
        Node* temp = static_cast<Node*>(static_cast<BaseNode*>(node)->prev);
        node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
        node = temp;
      }
      throw;
    }
    size_ = count;
  }
  List(const List& other) {
    alloc_ = node_traits::select_on_container_copy_construction(other.alloc_);
    const_iterator iter = other.cbegin();
    try {
      while (iter != other.cend()) {
        push_back(*iter);
        ++iter;
      }
    } catch (...) {
      while (size_ != 0) {
        pop_front();
      }
      throw;
    }
  }
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : alloc_(alloc) {
    auto iter = init.begin();
    try {
      while (iter != init.end()) {
        push_back(*iter);
        ++iter;
      }
    } catch (...) {
      while (size_ != 0) {
        pop_front();
      }
      throw;
    }
  }
  List& operator=(const List& other) {
    if (this == &other) {
      return *this;
    }
    if (node_traits::propagate_on_container_copy_assignment::value &&
        alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    size_t temp_size = 0;

    BaseNode base_node;
    BaseNode* temp_base_node = &base_node;
    try {
      auto iter = other.cbegin();
      for (size_t i = 0; i < other.size_; ++i) {
        Node* node;
        try {
          node = node_traits::allocate(alloc_, 1);
          node_traits::construct(alloc_, node, *iter);
        } catch (...) {
          node_traits::deallocate(alloc_, node, 1);
          throw;
        }
        ++temp_size;
        ++iter;
        temp_base_node->next = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(node)->prev = temp_base_node;
        temp_base_node = static_cast<BaseNode*>(node);
      }
      base_node.prev = temp_base_node;
      temp_base_node->next = &base_node;

      while (size_ != 0) {
        pop_front();
      }
      fake_node_ = base_node;
    } catch (...) {
      temp_base_node = base_node.next;
      for (size_t i = 0; i < temp_size; ++i) {
        Node* node = static_cast<Node*>(temp_base_node);
        temp_base_node = temp_base_node->next;
        node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
      }
      throw;
    }
    size_ = other.size_;
    return *this;
  }
  ~List() {
    while (size_ != 0) {
      pop_front();
    }
  }

  size_t size() const noexcept { return size_; }
  bool empty() const noexcept { return size_ == 0; }
  T& front() { return *(static_cast<Node*>(fake_node_.next)->value); }
  const T& front() const {
    return *(static_cast<Node*>(fake_node_.next)->value);
  }
  T& back() { return *(static_cast<Node*>(fake_node_.prev)->value); }
  const T& back() const {
    return *(static_cast<Node*>(fake_node_.prev)->value);
  }
  void push_back(const T& value) {
    if (size_ == 0) {
      Node* node;
      try {
        node = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, node, std::move(value));
        static_cast<BaseNode*>(node)->prev = &fake_node_;
        static_cast<BaseNode*>(node)->next = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(node);
        fake_node_.prev = static_cast<BaseNode*>(node);
        size_ = 1;
      } catch (...) {
        // node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
        throw;
      }
    } else {
      Node* node = static_cast<Node*>(fake_node_.prev);
      Node* temp;
      try {
        temp = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, temp, std::move(value));
        static_cast<BaseNode*>(temp)->prev = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(temp)->next = &fake_node_;
        fake_node_.prev = static_cast<BaseNode*>(temp);
        static_cast<BaseNode*>(node)->next = static_cast<BaseNode*>(temp);
        ++size_;
      } catch (...) {
        // node_traits::destroy(alloc_, temp);
        node_traits::deallocate(alloc_, temp, 1);
        throw;
      }
    }
  }
  void push_front(const T& value) {
    if (size_ == 0) {
      Node* node;
      try {
        node = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, node, std::move(value));
        static_cast<BaseNode*>(node)->prev = &fake_node_;
        static_cast<BaseNode*>(node)->next = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(node);
        fake_node_.prev = static_cast<BaseNode*>(node);
        size_ = 1;
      } catch (...) {
        // node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
        throw;
      }
    } else {
      Node* node = static_cast<Node*>(fake_node_.next);
      Node* temp;
      try {
        temp = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, temp, std::move(value));
        static_cast<BaseNode*>(temp)->next = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(temp)->prev = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(temp);
        static_cast<BaseNode*>(node)->prev = static_cast<BaseNode*>(temp);
        ++size_;
      } catch (...) {
        // node_traits::destroy(alloc_, temp);
        node_traits::deallocate(alloc_, temp, 1);
        throw;
      }
    }
  }
  void push_back(T&& value) {
    if (size_ == 0) {
      Node* node;
      try {
        node = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, node, std::move(value));
        static_cast<BaseNode*>(node)->prev = &fake_node_;
        static_cast<BaseNode*>(node)->next = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(node);
        fake_node_.prev = static_cast<BaseNode*>(node);
        size_ = 1;
      } catch (...) {
        // node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
        throw;
      }
    } else {
      Node* node = static_cast<Node*>(fake_node_.prev);
      Node* temp;
      try {
        temp = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, temp, std::move(value));
        static_cast<BaseNode*>(temp)->prev = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(temp)->next = &fake_node_;
        fake_node_.prev = static_cast<BaseNode*>(temp);
        static_cast<BaseNode*>(node)->next = static_cast<BaseNode*>(temp);
        ++size_;
      } catch (...) {
        // node_traits::destroy(alloc_, temp);
        node_traits::deallocate(alloc_, temp, 1);
        throw;
      }
    }
  }
  void push_front(T&& value) {
    if (size_ == 0) {
      Node* node;
      try {
        node = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, node, std::move(value));
        static_cast<BaseNode*>(node)->prev = &fake_node_;
        static_cast<BaseNode*>(node)->next = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(node);
        fake_node_.prev = static_cast<BaseNode*>(node);
        size_ = 1;
      } catch (...) {
        // node_traits::destroy(alloc_, node);
        node_traits::deallocate(alloc_, node, 1);
        throw;
      }
    } else {
      Node* node = static_cast<Node*>(fake_node_.next);
      Node* temp;
      try {
        temp = node_traits::allocate(alloc_, 1);
        node_traits::construct(alloc_, temp, std::move(value));
        static_cast<BaseNode*>(temp)->next = static_cast<BaseNode*>(node);
        static_cast<BaseNode*>(temp)->prev = &fake_node_;
        fake_node_.next = static_cast<BaseNode*>(temp);
        static_cast<BaseNode*>(node)->prev = static_cast<BaseNode*>(temp);
        ++size_;
      } catch (...) {
        // node_traits::destroy(alloc_, temp);
        node_traits::deallocate(alloc_, temp, 1);
        throw;
      }
    }
  }
  void pop_back() {
    if (size_ != 0) {
      Node* node = static_cast<Node*>(fake_node_.prev);
      Node* temp = static_cast<Node*>(static_cast<BaseNode*>(node)->prev);
      node_traits::destroy(alloc_, node);
      node_traits::deallocate(alloc_, node, 1);
      static_cast<BaseNode*>(temp)->next = &fake_node_;
      fake_node_.prev = static_cast<BaseNode*>(temp);
      --size_;
    }
  }
  void pop_front() {
    if (size_ != 0) {
      Node* node = static_cast<Node*>(fake_node_.next);
      Node* temp = static_cast<Node*>(static_cast<BaseNode*>(node)->next);
      node_traits::destroy(alloc_, node);
      node_traits::deallocate(alloc_, node, 1);
      static_cast<BaseNode*>(temp)->prev = &fake_node_;
      fake_node_.next = static_cast<BaseNode*>(temp);
      --size_;
    }
  }
  node_allocator get_allocator() { return alloc_; }
  template <bool IsConst>
  class CommonIterator {
   public:
    using value_type = std::conditional_t<IsConst, const T, T>;
    using base_node_type =
        std::conditional_t<IsConst, const BaseNode, BaseNode>;
    using node_type = std::conditional_t<IsConst, const Node, Node>;
    using pointer = value_type*;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = value_type&;
    using difference_type = std::ptrdiff_t;
    explicit CommonIterator(base_node_type* node) : node_(node) {}
    reference operator*() { return (static_cast<node_type*>(node_)->value); }
    pointer operator->() { return &(static_cast<node_type*>(node_)->value); }

    CommonIterator& operator++() {
      node_ = node_->next;
      return *this;
    }
    CommonIterator& operator--() {
      node_ = node_->prev;
      return *this;
    }
    CommonIterator operator++(int) {
      CommonIterator copy = *this;
      ++(*this);
      return copy;
    }
    CommonIterator operator--(int) {
      CommonIterator copy = *this;
      --(*this);
      return copy;
    }
    bool operator==(const CommonIterator& other) const {
      return (node_->next == other.node_->next &&
              node_->prev == other.node_->prev);
    }
    bool operator!=(const CommonIterator& other) const {
      return !(*this == other);
    }

   private:
    base_node_type* node_;
  };

  template <typename Iterator>
  class ReverseIterator {
   public:
    using value_type = typename Iterator::value_type;
    using pointer = typename Iterator::pointer;
    using iterator_category = typename Iterator::iterator_category;
    using reference = typename Iterator::reference;
    using difference_type = typename Iterator::difference_type;

    explicit ReverseIterator(Iterator iter) : iter_(iter) {}
    reference operator*() { return *iter_; }
    pointer operator->() { return &(*iter_); }

    ReverseIterator& operator++() {
      --iter_;
      return *this;
    }
    ReverseIterator& operator--() {
      ++iter_;
      return *this;
    }
    ReverseIterator operator++(int) {
      ReverseIterator temp = *this;
      ++(*this);
      return temp;
    }
    ReverseIterator operator--(int) {
      ReverseIterator temp = *this;
      --(*this);
      return temp;
    }
    bool operator==(const ReverseIterator& other) const {
      return (iter_ == other.iter_);
    }
    bool operator!=(const ReverseIterator& other) const {
      return !(*this == other);
    }

   private:
    Iterator iter_;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = ReverseIterator<CommonIterator<false>>;
  using const_reverse_iterator = ReverseIterator<CommonIterator<true>>;

  const_iterator begin() const { return const_iterator(fake_node_.next); }
  const_iterator end() const { return const_iterator(&fake_node_); }
  iterator begin() { return iterator(fake_node_.next); }
  iterator end() { return iterator(&fake_node_); }
  const_iterator cbegin() const { return const_iterator(fake_node_.next); }
  const_iterator cend() const { return const_iterator(&fake_node_); }
  iterator rbegin() const { return reverse_iterator(--end()); }
  iterator rend() const { return reverse_iterator(--begin()); }
  iterator rbegin() { return reverse_iterator(--end()); }
  iterator rend() { return reverse_iterator(--begin()); }
  const_iterator crbegin() const { return const_reverse_iterator(--cend()); }
  const_iterator crend() const { return const_reverse_iterator(--cbegin()); }
};