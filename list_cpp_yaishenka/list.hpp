#pragma once
#include <memory>

template<typename T, typename Allocator = std::allocator<T>>
class List {
private:
  struct BaseNode {
    BaseNode* prev_ = nullptr;
    BaseNode* next_ = nullptr;
  };
  struct Node : public BaseNode {
    T value_;
  };

  using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using node_traits = std::allocator_traits<node_allocator>;

  BaseNode fakeNode_;
  node_allocator alloc_;
  size_t size_ = 0;
  Node* createNode() {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value_);
    node->next_ = nullptr;
    node->prev_ = nullptr;
    return node;
  }
  Node* createNode(const T& value) {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value_, std::move(value));
    node->next_ = nullptr;
    node->prev_ = nullptr;
    return node;
  }
  Node* createNode(T&& value) {
    Node* node = node_traits::allocate(alloc_, 1);
    node_traits::construct(alloc_, &node->value_, std::move(value));
    node->next_ = nullptr;
    node->prev_ = nullptr;
    return node;
  }
  void destroyNode(Node* node) {
    node_traits::destroy(alloc_, &node->value_);
    node_traits::deallocate(alloc_, node, 1);
  }

public:
  using value_type = T;
  using allocator_type = Allocator;
  using different_type = std::ptrdiff_t;
  using reference = T&;


  List() : fakeNode_{ &fakeNode_, &fakeNode_ } {}
  List(size_t count, const T& value = T(), const Allocator& alloc = Allocator()) : size_(count), alloc_(alloc) {
    Node* node = createNode(value);
    fakeNode_.next_ = static_cast<BaseNode*>(node);
    node->prev_ = &fakeNode_;
    for (auto i = 0; i < count - 1; ++i) {
      Node* tmp = createNode(value);
      node->next_ = static_cast<BaseNode*>(tmp);
      tmp->prev_ = static_cast<BaseNode*>(node);
      node = tmp;
    }
    node->next_ = &fakeNode_;
    fakeNode_.prev_ = static_cast<BaseNode*>(node);
  }
  explicit List(size_t count, const Allocator& alloc) : size_(count), alloc_(alloc) {
    Node* head = createNode();
    fakeNode_.next_ = static_cast<BaseNode*>(head);
    head->prev_ = &fakeNode_;
    Node* temp = head;
    for (auto i = 0; i < count - 1; ++i) {
      Node* tmp = createNode();
      temp->next_ = static_cast<BaseNode*>(tmp);
      tmp->prev_ = static_cast<BaseNode*>(temp);
      temp = tmp;
    }
    temp->next_ = &fakeNode_;
    fakeNode_.prev_ = static_cast<BaseNode*>(temp);
  }
  List(const List& other) {
    size_ = other.size_;
    alloc_ = node_traits::select_on_container_copy_construction(other.alloc_);
    if (size_ != 0) {
      Node* head = createNode();
      fakeNode_.next_ = static_cast<BaseNode*>(head);
      Node* headOther = static_cast<Node*>(other.fakeNode_.next_);
      //head->value_ = std::move(headOther->value_);
      head->value_ = headOther->value_;
      head->prev_ = &fakeNode_;
      Node* temp = head;
      Node* tempOther = headOther;
      for (auto i = 0; i < size_ - 1; ++i) {
        Node* tmp = createNode();
        Node* tmpOther = static_cast<Node*>(static_cast<BaseNode*>(tempOther)->next_);
        //tmp->value_ = std::move(tmpOther->value_);
        tmp->value_ = tmpOther->value_;
        tempOther = tmpOther;
        temp->next_ = static_cast<BaseNode*>(tmp);
        tmp->prev_ = static_cast<BaseNode*>(temp);
        temp = tmp;
      }
      temp->next_ = &fakeNode_;
      fakeNode_.prev_ = static_cast<BaseNode*>(temp);
    }
  }
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator()) : alloc_(alloc) {
    size_ = init.size();
    if (size_ != 0) {
      auto iter = init.begin();
      Node* head = createNode(*iter);
      fakeNode_.next_ = static_cast<BaseNode*>(head);

      //head->value_ = std::move(*iter);
      ++iter;
      head->prev_ = &fakeNode_;
      Node* temp = head;

      for (auto i = 0; i < size_ - 1; ++i) {
        Node* tmp = createNode(*iter);
        //tmp->value_ = std::move(*iter);
        ++iter;
        temp->next_ = static_cast<BaseNode*>(tmp);
        tmp->prev_ = static_cast<BaseNode*>(temp);
        temp = tmp;
      }
      temp->next_ = &fakeNode_;
      fakeNode_.prev_ = static_cast<BaseNode*>(temp);
    }
  }
  List& operator=(const List& other) {
    if (this == &other) return *this;
    if (node_traits::propagate_on_container_copy_assignment::value && alloc_ != other.alloc_) {
      alloc_ = other.alloc_;
    }
    BaseNode tempFakeNode;
    try {
      Node* headOther = static_cast<Node*>(other.fakeNode_.next_);
      Node* head = createNode((*headOther).value_);
      //head->value_ = std::move(headOther->value_);
      //head->value_ = headOther->value_;
      tempFakeNode.next_ = static_cast<BaseNode*>(head);
      head->prev_ = &tempFakeNode;
      Node* temp = head;
      Node* tempOther = static_cast<Node*>(static_cast<BaseNode*>(headOther)->next_);
      for (int i = 0; i < other.size_ - 1; ++i) {
        Node* tmp = createNode((*tempOther).value_);
        //tmp->value_ = std::move(tempOther->value_);
        //tmp->value_ = tempOther->value_;
        tmp->prev_ = static_cast<BaseNode*>(temp);
        temp->next_ = static_cast<BaseNode*>(tmp);
        temp = tmp;
        tempOther = static_cast<Node*>(static_cast<BaseNode*>(tempOther)->next_);
      }
      temp->next_ = &tempFakeNode;
      tempFakeNode.prev_ = static_cast<BaseNode*>(temp);

      head = static_cast<Node*>(fakeNode_.next_);
      for (int i = 0; i < size_; ++i) {
        BaseNode* tmp = static_cast<BaseNode*>(head)->next_;
        destroyNode(head);
        head = static_cast<Node*>(tmp);
      }
      fakeNode_ = tempFakeNode;
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
      Node* temp = static_cast<Node*>(fakeNode_.next_);
      for (auto i = 0; i < size_; ++i) {
        BaseNode* tmp = static_cast<BaseNode*>(temp)->next_;
        destroyNode(temp);
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
    return *(static_cast<Node*>(fakeNode_.next_)->value_);
  }
  const T& front() const {
    return *(static_cast<Node*>(fakeNode_.next_)->value_);
  }
  T& back() {
    return *(static_cast<Node*>(fakeNode_.prev_)->value_);
  }
  const T& back() const {
    return *(static_cast<Node*>(fakeNode_.prev_)->value_);
  }
  void push_back(const T& value) {
    Node* node;
    try {
      node = createNode(value);
      Node* tail = static_cast<Node*>(fakeNode_.prev_);
      tail->next_ = static_cast<BaseNode*>(node);
      node->prev_ = static_cast<BaseNode*>(tail);
      node->next_ = &fakeNode_;
      fakeNode_.prev_ = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroyNode(node);
      throw;
    }
    ++size_;
  }
  void push_front(const T& value) {
    Node* node;
    try {
      node = createNode(value);
      Node* head = static_cast<Node*>(fakeNode_.next_);
      head->prev_ = static_cast<BaseNode*>(node);
      node->next_ = static_cast<BaseNode*>(head);
      node->prev_ = &fakeNode_;
      fakeNode_.next_ = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroyNode(node);
      throw;
    }
    ++size_;
  }
  void push_back(T&& value) {
    Node* node;
    try {
      node = createNode(value);
      Node* tail = static_cast<Node*>(fakeNode_.prev_);
      tail->next_ = static_cast<BaseNode*>(node);
      node->prev_ = static_cast<BaseNode*>(tail);
      node->next_ = &fakeNode_;
      fakeNode_.prev_ = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroyNode(node);
      throw;
    }
    ++size_;
  }
  void push_front(T&& value) {
    Node* node;
    try {
      node = createNode(value);
      Node* head = static_cast<Node*>(fakeNode_.next_);
      head->prev_ = static_cast<BaseNode*>(node);
      node->next_ = static_cast<BaseNode*>(head);
      node->prev_ = &fakeNode_;
      fakeNode_.next_ = static_cast<BaseNode*>(node);
    }
    catch (...) {
      destroyNode(node);
      throw;
    }
    ++size_;
  }
  void pop_back() {
    Node* node = static_cast<Node*>(fakeNode_.prev_);
    Node* tail = static_cast<Node*>(static_cast<BaseNode*>(node)->prev_);
    destroyNode(node);
    tail->next_ = &fakeNode_;
    fakeNode_.prev_ = static_cast<BaseNode*>(tail);
    --size_;
  }
  void pop_front() {
    Node* node = static_cast<Node*>(fakeNode_.next_);
    Node* head = static_cast<Node*>(static_cast<BaseNode*>(node)->next_);
    destroyNode(node);
    head->prev_ = &fakeNode_;
    fakeNode_.next_ = static_cast<BaseNode*>(head);
    --size_;
  }
  node_allocator get_allocator() {
    return alloc_;
  }

  template<bool IsConst>
  class common_iterator {
  public:
    using value_type = std::conditional_t<IsConst, const T, T>;

    using pointer = value_type*;
    using iterator_category = std::bidirectional_iterator_tag;
    using reference = value_type&;
    common_iterator() = default;
    common_iterator(Node* node) : node_(node) {}
    common_iterator(const common_iterator& other) = default;
    common_iterator& operator=(const common_iterator& other) = default;
    ~common_iterator() = default;
    pointer operator->() {
      return (node_->value_);
    }
    reference operator*() {
      return *(node_->value_);
    }
    const T* operator->() const {
      return (node_->value_);
    }
    const T& operator*() const {
      return *(node_->value_);
    }
    common_iterator& operator++() {
      node_ = node_->next;
      return *this;
    }
    common_iterator& operator--() {
      node_ = node_->prev_;
      return *this;
    }
    common_iterator operator++(int) {
      common_iterator<IsConst> copy = *this;
      ++(*this);
      return copy;
    }
    common_iterator operator--(int) {
      common_iterator<IsConst> copy = *this;
      --(*this);
      return copy;
    }

    bool operator==(const common_iterator<IsConst>& other) {
      return node_ == other.node_;
    }
    bool operator!=(const common_iterator<IsConst>& other) {
      return !(*this == other);
    }

  private:
    Node* node_ = nullptr;
  };

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  iterator begin() noexcept {
    return iterator(static_cast<Node*>(fakeNode_.next_));
  }
  iterator end() noexcept {
    return iterator(&fakeNode_);
  }
  const_iterator cbegin() const noexcept {
    return const_iterator(static_cast<Node*>(fakeNode_.next_));
  }
  const_iterator cend() const noexcept {
    return const_iterator(&fakeNode_);
  }
  iterator rbegin() noexcept {
    return iterator(static_cast<Node*>(fakeNode_.prev_));
  }
  iterator rend() noexcept {
    return iterator(&fakeNode_);
  }
  const_iterator rcbegin() const noexcept {
    return const_iterator(static_cast<Node*>(fakeNode_.prev_));
  }
  const_iterator rcend() const noexcept {
    return const_iterator(&fakeNode_);
  }
};