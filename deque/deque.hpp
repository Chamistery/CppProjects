#include <initializer_list>
#include <stdexcept>

template <typename T, typename Alloc = std::allocator<T>>
class Deque {
 public:
  Deque();
  Deque(const Alloc& alloc) : alloc_(alloc){};
  Deque(const Deque& to_copy);
  Deque(Deque&& to_copy);
  Deque(size_t count, const Alloc& alloc = Alloc());
  Deque(size_t count, const T& value, const Alloc& alloc = Alloc());
  Deque(std::initializer_list<T> init, const Alloc& alloc = Alloc());
  ~Deque();
  Deque& operator=(const Deque& other);
  Deque& operator=(Deque&& other);
  size_t size() const;
  bool empty() const { return last_elem_ == fst_elem_; }
  T& operator[](size_t ind);
  const T& operator[](size_t ind) const;
  T& at(size_t ind);
  const T& at(size_t ind) const;
  void push_back(const T& elem);
  void push_back(T&& elem);
  void push_front(const T& elem);
  void push_front(T&& elem);
  template <class... Args>
  void emplace_back(Args&&... args);
  template <class... Args>
  void emplace_front(Args&&... args);
  void pop_back();
  void pop_front();
  using value_type = T;
  using allocator_type = Alloc;
  allocator_type get_allocator() const { return alloc_; }

  template <bool IsConst>
  class Iterator;
  using iterator = Deque::Iterator<false>;
  using const_iterator = Deque::Iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using reverse_const_iterator = std::reverse_iterator<const_iterator>;
  iterator begin();
  iterator end();
  const_iterator cbegin();
  const_iterator cend();
  reverse_iterator rbegin();
  reverse_iterator rend();
  reverse_const_iterator rcbegin();
  reverse_const_iterator rcend();
  void insert(iterator iter, const T& elem);
  void erase(iterator iter);

 private:
  void reallock();
  void swap(Deque& other);

  allocator_type alloc_;
  using alloc_traits = std::allocator_traits<Alloc>;
  using alloc_buckets =
      typename std::allocator_traits<Alloc>::template rebind_alloc<T*>;
  alloc_buckets alloct_;
  using alloc_traits_t =
      typename std::allocator_traits<Alloc>::template rebind_traits<T*>;
  size_t chunks_ = 0;
  static const size_t kLenght = 7;
  size_t fst_chunk_ = 0;
  size_t last_chunk_ = 0;
  size_t fst_elem_ = 0;
  size_t last_elem_ = 0;
  T** buckets_;
};

template <typename T, typename Alloc>
template <bool IsConst>
class Deque<T, Alloc>::Iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using cond_type = std::conditional_t<IsConst, const T, T>;
  using value_type = cond_type;
  using pointer = cond_type*;
  using reference = cond_type&;

  Iterator(size_t started, pointer ptr, T** buckets_deq, size_t size)
      : start_(static_cast<int>(started)),
        ptr_(ptr),
        buckets_(buckets_deq),
        size_(size) {}
  reference operator*() const { return *ptr_; }
  pointer operator->() const { return ptr_; }
  Iterator& operator-=(int num);
  Iterator& operator+=(int num);
  Iterator operator-(int num) const;
  Iterator operator+(int num) const;
  Iterator operator++(int);
  Iterator& operator++();
  Iterator operator--(int);
  Iterator& operator--();
  bool operator==(const Iterator& scd) const { return start_ == scd.start_; }
  bool operator!=(const Iterator& scd) const { return !(start_ == scd.start_); }
  bool operator<(const Iterator& scd) const { return start_ < scd.start_; }
  bool operator<=(const Iterator& scd) const { return start_ <= scd.start_; }
  bool operator>(const Iterator& scd) const { return start_ > scd.start_; }
  bool operator>=(const Iterator& scd) const { return start_ >= scd.start_; }
  difference_type operator-(Iterator scd) const { return start_ - scd.start_; }

 private:
  int start_ = 0;
  pointer ptr_;
  T** buckets_;
  size_t size_;
};

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque() = default;

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque(Deque<T, Alloc>&& to_copy)
    : alloc_(std::move(to_copy.alloc_)),
      alloct_(std::move(to_copy.alloc_)),
      chunks_(std::move(to_copy.chunks_)),
      fst_chunk_(std::move(to_copy.fst_chunk_)),
      last_chunk_(std::move(to_copy.last_chunk_)),
      fst_elem_(std::move(to_copy.fst_elem_)),
      last_elem_(std::move(to_copy.last_elem_)),
      buckets_(std::move(to_copy.buckets_)) {
  to_copy.chunks_ = 0;
  to_copy.fst_chunk_ = 0;
  to_copy.last_chunk_ = 0;
  to_copy.fst_elem_ = 0;
  to_copy.last_elem_ = 0;
  to_copy.buckets_ = nullptr;
}

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque(const Deque& to_copy)
    : alloc_(
          alloc_traits::select_on_container_copy_construction(to_copy.alloc_)),
      alloct_(
          alloc_traits::select_on_container_copy_construction(to_copy.alloc_)),
      chunks_(to_copy.chunks_),
      fst_chunk_(to_copy.fst_chunk_),
      last_chunk_(to_copy.last_chunk_),
      fst_elem_(to_copy.fst_elem_),
      last_elem_(to_copy.last_elem_) {
  if (chunks_ == 0) {
    return;
  }
  size_t ind = fst_elem_;
  size_t ind_bucket = fst_chunk_;
  try {
    buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
    for (; ind_bucket <= last_chunk_; ++ind_bucket) {
      buckets_[ind_bucket] = alloc_traits::allocate(alloc_, kLenght);
      if (ind_bucket != last_chunk_) {
        for (size_t elem = 0; elem < kLenght; ++elem) {
          alloc_traits::construct(alloc_, buckets_[ind_bucket] + elem,
                                  to_copy.buckets_[ind_bucket][elem]);
          ++ind;
        }
      } else {
        for (size_t elem = 0; elem <= last_elem_ % kLenght; ++elem) {
          alloc_traits::construct(alloc_, buckets_[ind_bucket] + elem,
                                  to_copy.buckets_[ind_bucket][elem]);
          ++ind;
        }
      }
    }
  } catch (...) {
    for (size_t important = fst_elem_; important < ind; ++important) {
      alloc_traits::destroy(
          alloc_, buckets_[important / kLenght] + important % kLenght);
    }
    for (size_t i = fst_chunk_; i <= ind_bucket; ++i) {
      alloc_traits::deallocate(alloc_, buckets_[i], kLenght);
    }
    chunks_ = 0;
    alloc_traits_t::deallocate(alloct_, buckets_, chunks_);
    throw;
  }
}

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque(std::initializer_list<T> init, const Alloc& alloc)
    : alloc_(alloc),
      alloct_(alloc),
      chunks_(((init.size() + kLenght - 1) / kLenght) * 2),
      fst_chunk_(chunks_ / 4),
      last_chunk_((chunks_ - 1) * 3 / 4),
      fst_elem_(fst_chunk_ * kLenght),
      last_elem_(fst_elem_ + init.size() - 1) {
  buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
  for (size_t i = fst_chunk_; i <= last_chunk_; ++i) {
    buckets_[i] = alloc_traits::allocate(alloc_, kLenght);
    if (i != last_chunk_) {
      for (size_t j = 0; j < kLenght; ++j) {
        alloc_traits::construct(alloc_, buckets_[i] + j,
                                *(init.begin() + i * kLenght + j));
      }
    } else {
      for (size_t j = 0; j <= last_elem_ % kLenght; ++j) {
        alloc_traits::construct(alloc_, buckets_[i] + j,
                                *(init.begin() + i * kLenght + j));
      }
    }
  }
}

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque(size_t count, const Alloc& alloc)
    : alloc_(alloc),
      alloct_(alloc),
      chunks_(((count + kLenght - 1) / kLenght) * 2),
      fst_chunk_(chunks_ / 4),
      last_chunk_((chunks_ - 1) * 3 / 4),
      fst_elem_(fst_chunk_ * kLenght),
      last_elem_(fst_elem_ + count - 1) {
  size_t ind = fst_elem_;
  size_t ind_buck = fst_chunk_;
  try {
    buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
    for (; ind_buck <= last_chunk_; ++ind_buck) {
      buckets_[ind_buck] = alloc_traits::allocate(alloc_, kLenght);
      if (ind_buck != last_chunk_) {
        for (size_t j = 0; j < kLenght; ++j) {
          alloc_traits::construct(alloc_, buckets_[ind_buck] + j);
          ++ind;
        }
      } else {
        for (size_t j = 0; j <= last_elem_ % kLenght; ++j) {
          alloc_traits::construct(alloc_, buckets_[ind_buck] + j);
          ++ind;
        }
      }
    }
  } catch (...) {
    for (size_t important = fst_elem_; important < ind; ++important) {
      alloc_traits::destroy(
          alloc_, buckets_[important / kLenght] + important % kLenght);
    }
    for (size_t i = fst_chunk_; i <= ind_buck; ++i) {
      alloc_traits::deallocate(alloc_, buckets_[i], kLenght);
    }
    chunks_ = 0;
    alloc_traits_t::deallocate(alloct_, buckets_, chunks_);
    throw;
  }
}

template <typename T, typename Alloc>
Deque<T, Alloc>::Deque(size_t count, const T& value, const Alloc& alloc)
    : alloc_(alloc),
      alloct_(alloc),
      chunks_(((count + kLenght - 1) / kLenght) * 2),
      fst_chunk_(chunks_ / 4),
      last_chunk_((chunks_ - 1) * 3 / 4),
      fst_elem_(fst_chunk_ * kLenght),
      last_elem_(fst_elem_ + count - 1) {
  size_t ind = fst_elem_;
  size_t ind_buck = fst_chunk_;
  try {
    buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
    for (; ind_buck <= last_chunk_; ++ind_buck) {
      buckets_[ind_buck] = alloc_traits::allocate(alloc_, kLenght);
      if (ind_buck != last_chunk_) {
        for (size_t j = 0; j < kLenght; ++j) {
          alloc_traits::construct(alloc_, buckets_[ind_buck] + j, value);
          ++ind;
        }
      } else {
        for (size_t j = 0; j <= last_elem_ % kLenght; ++j) {
          alloc_traits::construct(alloc_, buckets_[ind_buck] + j, value);
          ++ind;
        }
      }
    }
  } catch (...) {
    for (size_t important = fst_elem_; important < ind; ++important) {
      alloc_traits::destroy(
          alloc_, buckets_[important / kLenght] + important % kLenght);
    }
    for (size_t i = fst_chunk_; i <= ind_buck; ++i) {
      alloc_traits::deallocate(alloc_, buckets_[i], kLenght);
    }
    chunks_ = 0;
    alloc_traits_t::deallocate(alloct_, buckets_, chunks_);
    throw;
  }
}

template <typename T, typename Alloc>
Deque<T, Alloc>::~Deque() {
  if (chunks_ != 0) {
    size_t important = fst_elem_;
    for (; important <= last_elem_; ++important) {
      alloc_traits::destroy(
          alloc_, buckets_[important / kLenght] + important % kLenght);
      if (important % kLenght == kLenght - 1) {
        alloc_traits::deallocate(alloc_, buckets_[important / kLenght],
                                 kLenght);
      }
    }
    if (important % kLenght != 0) {
      alloc_traits::deallocate(alloc_, buckets_[important / kLenght], kLenght);
    }
    alloc_traits_t::deallocate(alloct_, buckets_, chunks_);
  }
}

template <typename T, typename Alloc>
Deque<T, Alloc>& Deque<T, Alloc>::operator=(const Deque<T, Alloc>& other) {
  if (alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
    if (this != &other) {
      Deque<T, Alloc> new_one(other);
      new_one.swap(*this);
    }
  } else {
    while (other.size() < size()) {
      this->pop_back();
    }
    size_t ind = 0;
    for (; ind < size(); ++ind) {
      buckets_[(ind + fst_elem_) / kLenght][(ind + fst_elem_) % kLenght] =
          other[ind];
    }
    while (other.size() > size()) {
      push_back(other[ind]);
    }
  }
  return *this;
}

template <typename T, typename Alloc>
Deque<T, Alloc>& Deque<T, Alloc>::operator=(Deque<T, Alloc>&& other) {
  if (alloc_traits::propagate_on_container_move_assignment::value) {
    alloc_ = std::move(other.alloc_);
    if (this != &other) {
      while (size() != 0) {
        pop_back();
      }
      alloc_ = std::move(other.alloc_);
      buckets_ = std::move(other.buckets_);
      chunks_ = std::move(other.chunks_);
      fst_chunk_ = std::move(other.fst_chunk_);
      last_chunk_ = std::move(other.last_chunk_);
      fst_elem_ = std::move(other.fst_elem_);
      last_elem_ = std::move(other.last_elem_);
    }
  } else {
    if (get_allocator() == other.get_allocator()) {
      while (size() != 0) {
        pop_back();
      }
      buckets_ = std::move(other.buckets_);
      chunks_ = std::move(other.chunks_);
      fst_chunk_ = std::move(other.fst_chunk_);
      last_chunk_ = std::move(other.last_chunk_);
      fst_elem_ = std::move(other.fst_elem_);
      last_elem_ = std::move(other.last_elem_);
    } else {
      alloc_ = std::move(other.alloc_);
      while (other.size() < size()) {
        this->pop_back();
      }
      size_t ind = other.fst_elem_;
      for (; ind < size() + other.fst_elem_; ++ind) {
        buckets_[(ind + fst_elem_) / kLenght][(ind + fst_elem_) % kLenght] =
            std::move(other[ind]);
      }
      while (other.size() > size()) {
        emplace_back(other[ind]);
      }
    }
  }
  other.chunks_ = 0;
  other.fst_chunk_ = 0;
  other.last_chunk_ = 0;
  other.fst_elem_ = 0;
  other.last_elem_ = 0;
  other.last_chunk_ = 0;
  other.buckets_ = nullptr;
  return *this;
}

template <typename T, typename Alloc>
size_t Deque<T, Alloc>::size() const {
  if (last_elem_ == fst_elem_ && chunks_ == 0) {
    return 0;
  }
  return last_elem_ - fst_elem_ + 1;
}

template <typename T, typename Alloc>
T& Deque<T, Alloc>::operator[](size_t ind) {
  size_t curr =
      static_cast<size_t>(ind % kLenght >= (kLenght - fst_elem_ % kLenght));
  return buckets_[(fst_chunk_ + ind / kLenght) + curr]
                 [(fst_elem_ + ind) % kLenght];
}

template <typename T, typename Alloc>
const T& Deque<T, Alloc>::operator[](size_t ind) const {
  size_t curr =
      static_cast<size_t>(ind % kLenght >= (kLenght - fst_elem_ % kLenght));
  return buckets_[(fst_chunk_ + ind / kLenght) + curr]
                 [(fst_elem_ + ind) % kLenght];
}

template <typename T, typename Alloc>
T& Deque<T, Alloc>::at(size_t ind) {
  if (ind > last_elem_ - fst_elem_) {
    throw std::out_of_range("lol1");
  }
  size_t curr =
      static_cast<size_t>(ind % kLenght >= (kLenght - fst_elem_ % kLenght));
  return buckets_[(fst_chunk_ + ind / kLenght) + curr]
                 [(fst_elem_ + ind) % kLenght];
}

template <typename T, typename Alloc>
const T& Deque<T, Alloc>::at(size_t ind) const {
  if (ind > last_elem_ - fst_elem_) {
    throw std::out_of_range("lol2");
  }
  size_t curr =
      static_cast<size_t>(ind % kLenght >= (kLenght - fst_elem_ % kLenght));
  return buckets_[(fst_chunk_ + ind / kLenght) + curr]
                 [(fst_elem_ + ind) % kLenght];
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_back(const T& elem) {
  emplace_back(elem);
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_back(T&& elem) {
  emplace_back(std::move(elem));
}

template <typename T, typename Alloc>
template <class... Args>
void Deque<T, Alloc>::emplace_back(Args&&... args) {
  try {
    if (chunks_ == 0) {
      chunks_ = 1;
      buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
      buckets_[0] = alloc_traits::allocate(alloc_, kLenght);
      alloc_traits::construct(alloc_, buckets_[0], std::forward<Args>(args)...);
      return;
    }
    if (last_elem_ == kLenght * chunks_ - 1) {
      reallock();
    }
    if (last_elem_ % kLenght == (kLenght - 1)) {
      ++last_chunk_;
      buckets_[last_chunk_] = alloc_traits::allocate(alloc_, kLenght);
    }
    ++last_elem_;
    alloc_traits::construct(alloc_,
                            buckets_[last_chunk_] + last_elem_ % kLenght,
                            std::forward<Args>(args)...);
  } catch (...) {
    if (last_elem_ % kLenght == 0) {
      alloc_traits::deallocate(alloc_, buckets_[last_chunk_], kLenght);
      --last_chunk_;
    }
    --last_elem_;
    throw;
  }
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_front(const T& elem) {
  emplace_front(elem);
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::push_front(T&& elem) {
  emplace_front(std::move(elem));
}

template <typename T, typename Alloc>
template <class... Args>
void Deque<T, Alloc>::emplace_front(Args&&... args) {
  if (chunks_ == 0) {
    chunks_ = 1;
    buckets_ = alloc_traits_t::allocate(alloct_, chunks_);
    buckets_[0] = alloc_traits::allocate(alloc_, kLenght);
    alloc_traits::construct(alloc_, buckets_[0], std::forward<Args>(args)...);
    return;
  }
  if (fst_elem_ == 0) {
    reallock();
  }
  if (fst_elem_ % kLenght == 0 && fst_elem_ != 0) {
    --fst_chunk_;
    buckets_[fst_chunk_] = alloc_traits::allocate(alloc_, kLenght);
  }
  --fst_elem_;
  alloc_traits::construct(alloc_, buckets_[fst_chunk_] + (fst_elem_ % kLenght),
                          std::forward<Args>(args)...);
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::pop_back() {
  alloc_traits::destroy(alloc_, buckets_[last_chunk_] + last_elem_ % kLenght);
  if (last_elem_ % kLenght == 0) {
    alloc_traits::deallocate(alloc_, buckets_[last_chunk_], kLenght);
    buckets_[last_chunk_] = nullptr;
    --last_chunk_;
  }
  --last_elem_;
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::pop_front() {
  alloc_traits::destroy(alloc_, buckets_[fst_chunk_] + fst_elem_ % kLenght);
  if (fst_elem_ % kLenght == (kLenght - 1)) {
    alloc_traits::deallocate(alloc_, buckets_[fst_chunk_], kLenght);
    buckets_[fst_chunk_] = nullptr;
    ++fst_chunk_;
  }
  ++fst_elem_;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>&
Deque<T, Alloc>::Iterator<IsConst>::operator-=(int num) {
  start_ -= num;
  ptr_ = *(buckets_[start_ / kLenght] + start_ % kLenght);
  return *this;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>&
Deque<T, Alloc>::Iterator<IsConst>::operator+=(int num) {
  if (num != 0) {
    start_ += num;
    ptr_ = *(buckets_[start_ / kLenght] + start_ % kLenght);
  }
  return *this;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>
Deque<T, Alloc>::Iterator<IsConst>::operator-(int num) const {
  Iterator copy = *this;
  if (num != 0) {
    copy.start_ = start_ - num;
    copy.ptr_ = copy.buckets_[copy.start_ / kLenght] + (copy.start_ % kLenght);
  }
  return copy;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>
Deque<T, Alloc>::Iterator<IsConst>::operator+(int num) const {
  auto copy = *this;
  if (num != 0) {
    copy.start_ = start_ + num;
    if (start_ + num < static_cast<int>(size_) + start_) {
      copy.ptr_ =
          copy.buckets_[copy.start_ / kLenght] + (copy.start_ % kLenght);
    } else {
      pointer new_one = buckets_[(copy.start_ - num) / kLenght] +
                        ((copy.start_ - num) % kLenght);
      copy.ptr_ = new_one + 1;
    }
  }
  return copy;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>
Deque<T, Alloc>::Iterator<IsConst>::operator++(int) {
  Iterator tmp = *this;
  ++*this;
  return tmp;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>&
Deque<T, Alloc>::Iterator<IsConst>::operator++() {
  int old_start = start_;
  ++start_;
  if (start_ < old_start + static_cast<int>(size_)) {
    ptr_ = buckets_[start_ / kLenght] + (start_ % kLenght);
  } else {
    T* new_one = buckets_[(start_ - 1) / kLenght] + ((start_ - 1) % kLenght);
    ptr_ = new_one + 1;
  }
  return *this;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>
Deque<T, Alloc>::Iterator<IsConst>::operator--(int) {
  Iterator tmp = *this;
  --this;
  return tmp;
}

template <typename T, typename Alloc>
template <bool IsConst>
Deque<T, Alloc>::Iterator<IsConst>&
Deque<T, Alloc>::Iterator<IsConst>::operator--() {
  --start_;
  ptr_ = buckets_[start_ / kLenght] + (start_ % kLenght);
  return *this;
}

template <typename T, typename Alloc>
Deque<T, Alloc>::iterator Deque<T, Alloc>::begin() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[fst_elem_ / kLenght][fst_elem_ % kLenght]);
    return iterator(fst_elem_, new_one, buckets_, size());
  }
  return iterator(0, nullptr, buckets_, 0);
}

template <typename T, typename Alloc>
Deque<T, Alloc>::iterator Deque<T, Alloc>::end() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[last_elem_ / kLenght][last_elem_ % kLenght]);
    return iterator(last_elem_ + 1, new_one + 1, buckets_, size());
  }
  return iterator(0, nullptr, buckets_, 0);
}

template <typename T, typename Alloc>
Deque<T, Alloc>::const_iterator Deque<T, Alloc>::cbegin() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[fst_elem_ / kLenght][fst_elem_ % kLenght]);
    return const_iterator(fst_elem_, new_one, buckets_, size());
  }
  return const_iterator(0, nullptr, buckets_, 0);
}

template <typename T, typename Alloc>
Deque<T, Alloc>::const_iterator Deque<T, Alloc>::cend() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[last_elem_ / kLenght][last_elem_ % kLenght]);
    return const_iterator(last_elem_ + 1, new_one + 1, buckets_, size());
  }
  return const_iterator(0, nullptr, buckets_, 0);
}

template <typename T, typename Alloc>
Deque<T, Alloc>::reverse_iterator Deque<T, Alloc>::rbegin() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[last_elem_ / kLenght][last_elem_ % kLenght]);
    return std::make_reverse_iterator(
        iterator(last_elem_ + 1, new_one + 1, buckets_, size()));
  }
  return std::make_reverse_iterator(iterator(0, nullptr, buckets_, 0));
}

template <typename T, typename Alloc>
Deque<T, Alloc>::reverse_iterator Deque<T, Alloc>::rend() {
  if (chunks_ != 0) {
    T* new_one = reinterpret_cast<T*>(
        &buckets_[fst_elem_ / kLenght][fst_elem_ % kLenght]);
    return std::make_reverse_iterator(
        iterator(fst_elem_, new_one, buckets_, size()));
  }
  return std::make_reverse_iterator(iterator(0, nullptr, buckets_, 0));
}

template <typename T, typename Alloc>
Deque<T, Alloc>::reverse_const_iterator Deque<T, Alloc>::rcbegin() {
  T* new_one = reinterpret_cast<T*>(
      &buckets_[last_elem_ / kLenght][last_elem_ % kLenght]);
  return std::make_reverse_iterator(
      const_iterator(last_elem_ + 1, new_one + 1, buckets_, size()));
}

template <typename T, typename Alloc>
Deque<T, Alloc>::reverse_const_iterator Deque<T, Alloc>::rcend() {
  T* new_one =
      reinterpret_cast<T*>(&buckets_[fst_elem_ / kLenght][fst_elem_ % kLenght]);
  return std::make_reverse_iterator(
      const_iterator(fst_elem_, new_one, buckets_, size()));
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::insert(Deque::iterator iter, const T& elem) {
  push_back(elem);
  for (auto ind = end() - 1; ind != iter; --ind) {
    std::swap(*ind, *(ind - 1));
  }
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::erase(Deque::iterator iter) {
  for (auto it = iter; it != this->end(); ++it) {
    std::swap(*it, *(it + 1));
  }
  buckets_[last_chunk_][last_elem_ % kLenght].~T();
  if (last_elem_ % kLenght == 0) {
    alloc_traits::deallocate(alloc_, buckets_[last_chunk_], kLenght);
    buckets_[last_chunk_] = nullptr;
    --last_chunk_;
  }
  --last_elem_;
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::reallock() {
  int cof = 1;
  if (chunks_ == 1) {
    cof = 2;
  }
  T** new_backets = alloc_traits_t::allocate(alloct_, chunks_ * 3 * cof);
  size_t old_chunk = fst_chunk_;
  size_t diff = last_elem_ - fst_elem_;
  fst_chunk_ = chunks_ - chunks_ / 2;
  for (size_t ind = old_chunk; ind <= last_chunk_; ++ind) {
    new_backets[fst_chunk_ + ind - old_chunk] = buckets_[ind];
  }
  alloc_traits_t::deallocate(alloct_, buckets_, chunks_);
  fst_elem_ = fst_chunk_ * kLenght;
  last_elem_ = fst_elem_ + diff;
  last_chunk_ = last_elem_ / kLenght;
  chunks_ *= 3 * cof;
  buckets_ = new_backets;
}

template <typename T, typename Alloc>
void Deque<T, Alloc>::swap(Deque<T, Alloc>& other) {
  std::swap(chunks_, other.chunks_);
  std::swap(fst_elem_, other.fst_elem_);
  std::swap(fst_chunk_, other.fst_chunk_);
  std::swap(last_elem_, other.last_elem_);
  std::swap(last_chunk_, other.last_chunk_);
  std::swap(buckets_, other.buckets_);
}
