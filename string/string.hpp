#pragma once
#include <iostream>
#include <vector>

class String {
 public:
  String();
  String(size_t size, char character);
  String(const char* str);
  String(const String& str);
  String& operator=(const String& str);
  String& operator=(const char* str);
  ~String() { delete[] str_; };
  void Clear() { size_ = 0; };
  void PushBack(char character);
  void PopBack();
  void Resize(size_t new_size);
  void Resize(size_t new_size, char character);
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  void Swap(String& other);
  char& operator[](int ind) { return *(str_ + ind); };
  char operator[](int ind) const { return *(str_ + ind); };
  char Front() const { return str_[0]; };
  char& Front() { return str_[0]; };
  char Back() const { return str_[size_ - 1]; };
  char& Back() { return str_[size_ - 1]; };
  bool Empty() const { return size_ == 0; };
  size_t Size() const { return size_; };
  size_t Capacity() const { return capacity_; };
  const char* Data() const { return str_; };
  char* Data() { return str_; };
  bool operator==(const String& str) const;
  bool operator!=(const String& str) const { return !(str == *(this)); }
  bool operator<(const String& str) const { return (str > *this); };
  bool operator>(const String& str) const;
  bool operator<=(const String& str) const { return !(*this > str); }
  bool operator>=(const String& str) const { return !(*this < str); }
  String operator+(const String& str) const;
  String& operator+=(const String& str);
  String operator*(int num) const;
  String& operator*=(int num);
  std::vector<String> Split(const String& delim = " ");
  String Join(const std::vector<String>& strings);

 private:
  void Check(std::vector<String>& result) const;
  void RealizeMemory(size_t size);
  void CheckCapacity();
  void Spliting(std::vector<String>& result, int label_curr, int label);
  size_t size_ = 0;
  size_t capacity_ = 1;
  char* str_ = nullptr;
};

std::ostream& operator<<(std::ostream& os, const String& str);
std::istream& operator>>(std::istream& is, String& str);
