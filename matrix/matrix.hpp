#pragma once

#include <algorithm>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix();
  Matrix(T elem);
  Matrix(std::vector<std::vector<T>> vec);
  ~Matrix() = default;
  Matrix(const Matrix<N, M, T>& copy);
  Matrix<N, M, T> operator=(const Matrix<N, M, T>& copy);
  T& operator()(size_t i, size_t j);
  T operator()(size_t i, size_t j) const;
  Matrix<N, M, T> operator+(const Matrix<N, M, T>& obj);
  Matrix<N, M, T> operator-(const Matrix<N, M, T>& obj);
  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& obj);
  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& obj);
  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& obj);
  Matrix<N, M, T> operator*(const T& obj);

  Matrix<M, N, T> Transposed();

 private:
  std::vector<std::vector<T>> mat_;
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix() {
  mat_.resize(N, std::vector<T>(M, T()));
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(T elem) {
  mat_.resize(N, std::vector<T>(M, elem));
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(std::vector<std::vector<T>> vec) {
  mat_ = vec;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>::Matrix(const Matrix<N, M, T>& copy) {
  mat_ = copy.mat_;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator=(const Matrix<N, M, T>& copy) {
  mat_ = copy.mat_;
}

template <size_t N, size_t M, typename T>
T& Matrix<N, M, T>::operator()(size_t i, size_t j) {
  return mat_[i][j];
}

template <size_t N, size_t M, typename T>
T Matrix<N, M, T>::operator()(size_t i, size_t j) const {
  return mat_[i][j];
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator+(const Matrix<N, M, T>& obj) {
  Matrix<N, M, T> m = *this;
  m += obj;
  return m;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator-(const Matrix<N, M, T>& obj) {
  Matrix<N, M, T> m = *this;
  m -= obj;
  return m;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix<N, M, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      mat_[i][j] += obj.mat_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix<N, M, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      mat_[i][j] -= obj.mat_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
template <size_t K>
Matrix<N, K, T> Matrix<N, M, T>::operator*(const Matrix<M, K, T>& obj) {
  std::vector<std::vector<T>> vec(N, std::vector<T>(K));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < K; ++j) {
      T temp = T();
      for (size_t k = 0; k < M; ++k) {
        temp += mat_[i][k] * obj(k, j);
      }
      vec[i][j] = temp;
    }
  }
  Matrix<N, K, T> m(vec);
  return m;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(const T& obj) {
  Matrix<N, M, T> m = *this;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      m.mat_[i][j] *= obj;
    }
  }
  return m;
}

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() {
  std::vector<std::vector<T>> vec(M, std::vector<T>(N));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      vec[j][i] = mat_[i][j];
    }
  }
  Matrix<M, N, T> m(vec);
  return m;
}

template <size_t N, typename T>
class Matrix<N, N, T> {
 public:
  Matrix();
  Matrix(T elem);
  Matrix(std::vector<std::vector<T>> vec);
  ~Matrix() = default;
  Matrix(const Matrix<N, N, T>& copy);
  Matrix<N, N, T> operator=(const Matrix<N, N, T>& copy);
  T& operator()(size_t i, size_t j);
  T operator()(size_t i, size_t j) const;
  Matrix<N, N, T> operator+(const Matrix<N, N, T>& obj);
  Matrix<N, N, T> operator-(const Matrix<N, N, T>& obj);
  Matrix<N, N, T>& operator+=(const Matrix<N, N, T>& obj);
  Matrix<N, N, T>& operator-=(const Matrix<N, N, T>& obj);
  Matrix<N, N, T> operator*(const Matrix<N, N, T>& obj);
  Matrix<N, N, T> operator*(const T& obj);

  Matrix<N, N, T> Transposed();
  T Trace();

 private:
  std::vector<std::vector<T>> mat_;
};


template <size_t N, typename T>
Matrix<N, N, T>::Matrix() {
  mat_.resize(N, std::vector<T>(N, T()));
}

template <size_t N, typename T>
Matrix<N, N, T>::Matrix(T elem) {
  mat_.resize(N, std::vector<T>(N, elem));
}

template <size_t N, typename T>
Matrix<N, N, T>::Matrix(std::vector<std::vector<T>> vec) {
  mat_ = vec;
}

template <size_t N, typename T>
Matrix<N, N, T>::Matrix(const Matrix<N, N, T>& copy) {
  mat_ = copy.mat_;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator=(const Matrix<N, N, T>& copy) {
  mat_ = copy.mat_;
}

template <size_t N, typename T>
T& Matrix<N, N, T>::operator()(size_t i, size_t j) {
  return mat_[i][j];
}

template <size_t N, typename T>
T Matrix<N, N, T>::operator()(size_t i, size_t j) const {
  return mat_[i][j];
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator+(const Matrix<N, N, T>& obj) {
  Matrix<N, N, T> m = *this;
  m += obj;
  return m;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator-(const Matrix<N, N, T>& obj) {
  Matrix<N, N, T> m = *this;
  m -= obj;
  return m;
}

template <size_t N, typename T>
Matrix<N, N, T>& Matrix<N, N, T>::operator+=(const Matrix<N, N, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      mat_[i][j] += obj.mat_[i][j];
    }
  }
  return *this;
}

template <size_t N, typename T>
Matrix<N, N, T>& Matrix<N, N, T>::operator-=(const Matrix<N, N, T>& obj) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      mat_[i][j] -= obj.mat_[i][j];
    }
  }
  return *this;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator*(const Matrix<N, N, T>& obj) {
  std::vector<std::vector<T>> vec(N, std::vector<T>(N));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      T temp = T();
      for (size_t k = 0; k < N; ++k) {
        temp += mat_[i][k] * obj(k, j);
      }
      vec[i][j] = temp;
    }
  }
  Matrix<N, N, T> m(vec);
  return m;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::operator*(const T& obj) {
  Matrix<N, N, T> m = *this;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      m.mat_[i][j] *= obj;
    }
  }
  return m;
}

template <size_t N, typename T>
Matrix<N, N, T> Matrix<N, N, T>::Transposed() {
  std::vector<std::vector<T>> vec(N, std::vector<T>(N));
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      vec[j][i] = mat_[i][j];
    }
  }
  Matrix<N, N, T> m(vec);
  return m;
}

template <size_t N, typename T>
T Matrix<N, N, T>::Trace() {
  T trace = 0;
  size_t i = 0;
  while (i < N) {
    trace += mat_[i][i];
    ++i;
  }
  return trace;
}