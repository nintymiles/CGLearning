#ifndef QUAT_H
#define QUAT_H

#include <iostream>
#include <cassert>
#include <cmath>

#include "cvec.h"
#include "matrix4.h"

// Forward declarations used in the definition of Quat;
//在Quat类的定义和实现中需要使用到的一些前置方法及类型声明，因为在header文件中包含了类型的定义和实现
class Quat;
// dot product between Quaternions
double dot(const Quat& q, const Quat& p);
// Quaternion的平方态
double norm2(const Quat& q);
// Quaternion的倒数
Quat inv(const Quat& q);
// Quaternion的标准化
Quat normalize(const Quat& q);
// Quaternion到Matrix4的转换
Matrix4 quatToMatrix(const Quat& q);

//Quat类的定义和实现
class Quat {
    //类的变量存储，遵循的布局方式为scalar部件在前，vector部件在后
    Cvec4 q_;  // layout is: q_[0]==w, q_[1]==x, q_[2]==y, q_[3]==z
    
public:
    //[]操作符，当以const方式声明变量时，所获取的值为变量本身
    double operator [] (const int i) const {
        return q_[i];
    }
    
    //[]操作符非const实现，所返回值的值为reference，但是对于scalar返回实际也是变量本身
    double& operator [] (const int i) {
        return q_[i];
    }
    
    //()操作符const实现，返回值本身
    double operator () (const int i) const {
        return q_[i];
    }
    
    //()操作符非const实现，返回值reference，但是对scalar来讲，返回的事值本身，没有返回reference，是因为效率相当？
    double& operator () (const int i) {
        return q_[i];
    }
    
    //默认构造函数，初始化变量值为identity quaternion
    Quat() : q_(1,0,0,0) {}
    //接收scalar和vector的构造函数
    Quat(const double w, const Cvec3& v) : q_(w, v[0], v[1], v[2]) {}
    //接收4个scalar的构造函数
    Quat(const double w, const double x, const double y, const double z) : q_(w, x,y,z) {}
    
    //+=操作符，实现类似vector的方式
    Quat& operator += (const Quat& a) {
        q_ += a.q_;
        return *this;
    }
    
    //-=操作符实现，实际依赖Cvec4的实现
    Quat& operator -= (const Quat& a) {
        q_ -= a.q_;
        return *this;
    }
    
    //*=操作符实现，基于scalar乘法
    Quat& operator *= (const double a) {
        q_ *= a;
        return *this;
    }
    
    //“/=”操作符实现，基于scalar乘法（除法）
    Quat& operator /= (const double a) {
        q_ /= a;
        return *this;
    }
    
    //+操作符实现，基于Quat，依赖+=的实现
    Quat operator + (const Quat& a) const {
        return Quat(*this) += a;
    }
    
    //-操作符实现，基于Quat，依赖-=的实现
    Quat operator - (const Quat& a) const {
        return Quat(*this) -= a;
    }
    
    //overload negative sign operation
    //“-“单操作符实现，依赖*=的实现
    Quat operator - () const {
        return Quat(*this) *= -1;
    }
    
    //“*”操作符实现，scalar multiplication operator，依赖scalar *=的实现
    Quat operator * (const double a) const {
        return Quat(*this) *= a;
    }
    
    //“/”操作符实现，scalar division operator，依赖scalar /= 的实现
    Quat operator / (const double a) const {
        return Quat(*this) /= a;
    }
    
    //Quat间“*”操作符实现，Quaternion操作的核心计算
    Quat operator * (const Quat& a) const {
        const Cvec3 u(q_[1], q_[2], q_[3]), v(a.q_[1], a.q_[2], a.q_[3]);
        return Quat(q_[0]*a.q_[0] - dot(u, v), (v*q_[0] + u*a.q_[0]) + cross(u, v));
    }
    
    //Quat和Cvec4间“*”操作符实现，实现vector之上的rotation计算
    Cvec4 operator * (const Cvec4& a) const {
        const Quat r = *this * (Quat(0, a[0], a[1], a[2]) * inv(*this));
        return Cvec4(r[1], r[2], r[3], a[3]);
    }
    
    //类静态method，生成identity Quaternion
    static Quat identity(){
        return Quat(1,Cvec3(0));
    }
    
    //生成围绕x轴转动ang角度的单位Quaternion，ang为旋转的度数，非radian表达
    static Quat makeXRotation(const double ang) {
        Quat r;
        //将ang的一半转化为radian表达，ang的一半为quaternion所使用的角度量
        const double h = 0.5 * ang * CS175_PI/180;
        //将vector的第一部件设置为sin(h)角度即可，注意h为实际角度的一半
        r.q_[1] = std::sin(h);
        r.q_[0] = std::cos(h);
        return r;
    }
    
    //生成围绕y轴转动ang角度的单位Quaternion，围绕哪个轴设置哪个轴对应的Cvec4部件即可
    static Quat makeYRotation(const double ang) {
        Quat r;
        const double h = 0.5 * ang * CS175_PI/180;
        r.q_[2] = std::sin(h);
        r.q_[0] = std::cos(h);
        return r;
    }
    
    //生成围绕z轴转动ang角度的单位Quaternon
    static Quat makeZRotation(const double ang) {
        Quat r;
        const double h = 0.5 * ang * CS175_PI/180;
        r.q_[3] = std::sin(h);
        r.q_[0] = std::cos(h);
        return r;
    }
};

//Quaternion之间的dot product实现
inline double dot(const Quat& q, const Quat& p) {
    double s = 0.0;
    for (int i = 0; i < 4; ++i) {
        s += q(i) * p(i);
    }
    return s;
}

//Quaernion的平方态实现
inline double norm2(const Quat& q) {
    return dot(q, q);
}

//Quaternion的倒数实现，但是这个实现好像也针对非标准Quaternion？
inline Quat inv(const Quat& q) {
    const double n = norm2(q);
    assert(n > CS175_EPS2);
    return Quat(q(0), -q(1), -q(2), -q(3)) * (1.0/n);
}

//Quternion的标准化实现
inline Quat normalize(const Quat& q) {
    return q / std::sqrt(norm2(q));
}

//Quaternion到Matrix4的转换实现
inline Matrix4 quatToMatrix(const Quat& q) {
    Matrix4 r;
    const double n = norm2(q);
    //如果Quaternion的平方态很小，则不计算Matrix的效果
    if (n < CS175_EPS2)
        return Matrix4(0);
    
    const double two_over_n = 2/n;
    r(0, 0) -= (q(2)*q(2) + q(3)*q(3)) * two_over_n;
    r(0, 1) += (q(1)*q(2) - q(0)*q(3)) * two_over_n;
    r(0, 2) += (q(1)*q(3) + q(2)*q(0)) * two_over_n;
    r(1, 0) += (q(1)*q(2) + q(0)*q(3)) * two_over_n;
    r(1, 1) -= (q(1)*q(1) + q(3)*q(3)) * two_over_n;
    r(1, 2) += (q(2)*q(3) - q(1)*q(0)) * two_over_n;
    r(2, 0) += (q(1)*q(3) - q(2)*q(0)) * two_over_n;
    r(2, 1) += (q(2)*q(3) + q(1)*q(0)) * two_over_n;
    r(2, 2) -= (q(1)*q(1) + q(2)*q(2)) * two_over_n;
    
    assert(isAffine(r));
    return r;
}

//--------------------------------------------------------------------------------
//  Power Operation for quaternion 冥操作的实现
//--------------------------------------------------------------------------------
inline Quat pow(const Quat& q,const double alpha){
    Quat r;
    if(q[0]<0)
        r = -q ;//Quat(-q[0],-q[1],-q[2],-q[3]);
    else
        r = q;
    
    //--------------------------------------------------------------------------------
    //  two steps to extract degree in Quaternion representation
    //--------------------------------------------------------------------------------
    const Cvec3 u(r[1],r[2],r[3]);
    const double sqrtU = norm(u);
    Cvec3 k = u/sqrtU;
    //atan2(y,x)函数先接收y坐标，再x坐标
    const double angle = 2*atan2(sqrtU,r[0]);
    r[0] = cos(alpha * angle/2);
    k = k * sin(alpha * angle /2);
    r[1] = k[0];
    r[2] = k[1];
    r[3] = k[2];
    
    return r;
}

//--------------------------------------------------------------------------------
//  unit sphere linear interploation - slerping function
//--------------------------------------------------------------------------------
inline Quat slerp(const Quat& q0,const Quat& q1,const double alpha){
    //alpha belongs to [0..1]
    assert(alpha<=1 || alpha >= 0);
    Quat r = q1 * inv(q0);
    
    //not needed,because negation operation will be done at pow function
    //    if(r[0]<0){
    //        r = Quat(-r[0],-r[1],-r[2],-r[3]);
    //    }
    
    return pow(r,alpha) * q0;
}

//--------------------------------------------------------------------------------
//  lerping function
//--------------------------------------------------------------------------------
inline Quat lerp(const Quat& q0,const Quat& q1,const double alpha){
    assert(alpha <= 1 || alpha >= 0);
    Quat r0,r1;
    const double dotP= dot(q0,q1);
    //if q0.q1<0,then negating one of q0/q1
    if( dotP < 0)
        r0 = -q0;//Quat(-q0[0],-q0[1],-q0[2],-q0[3]);
    
    r1 = q1;
    Quat r = r0 * (1-alpha) +  r1 * alpha;
    return normalize(r);
}


#endif
