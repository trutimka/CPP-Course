//
// pch.h
//

#pragma once
#include "gtest/gtest.h"
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class List {
private:
  struct BaseNode {
    BaseNode* prev = nullptr;
    BaseNode* next = nullptr;
  };
  struct Node : public BaseNode {
    T value;
  };

  using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using node_traits = std::allocator_traits<node_allocator>;

  BaseNode fake_node;
  node_allocator alloc_;
  size_t size_ = 0;
  Node* create_node() {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value);
    node->next = nullptr;
    node->prev = nullptr;
    return node;
  }
  Node* create_node(const T& val) {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value, std::move(val));
    node->next = nullptr;
    node->prev = nullptr;
    return node;
  }
  Node* create_node(T&& val) {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value, std::move(val));
    node->next = nullptr;
    node->prev = nullptr;
    return node;
  }
  void destroy_node(Node* node) {
    node_traits::destroy(alloc_, &node->value);
    node_traits::deallocate(alloc_, node, 1);
  }

public:
  using valuetype = T;
  using allocator_type = Allocator;
  using different_type = std::ptrdiff_t;
  using reference = T&;


  List() : fake_node{ &fake_node, &fake_node } {}
  List(size_t count, const T& value = T(), const Allocator& alloc = Allocator()) : size_(count), alloc_(alloc) {
    Node* node = create_node(value);
    fake_node.next = static_cast<BaseNode*>(node);
    node->prev = &fake_node;
    for (auto i = 0; i < count - 1; ++i) {
      Node* tmp = create_node(value);
      node->next = static_cast<BaseNode*>(tmp);
      tmp->prev = static_cast<BaseNode*>(node);
      node = tmp;
    }
    node->next = &fake_node;
    fake_node.prev = static_cast<BaseNode*>(node);
  }
  explicit List(size_t count, const Allocator& alloc) : size_(count), alloc_(alloc) {
    Node* head = create_node();
    fake_node.next = static_cast<BaseNode*>(head);
    head->prev = &fake_node;
    Node* temp = head;
    for (auto i = 0; i < count - 1; ++i) {
      Node* tmp = create_node();
      temp->next = static_cast<BaseNode*>(tmp);
      tmp->prev = static_cast<BaseNode*>(temp);
      temp = tmp;
    }
    temp->next = &fake_node;
    fake_node.prev = static_cast<BaseNode*>(temp);
  }
  List(const List& other) {
    size_ = other.size_;
    alloc_ = node_traits::select_on_container_copy_construction(other.alloc_);
    if (size_ != 0) {
      Node* head = create_node();
      fake_node.next = static_cast<BaseNode*>(head);
      Node* head_other = static_cast<Node*>(other.fake_node.next);
      //head->value = std::move(head_other->value);
      head->value = head_other->value;
      head->prev = &fake_node;
      Node* temp = head;
      Node* temp_other = head_other;
      for (auto i = 0; i < size_ - 1; ++i) {
        Node* tmp = create_node();
        Node* tmp_other = static_cast<Node*>(static_cast<BaseNode*>(temp_other)->next);
        //tmp->value = std::move(tmp_other->value);
        tmp->value = tmp_other->value;
        temp_other = tmp_other;
        temp->next = static_cast<BaseNode*>(tmp);
        tmp->prev = static_cast<BaseNode*>(temp);
        temp = tmp;
      }
      temp->next = &fake_node;
      fake_node.prev = static_cast<BaseNode*>(temp);
    }
  }
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator()) : alloc_(alloc) {
    size_ = init.size();
    if (size_ != 0) {
      auto iter = init.begin();
      Node* head = create_node(*iter);
      fake_node.next = static_cast<BaseNode*>(head);

      //head->value = std::move(*iter);
      ++iter;
      head->prev = &fake_node;
      Node* temp = head;

      for (auto i = 0; i < size_ - 1; ++i) {
        Node* tmp = create_node(*iter);
        //tmp->value = std::move(*iter);
        ++iter;
        temp->next = static_cast<BaseNode*>(tmp);
        tmp->prev = static_cast<BaseNode*>(temp);
        temp = tmp;
      }
      temp->next = &fake_node;
      fake_node.prev = static_cast<BaseNode*>(temp);
    }
  }
  List& operator=(const List& other) {
    if (this == &other) { return *this; }
    if (node_traits::propagate_on_container_copy_assignment::value && alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    BaseNode tempFakeNode;
    try {
      Node* head_other = static_cast<Node*>(other.fake_node.next);
      Node* head = create_node((*head_other).value);
      //head->value = std::move(head_other->value);
      //head->value = head_other->value;
      tempFakeNode.next = static_cast<BaseNode*>(head);
      head->prev = &tempFakeNode;
      Node* temp = head;
      Node* temp_other = static_cast<Node*>(static_cast<BaseNode*>(head_other)->next);
      for (int i = 0; i < other.size_ - 1; ++i) {
        Node* tmp = create_node((*temp_other).value);
        //tmp->value = std::move(temp_other->value);
        //tmp->value = temp_other->value;
        tmp->prev = static_cast<BaseNode*>(temp);
        temp->next = static_cast<BaseNode*>(tmp);
        temp = tmp;
        temp_other = static_cast<Node*>(static_cast<BaseNode*>(temp_other)->next);
      }
      temp->next = &tempFakeNode;
      tempFakeNode.prev = static_cast<BaseNode*>(temp);

      head = static_cast<Node*>(fake_node.next);
      for (int i = 0; i < size_; ++i) {
        BaseNode* tmp = static_cast<BaseNode*>(head)->next;
        destroy_node(head);
        head = static_cast<Node*>(tmp);
      }
      fake_node = tempFakeNode;
    }
    catch (...) {
      // дописать удаление

      throw;
    }
    size_ = other.size_;
    return *this;
  }
  ~List() {
    if (size_ != 0) {
      Node* temp = static_cast<Node*>(fake_node.next);
      for (auto i = 0; i < size_; ++i) {
        BaseNode* tmp = static_cast<BaseNode*>(temp)->next;
        destroy_node(temp);
        temp = static_cast<Node*>(tmp);
      }
    }
  }

  size_t size() const noexcept {
    return size_;
  }
  bool empty() const noexcept {
    return size_ == 0;
  }
  T& front() {
    return *(static_cast<Node*>(fake_node.next)->value);
  }
  const T& front() const {
    return *(static_cast<Node*>(fake_node.next)->value);
  }
  T& back() {
    return *(static_cast<Node*>(fake_node.prev)->value);
  }
  const T& back() const {
    return *(static_cast<Node*>(fake_node.prev)->value);
  }
  void push_back(const T& value) {
    Node* node;
    try {
      node = create_node(value);
      Node* tail = static_cast<Node*>(fake_node.prev);
      tail->next = static_cast<BaseNode*>(node);
      node->prev = static_cast<BaseNode*>(tail);
      node->next = &fake_node;
      fake_node.prev = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroy_node(node);
      throw;
    }
    ++size_;
  }
  void push_front(const T& value) {
    Node* node;
    try {
      node = create_node(value);
      Node* head = static_cast<Node*>(fake_node.next);
      head->prev = static_cast<BaseNode*>(node);
      node->next = static_cast<BaseNode*>(head);
      node->prev = &fake_node;
      fake_node.next = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroy_node(node);
      throw;
    }
    ++size_;
  }
  void push_back(T&& value) {
    Node* node;
    try {
      node = create_node(value);
      Node* tail = static_cast<Node*>(fake_node.prev);
      tail->next = static_cast<BaseNode*>(node);
      node->prev = static_cast<BaseNode*>(tail);
      node->next = &fake_node;
      fake_node.prev = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroy_node(node);
      throw;
    }
    ++size_;
  }
  void push_front(T&& value) {
    Node* node;
    try {
      node = create_node(value);
      Node* head = static_cast<Node*>(fake_node.next);
      head->prev = static_cast<BaseNode*>(node);
      node->next = static_cast<BaseNode*>(head);
      node->prev = &fake_node;
      fake_node.next = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroy_node(node);
      throw;
    }
    ++size_;
  }
  void pop_back() {
    Node* node = static_cast<Node*>(fake_node.prev);
    Node* tail = static_cast<Node*>(static_cast<BaseNode*>(node)->prev);
    destroy_node(node);
    tail->next = &fake_node;
    fake_node.prev = static_cast<BaseNode*>(tail);
    --size_;
  }
  void pop_front() {
    Node* node = static_cast<Node*>(fake_node.next);
    Node* head = static_cast<Node*>(static_cast<BaseNode*>(node)->next);
    destroy_node(node);
    head->prev = &fake_node;
    fake_node.next = static_cast<BaseNode*>(head);
    --size_;
  }
  node_allocator get_allocator() {
    return alloc_;
  }

  template<bool IsConst>
  class CommonIterator {
  public:
    using valuetype = std::conditional_t<IsConst, const T, T>;

    using pointer = valuetype*;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = valuetype&;
    CommonIterator() = default;
    CommonIterator(Node* node) : node_(node) {}
    CommonIterator(const CommonIterator& other) = default;
    CommonIterator& operator=(const CommonIterator& other) = default;
    ~CommonIterator() = default;
    pointer operator->() {
      return (node_->value);
    }
    reference operator*() {
      return *(node_->value);
    }
    const T* operator->() const {
      return (node_->value);
    }
    const T& operator*() const {
      return *(node_->value);
    }
    CommonIterator& operator++() {
      node_ = node_->next;
      return *this;
    }
    CommonIterator& operator--() {
      node_ = node_->prev;
      return *this;
    }
    CommonIterator operator++(int) {
      CommonIterator<IsConst> copy = *this;
      ++(*this);
      return copy;
    }
    CommonIterator operator--(int) {
      CommonIterator<IsConst> copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const CommonIterator<IsConst>& other) {
      return node_ == other.node_;
    }
    bool operator!=(const CommonIterator<IsConst>& other) {
      return !(*this == other);
    }

  private:
    Node* node_ = nullptr;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  iterator begin() noexcept {
    return iterator(static_cast<Node*>(fake_node.next));
  }
  iterator end() noexcept {
    return iterator(&fake_node);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(static_cast<Node*>(fake_node.next));
  }
  const_iterator cend() const noexcept {
    return const_iterator(&fake_node);
  }
  iterator rbegin() noexcept {
    return iterator(static_cast<Node*>(fake_node.prev));
  }
  iterator rend() noexcept {
    return iterator(&fake_node);
  }
  const_iterator rcbegin() const noexcept {
    return const_iterator(static_cast<Node*>(fake_node.prev));
  }
  const_iterator rcend() const noexcept {
    return const_iterator(&fake_node);
  }
};