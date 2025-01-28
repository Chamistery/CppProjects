#include "iostream.hpp"
#include <cstdio>
#include <limits>
#include <type_traits>

namespace stdlike {

istream cin(cout);
ostream cout;

istream::istream(ostream& output_stream)
    : buffer_pos(0), buffer_end(0), output_stream(output_stream) {
}

bool istream::fail() const {
    return fail_flag;
}

bool ostream::fail() const {
    return fail_flag;
}

void istream::fill_buffer() {
    ssize_t bytes_read = read(STDIN_FILENO, buffer, buffer_size);
    if (bytes_read < 0) {
        fail_flag = true;
        buffer_end = 0;
    } else {
        buffer_end = static_cast<size_t>(bytes_read);
    }
    buffer_pos = 0;
}

char istream::get() {
    if (buffer_pos >= buffer_end) {
        fill_buffer();
    }
    return (buffer_end > 0) ? buffer[buffer_pos++] : '\0';
}

void istream::unget() {
    if (buffer_pos > 0) {
        --buffer_pos;
    }
}

char istream::peek() {
    if (buffer_pos >= buffer_end) {
        fill_buffer();
    }
    return (buffer_end > 0) ? buffer[buffer_pos] : '\0';
}

template <typename T>
istream& istream::ReadNumber(T& value) {
    value = 0;
    output_stream.flush();
    char curr = peek();
    bool negative = false;

    while (curr == ' ' || curr == '\n' || curr == '\t') {
        curr = get();
    }

    if (curr != '-' && curr != '+' && (curr < '0' || curr > '9')) {
        fail_flag = true;
        return *this;
    }

    if (curr == '-') {
        negative = true;
        curr = get();
    } else if (curr == '+') {
        curr = get();
    }

    while (curr >= '0' && curr <= '9') {
        value = value * 10 - (curr - '0');
        curr = get();
    }

    if (!negative) {
        if (value == std::numeric_limits<T>::min()) {
            fail_flag = true;
        } else {
            value = -value;
        }
    }

    if (peek() != '\0') {
        --buffer_pos;
    }
    output_stream.flush();
    return *this;
}

istream& istream::operator>>(int& value) {
    return ReadNumber<int>(value);
}

istream& istream::operator>>(unsigned int& value) {
    return ReadNumber<unsigned int>(value);
}

istream& istream::operator>>(long long& value) {
    return ReadNumber<long long>(value);
}

istream& istream::operator>>(unsigned long long& value) {
    return ReadNumber<unsigned long long>(value);
}

template <typename T>
istream& istream::ReadReal(T& value) {
    value = 0.0;
    bool negative = false;
    bool decimal_point_found = false;
    bool has_digits = false;
    double decimal_factor = 0.1;
    output_stream.flush();
    char curr = get();

    while (curr == ' ' || curr == '\n' || curr == '\t') {
        output_stream.flush();
        curr = get();
    }

    if (curr == '-') {
        negative = true;
        output_stream.flush();
        curr = get();
    } else if (curr == '+') {
        output_stream.flush();
        curr = get();
    }
    while ((curr >= '0' && curr <= '9') || curr == '.') {
        if (curr >= '0' && curr <= '9') {
            has_digits = true;
            if (!decimal_point_found) {
                value = value * 10 + (curr - '0');
            } else {
                value += (curr - '0') * decimal_factor;
                decimal_factor /= 10;
            }
        } else if (curr == '.') {
            if (decimal_point_found) {
                break;
            }
            decimal_point_found = true;
        }
        output_stream.flush();
        curr = get();
    }

    if (!has_digits) {
        value = 0.0;
        return *this;
    }
    unget();
    if (negative) {
        value = -value;
    }
    output_stream.flush();
    return *this;
}

istream& istream::operator>>(double& value) {
    return ReadReal<double>(value);
}

istream& istream::operator>>(float& value) {
    return ReadReal<float>(value);
}

istream& istream::operator>>(char& value) {
    output_stream.flush();
    char curr = get();
    while (curr == ' ' || curr == '\n' || curr == '\t') {
        output_stream.flush();
        curr = get();
    }
    value = curr;
    output_stream.flush();
    return *this;
}

istream& istream::operator>>(bool& value) {
    int temp = 0;
    *this >> temp;
    value = (temp != 0);
    output_stream.flush();
    return *this;
}

ostream::ostream() : buffer_pos(0) {
}

void ostream::flush_buffer() {
    if (buffer_pos > 0) {
        ssize_t written = write(STDOUT_FILENO, buffer, buffer_pos);
        if (written < 0) {
            fail_flag = true;
        }
        buffer_pos = 0;
    }
}

template <typename T>
ostream& ostream::WriteNumber(T value) {
    if constexpr (std::is_signed_v<T>) {
        if (value == std::numeric_limits<T>::min()) {
            put('-');
            T abs_value = -(value / 10);
            WriteNumber(abs_value);
            put('0' - (value % 10));
            return *this;
        }
        if (value < 0) {
            put('-');
            value = -value;
        }
    }

    char temp[20];
    int len = 0;
    do {
        temp[len++] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);

    for (int i = len - 1; i >= 0; --i) {
        put(temp[i]);
    }
    return *this;
}

ostream& ostream::operator<<(long long value) {
    return WriteNumber<long long>(value);
}

ostream& ostream::operator<<(unsigned long long value) {
    return WriteNumber<unsigned long long>(value);
}

ostream& ostream::operator<<(int value) {
    return WriteNumber<int>(value);
}

ostream& ostream::operator<<(unsigned int value) {
    return WriteNumber<unsigned int>(value);
}

template <typename T>
ostream& ostream::WriteReal(T value) {
    char temp[64];
    int len = snprintf(temp, sizeof(temp), "%.6f", value);
    for (int i = 0; i < len; ++i) {
        put(temp[i]);
    }
    return *this;
}

ostream& ostream::operator<<(double value) {
    return WriteReal<double>(value);
}

ostream& ostream::operator<<(float value) {
    return WriteReal<float>(value);
}

ostream& ostream::operator<<(char value) {
    put(value);
    return *this;
}

ostream& ostream::operator<<(bool value) {
    put(value ? '1' : '0');
    return *this;
}

ostream& ostream::operator<<(const char* str) {
    while (*str) {
        put(*str++);
    }
    return *this;
}

ostream& ostream::operator<<(const void* ptr) {
    char buffer[20];
    int len = snprintf(buffer, sizeof(buffer), "%p", ptr);
    for (int i = 0; i < len; ++i) {
        put(buffer[i]);
    }
    return *this;
}

void ostream::put(char curr) {
    if (buffer_pos >= buffer_size) {
        flush_buffer();
    }
    buffer[buffer_pos++] = curr;
}

void ostream::flush() {
    flush_buffer();
}
}  // namespace stdlike
