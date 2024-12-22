#pragma once
#include <iostream>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix() : data_(N, std::vector<T>(M)) {}
  Matrix(std::vector<std::vector<T>> data) : data_(data) {}
  Matrix(const T& elem);
  Matrix<N, M, T> operator+(Matrix<N, M, T> to_add);
  Matrix<N, M, T> operator-(Matrix<N, M, T> to_erase);
  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& to_add);
  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& to_erase);
  Matrix<N, M, T> operator*(T elem);
  template <size_t L>
  Matrix<N, L, T> operator*(Matrix<M, L, T> to_multy);
  Matrix<M, N, T> Transposed();
  T Trace();
  const T& operator()(int64_t fst, int64_t scd) const;
  T& operator()(int64_t fst, int64_t scd);
  bool operator==(Matrix<N, M, T> to_equal);

 private:
  std::vector<std::vector<T>> data_;
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(const T& elem) : data_(N, std::vector<T>(M)) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      data_[i][j] = elem;
    }
  }
}
template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix<N, M, T>& to_add) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      (*this).data_[i][j] += to_add(i, j);
    }
  }
  return *this;
}
template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(Matrix<N, M, T> to_add) {
  Matrix<N, M, T> new_mat(data_);
  new_mat += to_add;
  return new_mat;
}
template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix<N, M, T>& to_erase) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      (*this).data_[i][j] -= to_erase(i, j);
    }
  }
  return *this;
}
template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(Matrix<N, M, T> to_erase) {
  Matrix<N, M, T> new_mat(data_);
  new_mat -= to_erase;
  return new_mat;
}
template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(T elem) {
  Matrix<N, M, T> new_mat(data_);
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      new_mat(i, j) *= elem;
    }
  }
  return new_mat;
}
template <size_t N, size_t M, typename T>
template <size_t L>
Matrix<N, L, T> Matrix<N, M, T>::operator*(Matrix<M, L, T> to_multy) {
  Matrix<N, L, T> new_mat;
  size_t fst = 0;
  size_t scd = 0;
  while (fst != N) {
    if (fst == N) {
      fst = 0;
    }
    if (scd == L) {
      scd = 0;
    }
    for (size_t i = 0; i < M; ++i) {
      new_mat(fst, scd) += data_[fst][i] * to_multy(i, scd);
    }
    if (scd == L - 1) {
      ++fst;
      scd = 0;
    } else {
      ++scd;
    }
  }
  return new_mat;
}
template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() {
  Matrix<M, N, T> new_mat;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      new_mat(j, i) = data_[i][j];
    }
  }
  return new_mat;
}
template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::Trace() {
  static_assert(N == M);
  T ans = data_[0][0];
  for (size_t i = 1; i < N; ++i) {
    ans += data_[i][i];
  }
  return ans;
}
template <size_t N, size_t M, typename T>
T& Matrix<N, M, T>::operator()(int64_t fst, int64_t scd) {
  T& ans = data_[fst][scd];
  return ans;
}
template <size_t N, size_t M, typename T>
const T& Matrix<N, M, T>::operator()(int64_t fst, int64_t scd) const {
  return data_[fst][scd];
}
template <size_t N, size_t M, typename T>
bool Matrix<N, M, T>::operator==(Matrix<N, M, T> to_equal) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      if (data_[i][j] != to_equal(i, j)) {
        return false;
      }
    }
  }
  return true;
}
