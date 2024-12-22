#include "string.hpp"

#include <cmath>
#include <cstring>
#include <iostream>
String::String() = default;
String::String(const char* str)
    : size_(strlen(str)), capacity_(size_ + 1), str_(new char[capacity_]) {
  std::copy(str, str + size_, str_);
  str_[capacity_ - 1] = '\0';
}
String::String(const String& str) : size_(str.size_) {
  CheckCapacity();
  str_ = new char[capacity_];
  std::copy(str.str_, str.str_ + str.size_, str_);
}
String& String::operator=(const String& str) {
  if (str_ != str.str_) {
    size_ = str.size_;
    capacity_ = str.capacity_;
    delete[] str_;
    str_ = new char[capacity_];
    std::copy(str.str_, str.str_ + str.size_, str_);
  }
  return *this;
}
String& String::operator=(const char* str) {
  size_t capacity = capacity_;
  size_ = strlen(str);
  CheckCapacity();
  if (capacity < capacity_) {
    delete[] str_;
    str_ = new char[capacity_];
  }
  std::copy(str, str + size_, str_);
  return *this;
}
String::String(size_t size, char character) : size_(size) {
  CheckCapacity();
  str_ = new char[capacity_];
  for (size_t i = 0; i < size; ++i) {
    str_[i] = character;
  }
}

void String::CheckCapacity() {
  if (capacity_ < size_) {
    capacity_ = pow(2, ceil(std::log2(size_)));
  }
}
void String::RealizeMemory(size_t size) {
  if (size != 0) {
    char* str = new char[capacity_];
    std::copy(str_, str_ + size, str);
    delete[] str_;
    str_ = str;
  }
}
void String::PushBack(char character) {
  ++size_;
  size_t capacity = capacity_;
  CheckCapacity();
  if (size_ != 1) {
    if (capacity != capacity_) {
      RealizeMemory(size_ - 1);
    }
  } else {
    str_ = new char[1];
  }
  str_[size_ - 1] = character;
}
void String::PopBack() {
  if (size_ != 0) {
    --size_;
  }
}
void String::Resize(size_t new_size) {
  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      str_[i] = '\0';
    }
  }
  size_t size = size_;
  size_ = new_size;
  size_t capacity = capacity_;
  CheckCapacity();
  if (capacity != capacity_) {
    RealizeMemory(size);
  }
}
void String::Resize(size_t new_size, char character) {
  if (new_size < size_) {
    for (size_t i = new_size; i < size_; ++i) {
      str_[i] = '\0';
    }
  }
  size_t size = size_;
  size_t capacity = capacity_;
  Resize(new_size);
  if (size_ > size) {
    for (size_t i = size; i < size_; ++i) {
      str_[i] = character;
    }
  }
}
void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    capacity_ = new_cap;
    RealizeMemory(size_);
  }
}
void String::ShrinkToFit() {
  if (capacity_ > size_) {
    capacity_ = size_;
    RealizeMemory(size_);
  }
}
void String::Swap(String& other) {
  std::swap(other.str_, str_);
  std::swap(other.size_, size_);
  std::swap(other.capacity_, capacity_);
}
bool String::operator==(const String& str) const {
  bool result = true;
  if (str.size_ == size_) {
    for (int i = 0; i < static_cast<int>(size_); ++i) {
      if ((*this)[i] != *(str.str_ + i)) {
        result = false;
        return result;
      }
    }
  } else {
    result = false;
    return result;
  }
  return result;
}

bool String::operator>(const String& str) const {
  size_t min = std::min(str.size_, size_);
  for (int i = 0; i < static_cast<int>(min); ++i) {
    if ((*this)[i] < *(str.str_ + i)) {
      return false;
    }
    if ((*this)[i] > *(str.str_ + i)) {
      return true;
    }
  }
  return size_ > str.size_;
}

String& String::operator+=(const String& str) {
  size_t size = size_;
  size_ += str.size_;
  size_t capacity = capacity_;
  CheckCapacity();
  if (capacity != capacity_) {
    RealizeMemory(size);
  }
  std::copy(str.str_, str.str_ + str.size_, str_ + size);
  return (*this);
}
String String::operator+(const String& str) const {
  String new_str = *this;
  new_str += str;
  return new_str;
}
String String::operator*(int num) const {
  String new_string = *this;
  new_string *= num;
  return new_string;
}
String& String::operator*=(int num) {
  char* str = new char[capacity_];
  int size = static_cast<int>(size_);
  std::copy(str_, str_ + size_, str);
  size_ *= num;
  size_t capacity = capacity_;
  CheckCapacity();
  if (capacity != capacity_) {
    RealizeMemory(size);
  }
  for (int i = size; i < static_cast<int>(size_); i += size) {
    std::copy(str, str + size, str_ + i);
  }
  delete[] str;
  return *this;
}
String String::Join(const std::vector<String>& strings) {
  String new_string;
  if (!strings.empty()) {
    new_string.size_ += size_ * (strings.size() - 1);
    for (int i = 0; i < static_cast<int>(strings.size()); ++i) {
      new_string.size_ += static_cast<int>(strings[i].size_);
    }
    new_string.CheckCapacity();
    new_string.str_ = new char[new_string.capacity_];
    int curr = 0;
    int end = static_cast<int>(strings.size()) - 1;
    for (int i = 0; i < end; ++i) {
      std::copy(strings[i].str_, strings[i].str_ + strings[i].size_,
                new_string.str_ + curr);
      curr += strings[i].size_;
      std::copy(str_, str_ + size_, new_string.str_ + curr);
      curr += size_;
    }
    std::copy(strings[end].str_, strings[end].str_ + strings[end].size_,
              new_string.str_ + curr);
  } else {
    new_string.capacity_ = 1;
    new_string.size_ = 0;
    new_string.str_ = new char[1];
  }
  return new_string;
}
void String::Spliting(std::vector<String>& result, int label_curr, int label) {
  if (label_curr - label > 0) {
    String str;
    str.capacity_ = 0;
    str.size_ = label_curr - label;
    str.CheckCapacity();
    str.str_ = new char[str.capacity_];
    std::copy(str_ + label, str_ + label_curr, str.str_);
    result.push_back(str);
  }
  if (!result.empty()) {
    if (result[result.size() - 1] != String("") && label_curr - label == 0) {
      result.emplace_back();
    }
  } else {
    result.emplace_back();
  }
}
std::vector<String> String::Split(const String& delim) {
  std::vector<String> result;
  int label = 0;
  size_t index = 0;
  while (index < size_) {
    if (index + delim.size_ - 1 < size_) {
      bool flag = true;
      for (size_t j = index; j < index + delim.size_; ++j) {
        if (str_[j] != delim[j - index]) {
          flag = false;
          break;
        }
      }
      if (flag) {
        Spliting(result, index, label);
        index += delim.size_ - 1;
        label = index + 1;
        flag = false;
      }
    }
    if ((index == size_ - 1) || (index == size_)) {
      Spliting(result, index + 1, label);
    }
    ++index;
  }
  Check(result);
  return result;
}
void String::Check(std::vector<String>& result) const {
  if (result.size() == 1 && result[result.size() - 1] == String("")) {
    result.emplace_back();
  } else if (result.empty()) {
    result.push_back(*this);
  }
}
std::ostream& operator<<(std::ostream& os, const String& str) {
  for (int i = 0; i < static_cast<int>(str.Size()); ++i) {
    os << str[i];
  }
  return os;
}
std::istream& operator>>(std::istream& is, String& str) {
  char curr;
  int size = 0;
  while (is.get(curr)) {
    if (curr != ' ') {
      str.PushBack(curr);
      ++size;
    } else if (size != 0) {
      break;
    }
  }
  return is;
}
