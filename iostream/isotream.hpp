#pragma once

#include <unistd.h>
#include <cstddef>

namespace stdlike {

class istream;
class ostream;

extern istream cin;
extern ostream cout;

class istream {
public:
    bool fail() const;
    istream(ostream& output_stream);
    istream& operator>>(unsigned long long& value);
    istream& operator>>(int& value);
    istream& operator>>(long long& value);
    istream& operator>>(unsigned int& value);
    istream& operator>>(double& value);
    istream& operator>>(float& value);
    istream& operator>>(char& value);
    istream& operator>>(bool& value);

    char get();
    void unget();
    void put(char c);
    char peek();

private:
    static const size_t buffer_size = 1024;
    char buffer[buffer_size];
    size_t buffer_pos;
    size_t buffer_end;

    template <typename T>
    istream& ReadNumber(T& value);
    template <typename T>
    istream& ReadReal(T& value);
    void fill_buffer();
    void clear_buffer();
    bool fail_flag = false;
    ostream& output_stream;
};

class ostream {
public:
    ostream();
    bool fail() const;
    ostream& operator<<(int value);
    ostream& operator<<(long long value);
    ostream& operator<<(unsigned long long value);
    ostream& operator<<(unsigned int value);
    ostream& operator<<(double value);
    ostream& operator<<(float value);
    ostream& operator<<(char value);
    ostream& operator<<(bool value);
    ostream& operator<<(const char* str);
    ostream& operator<<(const void* ptr);

    void put(char c);
    void flush();

private:
    static const size_t buffer_size = 1024;
    char buffer[buffer_size];

    template <typename T>
    ostream& WriteNumber(T value);
    template <typename T>
    ostream& WriteReal(T value);
    size_t buffer_pos;

    bool fail_flag = false;
    void flush_buffer();
};

extern istream cin;
extern ostream cout;

}  // namespace stdlike
