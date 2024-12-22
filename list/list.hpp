#include <initializer_list>
#include <iostream>

template <typename T, typename Alloc = std::allocator<T>>
class List {
 public:
  List() = default;
  List(size_t count, const T& value, const Alloc& alloc = Alloc());
  explicit List(size_t count, const Alloc& alloc = Alloc());
  List(const List& other);
  List(std::initializer_list<T> init, const Alloc& alloc = Alloc());
  ~List();
  List& operator=(const List& other);
  T& front();
  const T& front() const;
  T& back();
  const T& back() const;
  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }
  void push_back(const T& elem);
  void push_front(const T& elem);
  void pop_back();
  void pop_front();
  using value_type = T;
  using allocator_type = Alloc;
  allocator_type get_allocator() const { return alloc_; }

  template <bool IsConst>
  class Iterator;

  using iterator = List::Iterator<false>;
  using const_iterator = List::Iterator<true>;
  using rev_iterator = std::reverse_iterator<iterator>;
  iterator begin() const;
  iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  rev_iterator rbegin() const;
  rev_iterator rend() const;

 private:
  struct FakeNode {
    FakeNode* prev;
    FakeNode* next;
  };
  struct Node : FakeNode {
    explicit Node(const T& value_add) : value(value_add) {}
    Node() = default;
    T value;
  };
  void second_part(const List& other);
  void little_destroy(size_t ind, size_t jind, Node* curr);

  FakeNode base_ = {&base_, &base_};
  size_t size_ = 0;
  using alloc_node =
      typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using alloc_traits_node =
      typename std::allocator_traits<Alloc>::template rebind_traits<Node>;
  alloc_node alloc_;
};

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const T& value, const Alloc& alloc)
    : alloc_(alloc) {
  size_t ind = 0;
  size_t jind = 0;
  Node* first;
  try {
    base_.prev = &base_;
    base_.next = &base_;
    if (count != 0) {
      first = alloc_traits_node::allocate(alloc_, 1);
      ++ind;
      FakeNode* before = static_cast<FakeNode*>(first);
      alloc_traits_node::construct(alloc_, first, value);
      ++jind;
      Node* curr;
      while (ind < count) {
        curr = alloc_traits_node::allocate(alloc_, 1);
        ++ind;
        alloc_traits_node::construct(alloc_, curr, value);
        ++jind;
        before->next = static_cast<FakeNode*>(curr);
        static_cast<FakeNode*>(curr)->prev = before;
        before = static_cast<FakeNode*>(curr);
      }
      static_cast<FakeNode*>(curr)->next = &base_;
      first->prev = &base_;
      base_.prev = static_cast<FakeNode*>(curr);
      base_.next = first;
    }
    size_ = count;
  } catch (...) {
    little_destroy(ind, jind, first);
    base_.next = &base_;
    base_.prev = &base_;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(size_t count, const Alloc& alloc) : alloc_(alloc) {
  size_t ind = 0;
  size_t jind = 0;
  Node* first;
  try {
    if (count != 0) {
      first = alloc_traits_node::allocate(alloc_, 1);
      ++ind;
      FakeNode* before = first;
      alloc_traits_node::construct(alloc_, first);
      ++jind;
      Node* curr;
      while (ind < count) {
        curr = alloc_traits_node::allocate(alloc_, 1);
        ++ind;
        before->next = static_cast<FakeNode*>(curr);
        alloc_traits_node::construct(alloc_, curr);
        ++jind;
        static_cast<FakeNode*>(curr)->prev = before;
        before = static_cast<FakeNode*>(curr);
      }
      static_cast<FakeNode*>(curr)->next = &base_;
      first->prev = &base_;
      base_.prev = static_cast<FakeNode*>(curr);
      base_.next = first;
    }
    size_ = count;
  } catch (...) {
    little_destroy(ind, jind, first);
    base_.next = &base_;
    base_.prev = &base_;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(const List<T, Alloc>& other)
    : alloc_(alloc_traits_node::select_on_container_copy_construction(
          other.alloc_)) {
  size_t ind = 0;
  size_t jind = 0;
  Node* first;
  try {
    if (other.size_ != 0) {
      first = alloc_traits_node::allocate(alloc_, 1);
      ++ind;
      FakeNode* before = first;
      const FakeNode* curr_value = other.base_.next;
      alloc_traits_node::construct(alloc_, first,
                                   static_cast<const Node*>(curr_value)->value);
      ++jind;
      Node* curr;
      while (ind < other.size_) {
        curr = alloc_traits_node::allocate(alloc_, 1);
        ++ind;
        before->next = static_cast<FakeNode*>(curr);
        alloc_traits_node::construct(
            alloc_, curr, static_cast<const Node*>(curr_value->next)->value);
        ++jind;
        curr->prev = before;
        curr->next = &base_;
        before = curr;
        curr_value = curr_value->next;
      }
      static_cast<FakeNode*>(curr)->next = &base_;
      first->prev = &base_;
      base_.prev = curr;
      base_.next = first;
    }
    size_ = other.size_;
  } catch (...) {
    little_destroy(ind, jind, first);
    base_.next = &base_;
    base_.prev = &base_;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& other) {
  alloc_node old_one = alloc_;
  size_t curr_size = size_;
  if (alloc_traits_node::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
    size_t ind = 0;
    size_t jind = 0;
    Node* first;
    Node* curr;
    try {
      if (other.size_ != 0) {
        first = alloc_traits_node::allocate(alloc_, 1);
        ++ind;
        const FakeNode* curr_value = other.base_.next;
        alloc_traits_node::construct(
            alloc_, first, static_cast<const Node*>(curr_value)->value);
        ++jind;
        FakeNode* before = static_cast<FakeNode*>(first);
        while (ind < other.size_) {
          curr = alloc_traits_node::allocate(alloc_, 1);
          ++ind;
          before->next = static_cast<FakeNode*>(curr);
          alloc_traits_node::construct(
              alloc_, curr, static_cast<const Node*>(curr_value->next)->value);
          ++jind;
          static_cast<FakeNode*>(curr)->prev = before;
          before = static_cast<FakeNode*>(curr);
          curr_value = curr_value->next;
        }
        size_t curr_size = size_;
        for (size_t i = 0; i < curr_size; ++i) {
          this->pop_front();
        }
        static_cast<FakeNode*>(curr)->next = &base_;
        first->prev = &base_;
        base_.prev = static_cast<FakeNode*>(curr);
        base_.next = first;
        size_ = other.size_;
      }
    } catch (...) {
      little_destroy(ind, jind, first);
      alloc_ = old_one;
      size_ = curr_size;
    }
  } else {
    second_part(other);
  }
  return *this;
}

template <typename T, typename Alloc>
void List<T, Alloc>::little_destroy(size_t ind, size_t jind,
                                    List<T, Alloc>::Node* curr) {
  size_t cat = 0;
  Node* next_curr = curr;
  FakeNode* after;
  if (cat < jind || cat < ind) {
    after = curr;
  }
  ++cat;
  while (cat <= jind || cat <= ind) {
    if (cat < jind || cat < ind) {
      after = after->next;
    }
    if (cat <= jind) {
      alloc_traits_node::destroy(alloc_, next_curr);
    }
    if (cat <= ind) {
      alloc_traits_node::deallocate(alloc_, next_curr, 1);
    }
    if (cat < jind || cat < ind) {
      next_curr = static_cast<Node*>(after);
    }
    ++cat;
  }
  size_ = 0;
}

template <typename T, typename Alloc>
void List<T, Alloc>::second_part(const List<T, Alloc>& other) {
  while (other.size_ < size_) {
    this->pop_back();
  }
  FakeNode* curr = base_.next;
  const FakeNode* curr_add = other.base_.next;
  for (size_t i = 0; i < size_; ++i) {
    static_cast<Node*>(curr)->value = static_cast<const Node*>(curr_add)->value;
    curr = curr->next;
    curr_add = curr_add->next;
  }
  while (other.size_ > size_) {
    this->push_back(static_cast<const Node*>(curr_add)->value);
    curr_add = curr_add->next;
  }
}

template <typename T, typename Alloc>
List<T, Alloc>::List(std::initializer_list<T> init, const Alloc& alloc)
    : alloc_(alloc) {
  base_.prev = &base_;
  base_.next = &base_;
  if (init.size() != 0) {
    Node* first = alloc_traits_node::allocate(alloc_, 1);
    FakeNode* before = first;
    auto iter = init.begin();
    alloc_traits_node::construct(alloc_, first, *(iter));
    Node* curr;
    ++iter;
    for (; iter != init.end(); ++iter) {
      curr = alloc_traits_node::allocate(alloc_, 1);
      alloc_traits_node::construct(alloc_, curr, *iter);
      before->next = static_cast<FakeNode*>(curr);
      static_cast<FakeNode*>(curr)->prev = before;
      before = static_cast<FakeNode*>(curr);
    }
    static_cast<FakeNode*>(curr)->next = &base_;
    first->prev = &base_;
    base_.prev = static_cast<FakeNode*>(curr);
    base_.next = first;
  }
  size_ = init.size();
}

template <typename T, typename Alloc>
List<T, Alloc>::~List() {
  size_t curr_size = size_;
  for (size_t i = 0; i < curr_size; ++i) {
    this->pop_front();
  }
}

template <typename T, typename Alloc>
T& List<T, Alloc>::front() {
  return static_cast<Node*>(base_.next)->value;
}

template <typename T, typename Alloc>
const T& List<T, Alloc>::front() const {
  return static_cast<Node*>(base_.next)->value;
}

template <typename T, typename Alloc>
T& List<T, Alloc>::back() {
  return static_cast<Node*>(base_.prev)->value;
}

template <typename T, typename Alloc>
const T& List<T, Alloc>::back() const {
  return static_cast<Node*>(base_.prev)->value;
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& elem) {
  bool allocated = false;
  Node* last;
  try {
    last = alloc_traits_node::allocate(alloc_, 1);
    allocated = true;
    alloc_traits_node::construct(alloc_, last, elem);
    base_.prev->next = last;
    last->prev = base_.prev;
    last->next = &base_;
    base_.prev = last;
    ++size_;
  } catch (...) {
    if (allocated) {
      alloc_traits_node::deallocate(alloc_, last, 1);
    }
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& elem) {
  bool allocated = false;
  Node* last;
  try {
    last = alloc_traits_node::allocate(alloc_, 1);
    allocated = true;
    alloc_traits_node::construct(alloc_, last, elem);
    base_.next->prev = static_cast<FakeNode*>(last);
    static_cast<FakeNode*>(last)->next = base_.next;
    static_cast<FakeNode*>(last)->prev = &base_;
    base_.next = static_cast<FakeNode*>(last);
    ++size_;
  } catch (...) {
    if (allocated) {
      alloc_traits_node::deallocate(alloc_, last, 1);
    }
  }
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  FakeNode* last = base_.prev->prev;
  alloc_traits_node::destroy(alloc_, static_cast<Node*>(base_.prev));
  alloc_traits_node::deallocate(alloc_, static_cast<Node*>(base_.prev), 1);
  base_.prev = last;
  last->next = &base_;
  --size_;
}

template <typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  FakeNode* last = base_.next->next;
  alloc_traits_node::destroy(alloc_, static_cast<Node*>(base_.next));
  alloc_traits_node::deallocate(alloc_, static_cast<Node*>(base_.next), 1);
  base_.next = last;
  last->prev = &base_;
  --size_;
}

template <typename T, typename Alloc>
template <bool IsConst>
class List<T, Alloc>::Iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using cond_type = std::conditional_t<IsConst, const T, T>;
  using value_type = cond_type;
  using pointer = cond_type*;
  using reference = cond_type&;
  Iterator(FakeNode* const kPtr) : ptr_(kPtr) {}
  reference operator*() const { return static_cast<Node*>(ptr_)->value; }
  pointer operator->() const { return &static_cast<Node*>(ptr_)->value; }
  Iterator operator++(int);
  Iterator& operator++();
  Iterator operator--(int);
  Iterator& operator--();
  bool operator==(const Iterator& scd) const { return ptr_ == scd.ptr_; }
  bool operator!=(const Iterator& scd) const { return !(ptr_ == scd.ptr_); }

 private:
  FakeNode* ptr_;
};

template <typename T, typename Alloc>
template <bool IsConst>
List<T, Alloc>::Iterator<IsConst>&
List<T, Alloc>::Iterator<IsConst>::operator++() {
  ptr_ = ptr_->next;
  return *this;
}

template <typename T, typename Alloc>
template <bool IsConst>
List<T, Alloc>::Iterator<IsConst> List<T, Alloc>::Iterator<IsConst>::operator++(
    int) {
  Iterator tmp = *this;
  ++tmp;
  return tmp;
}

template <typename T, typename Alloc>
template <bool IsConst>
List<T, Alloc>::Iterator<IsConst>&
List<T, Alloc>::Iterator<IsConst>::operator--() {
  ptr_ = ptr_->prev;
  return *this;
}

template <typename T, typename Alloc>
template <bool IsConst>
List<T, Alloc>::Iterator<IsConst> List<T, Alloc>::Iterator<IsConst>::operator--(
    int) {
  Iterator tmp = *this;
  --tmp;
  return tmp;
}

template <typename T, typename Alloc>
List<T, Alloc>::iterator List<T, Alloc>::begin() const {
  return iterator(base_.next);
}

template <typename T, typename Alloc>
List<T, Alloc>::iterator List<T, Alloc>::end() const {
  return iterator(base_.next->prev);
}

template <typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::cbegin() const {
  return const_iterator(base_.next);
}

template <typename T, typename Alloc>
List<T, Alloc>::const_iterator List<T, Alloc>::cend() const {
  return const_iterator(base_.next->prev);
}

template <typename T, typename Alloc>
List<T, Alloc>::rev_iterator List<T, Alloc>::rbegin() const {
  return std::make_reverse_iterator(base_.next->prev);
}

template <typename T, typename Alloc>
List<T, Alloc>::rev_iterator List<T, Alloc>::rend() const {
  return std::make_reverse_iterator(iterator(base_.next));
}
