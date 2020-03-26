#ifndef VEC_H
#define VEC_H

#include <cmath>
#include <cassert>


static const double CS175_PI = 3.14159265358979323846264338327950288;
static const double CS175_EPS = 1e-8;
static const double CS175_EPS2 = CS175_EPS * CS175_EPS;
static const double CS175_EPS3 = CS175_EPS * CS175_EPS * CS175_EPS;

//Cvec类型的实现为何借助于模版？因为vector的基类型包括int，bool，float。同时vector部件的数目并不固定
template <typename T, int n>
class Cvec {
    //类数据的存储，以对应基础类型的数组方式存在
    T d_[n];
    
public:
    //默认构造函数，以for语句的方式初始化默认存储为0
    Cvec() {
        for (int i = 0; i < n; ++i) {
            d_[i] = 0;
        }
    }
    
    //构造函数，将所有部件都初始化为单一基础类型值
    Cvec(const T& t) {
        for (int i = 0; i < n; ++i) {
            d_[i] = t;
        }
    }
    
    //构造函数，针对2维vector的初始化，从两个基本类型
    Cvec(const T& t0, const T& t1) {
        //断言是否为2维
        assert(n == 2); // better to use static_assert from c++11
        d_[0] = t0, d_[1] = t1;
    }
    
    //构造函数，从3个基本类型逐一初始化vector
    Cvec(const T& t0, const T& t1, const T& t2) {
        assert(n == 3); // better to use static_assert from c++11
        d_[0] = t0, d_[1] = t1, d_[2] = t2;
    }
    
    //构造函数，从4个基本类型初始化vector
    Cvec(const T& t0, const T& t1, const T& t2, const T& t3) {
        assert(n == 4); // better to use static_assert from c++11
        d_[0] = t0, d_[1] = t1, d_[2] = t2, d_[3] = t3;
    }
    
    //构造函数，从vector类型和基础类型共同构造，基础类型默认初始值为0
    //参数vector类型的数目可以大于也可以小于当前构造的部件，总之，根据两者的最小部件数目进行
    //两个vector之间的对应部件复制，若赋值部件数目小于当前vector的数目，则用扩展值单一填充，扩展值默认为0
    // either truncate if m < n, or extend with extendValue
    template<int m>
    explicit Cvec(const Cvec<T, m>& v, const T& extendValue = T(0)) {
        for (int i = 0; i < fmin(m, n); ++i) {
            d_[i] = v[i];
        }
        for (int i = fmin(m, n); i < n; ++i) {
            d_[i] = extendValue;
        }
    }
    
    //[]操作符实现
    T& operator [] (const int i) {
        return d_[i];
    }
    
    //[]操作符常量类型实现
    const T& operator [] (const int i) const {
        return d_[i];
    }
    
    //()操作符实现
    T& operator () (const int i) {
        return d_[i];
    }
    
    //()操作符常量类型实现
    const T& operator () (const int i) const {
        return d_[i];
    }
    
    //“-”操作符实现
    Cvec operator - () const {
        return Cvec(*this) *= -1;
    }
    
    //“+=”操作符实现
    Cvec& operator += (const Cvec& v) {
        //最终借助基础类型的内置+=语意实现
        for (int i = 0; i < n; ++i) {
            d_[i] += v[i];
        }
        return *this;
    }
    
    //“-=”操作符实现
    Cvec& operator -= (const Cvec& v) {
        for (int i = 0; i < n; ++i) {
            d_[i] -= v[i];
        }
        return *this;
    }
    
    //标量“*=”操作符实现
    Cvec& operator *= (const T a) {
        for (int i = 0; i < n; ++i) {
            //此处的“*=”是本意
            d_[i] *= a;
        }
        return *this;
    }
    
    //标量“*=”操作符实现
    Cvec& operator *= (const Cvec& v) {
        for (int i = 0; i < n; ++i) {
            //此处的“*=”是本意
            d_[i] *= v[i];
        }
        return *this;
    }
    
    //标量“/=”操作符实现
    Cvec& operator /= (const T a) {
        //此处inva是变量，括号中内容是list initialzation，等同于T inva = 1/a
        const T inva(1/a);
        for (int i = 0; i < n; ++i) {
            d_[i] *= inva;
        }
        return *this;
    }
    
    //vector "=="操作符实现
    bool operator == (const Cvec& v) {
        for (int i = 0; i < n; ++i) {
            //只要有部件不相同，则vector不相等
            if(d_[i] != v[i])
                return false;
        }
        return true;
    }
    
    //“/=”操作符实现，基于“==”操作符
    bool operator != (const Cvec& v) {
        return !(Cvec(*this) == v); // ! precedes ==
    }
    
    //vector间“+”操作符实现
    Cvec operator + (const Cvec& v) const {
        return Cvec(*this) += v;
    }
    
    //vector间“-”操作符实现
    Cvec operator - (const Cvec& v) const {
        return Cvec(*this) -= v;
    }
    
    //标量“*”操作符实现
    Cvec operator * (const T a) const {
        return Cvec(*this) *= a;
    }
    
    Cvec operator * (const Cvec& v) const {
        return Cvec(*this) *= v;
    }
    
    //标量“/”操作符实现
    Cvec operator / (const T a) const {
        return Cvec(*this) /= a;
    }
    
    //vector自身的标准化实现
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
    //使用list initialization将变量r初始化为0
    T r(0);
    for (int i = 0; i < n; ++i) {
        r += a(i)*b(i);
    }
    return r;
}

//--------------------------------------------------------------------------------
// 平方长
//--------------------------------------------------------------------------------
template<typename T, int n>
inline T norm2(const Cvec<T, n>& v) {
    //vector与自身的dot product即为平方长
    return dot(v, v);
}

//--------------------------------------------------------------------------------
//  平方长开方，平方态
//--------------------------------------------------------------------------------
template<typename T, int n>
inline T norm(const Cvec<T, n>& v) {
    return std::sqrt(norm2(v));
}

// Return a normalized vector without modifying the input (unlike the member
// function version v.normalize() ).
template<typename T, int n>
inline Cvec<T, n> normalize(const Cvec<T,n>& v) {
    //对vector的平方长的数值要求最小精度，否则程序无法区别
    assert(dot(v, v) > CS175_EPS2);
    return v / norm(v);
}

template<typename T, int n>
inline Cvec<T, n> maxVec(const Cvec<T,n>& v1,const Cvec<T,n>& v2) {
    Cvec<T,n> v;
    for(int i=0;i<n;i++){
        v[i] = fmax(v1[i],v2[i]);
    }
    
    return v;
}

template<typename T, int n>
inline Cvec<T, n> minVec(const Cvec<T,n>& v1,const Cvec<T,n>& v2) {
    Cvec<T,n> v;
    for(int i=0;i<n;i++){
        v[i] = fmin(v1[i],v2[i]);
    }
    
    return v;
}

//设置常用子类型的alias，以方便使用
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
