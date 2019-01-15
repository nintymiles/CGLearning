#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <cassert>


static const double CS175_PI = 3.14159265358979323846264338327950288;
static const double CS175_EPS = 1e-8;
static const double CS175_EPS2 = CS175_EPS * CS175_EPS;
static const double CS175_EPS3 = CS175_EPS * CS175_EPS * CS175_EPS;


template <typename T, int n>
class Cvec {
  T d_[n];

public:
  Cvec() {
    for (int i = 0; i < n; ++i) {
      d_[i] = 0;
    }
  }

  Cvec(const T& t) {
    for (int i = 0; i < n; ++i) {
      d_[i] = t;
    }
  }

  Cvec(const T& t0, const T& t1) {
    assert(n == 2); // better to use static_assert from c++11
    d_[0] = t0, d_[1] = t1;
  }

  Cvec(const T& t0, const T& t1, const T& t2) {
    assert(n == 3); // better to use static_assert from c++11
    d_[0] = t0, d_[1] = t1, d_[2] = t2;
  }

  Cvec(const T& t0, const T& t1, const T& t2, const T& t3) {
    assert(n == 4); // better to use static_assert from c++11
    d_[0] = t0, d_[1] = t1, d_[2] = t2, d_[3] = t3;
  }

  // either truncate if m < n, or extend with extendValue
  template<int m>
  explicit Cvec(const Cvec<T, m>& v, const T& extendValue = T(0)) {
    for (int i = 0; i < std::min(m, n); ++i) {
      d_[i] = v[i];
    }
    for (int i = std::min(m, n); i < n; ++i) {
      d_[i] = extendValue;
    }
  }

  T& operator [] (const int i) {
    return d_[i];
  }

  const T& operator [] (const int i) const {
    return d_[i];
  }

  T& operator () (const int i) {
    return d_[i];
  }

  const T& operator () (const int i) const {
    return d_[i];
  }

  Cvec operator - () const {
    return Cvec(*this) *= -1;
  }

  Cvec& operator += (const Cvec& v) {
    for (int i = 0; i < n; ++i) {
      d_[i] += v[i];
    }
    return *this;
  }

  Cvec& operator -= (const Cvec& v) {
    for (int i = 0; i < n; ++i) {
      d_[i] -= v[i];
    }
    return *this;
  }

  Cvec& operator *= (const T a) {
    for (int i = 0; i < n; ++i) {
      d_[i] *= a;
    }
    return *this;
  }

  Cvec& operator /= (const T a) {
    const T inva(1/a);
    for (int i = 0; i < n; ++i) {
      d_[i] *= inva;
    }
    return *this;
  }
    
    bool operator == (const Cvec& v) {
        for (int i = 0; i < n; ++i) {
            if(d_[i] != v[i])
                return false;
        }
        return true;
    }
    
    // ! precedes ==
    bool operator != (const Cvec& v) {
        return !(Cvec(*this) == v);
    }

  Cvec operator + (const Cvec& v) const {
    return Cvec(*this) += v;
  }

  Cvec operator - (const Cvec& v) const {
    return Cvec(*this) -= v;
  }

  Cvec operator * (const T a) const {
    return Cvec(*this) *= a;
  }

  Cvec operator / (const T a) const {
    return Cvec(*this) /= a;
  }

  // Normalize self and returns self
  Cvec& normalize() {
    assert(dot(*this, *this) > CS175_EPS2);
    return *this /= std::sqrt(dot(*this, *this));
  }
};

//--------------------------------------------------------------------------------
//  向量积 cross product
//--------------------------------------------------------------------------------
template<typename T>
inline Cvec<T,3> cross(const Cvec<T,3>& a, const Cvec<T,3>& b) {
  return Cvec<T,3>(a(1)*b(2)-a(2)*b(1), a(2)*b(0)-a(0)*b(2), a(0)*b(1)-a(1)*b(0));
}

//--------------------------------------------------------------------------------
//  dot product implementation 点积实现
//--------------------------------------------------------------------------------
template<typename T, int n>
inline T dot(const Cvec<T,n>& a, const Cvec<T,n>& b) {
  T r(0);
  for (int i = 0; i < n; ++i) {
    r += a(i)*b(i);
  }
  return r;
}

//--------------------------------------------------------------------------------
// 平方长，平方态
//--------------------------------------------------------------------------------
template<typename T, int n>
inline T norm2(const Cvec<T, n>& v) {
  return dot(v, v);
}

//--------------------------------------------------------------------------------
//  平方长开方
//--------------------------------------------------------------------------------
template<typename T, int n>
inline T norm(const Cvec<T, n>& v) {
  return std::sqrt(dot(v, v));
}

// Return a normalized vector without modifying the input (unlike the member
// function version v.normalize() ).
template<typename T, int n>
inline Cvec<T, n> normalize(const Cvec<T,n>& v) {
  assert(dot(v, v) > CS175_EPS2);
  return v / norm(v);
}

// element of type double precision float
typedef Cvec <double, 2> Cvec2;
typedef Cvec <double, 3> Cvec3;
typedef Cvec <double, 4> Cvec4;

// element of type single precision float
typedef Cvec <float, 2> Cvec2f;
typedef Cvec <float, 3> Cvec3f;
typedef Cvec <float, 4> Cvec4f;

// elements of type unsigned byte
typedef Cvec <unsigned char, 2> Cvec2ub;
typedef Cvec <unsigned char, 3> Cvec3ub;
typedef Cvec <unsigned char, 4> Cvec4ub;

#endif
