#ifndef QUAT_H
#define QUAT_H

#include <iostream>
#include <cassert>
#include <cmath>

#include "cvec.h"
#include "matrix4.h"

// Forward declarations used in the definition of Quat;
class Quat;
double dot(const Quat& q, const Quat& p);
double norm2(const Quat& q);
Quat inv(const Quat& q);
Quat normalize(const Quat& q);
Matrix4 quatToMatrix(const Quat& q);

class Quat {
  Cvec4 q_;  // layout is: q_[0]==w, q_[1]==x, q_[2]==y, q_[3]==z

public:
  double operator [] (const int i) const {
    return q_[i];
  }

  double& operator [] (const int i) {
    return q_[i];
  }

  double operator () (const int i) const {
    return q_[i];
  }

  double& operator () (const int i) {
    return q_[i];
  }

  Quat() : q_(1,0,0,0) {}
  Quat(const double w, const Cvec3& v) : q_(w, v[0], v[1], v[2]) {}
  Quat(const double w, const double x, const double y, const double z) : q_(w, x,y,z) {}

  Quat& operator += (const Quat& a) {
    q_ += a.q_;
    return *this;
  }

  Quat& operator -= (const Quat& a) {
    q_ -= a.q_;
    return *this;
  }

  Quat& operator *= (const double a) {
    q_ *= a;
    return *this;
  }

  Quat& operator /= (const double a) {
    q_ /= a;
    return *this;
  }

  Quat operator + (const Quat& a) const {
    return Quat(*this) += a;
  }

  Quat operator - (const Quat& a) const {
    return Quat(*this) -= a;
  }
    
  //overload negative sign operation
  Quat operator - () const {
        return Quat(*this) *= -1;
  }

  Quat operator * (const double a) const {
    return Quat(*this) *= a;
  }

  Quat operator / (const double a) const {
    return Quat(*this) /= a;
  }

  Quat operator * (const Quat& a) const {
    const Cvec3 u(q_[1], q_[2], q_[3]), v(a.q_[1], a.q_[2], a.q_[3]);
    return Quat(q_[0]*a.q_[0] - dot(u, v), (v*q_[0] + u*a.q_[0]) + cross(u, v));
  }

  Cvec4 operator * (const Cvec4& a) const {
    const Quat r = *this * (Quat(0, a[0], a[1], a[2]) * inv(*this));
    return Cvec4(r[1], r[2], r[3], a[3]);
  }
    
    static Quat identity(){
        return Quat(1,Cvec3(0));
    }

  static Quat makeXRotation(const double ang) {
    Quat r;
    const double h = 0.5 * ang * CS175_PI/180;
    r.q_[1] = std::sin(h);
    r.q_[0] = std::cos(h);
    return r;
  }

  static Quat makeYRotation(const double ang) {
    Quat r;
    const double h = 0.5 * ang * CS175_PI/180;
    r.q_[2] = std::sin(h);
    r.q_[0] = std::cos(h);
    return r;
  }

  static Quat makeZRotation(const double ang) {
    Quat r;
    const double h = 0.5 * ang * CS175_PI/180;
    r.q_[3] = std::sin(h);
    r.q_[0] = std::cos(h);
    return r;
  }
};

inline double dot(const Quat& q, const Quat& p) {
  double s = 0.0;
  for (int i = 0; i < 4; ++i) {
    s += q(i) * p(i);
  }
  return s;
}

inline double norm2(const Quat& q) {
  return dot(q, q);
}

inline Quat inv(const Quat& q) {
  const double n = norm2(q);
  assert(n > CS175_EPS2);
  return Quat(q(0), -q(1), -q(2), -q(3)) * (1.0/n);
}

inline Quat normalize(const Quat& q) {
  return q / std::sqrt(norm2(q));
}

inline Matrix4 quatToMatrix(const Quat& q) {
  Matrix4 r;
  const double n = norm2(q);
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
//  Power Operation for quaternion
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
    assert(alpha<=1 || alpha > 0);
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
