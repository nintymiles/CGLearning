/*****************************************************************************
 * Source template created by author of MIT-Foundation of 3D computer graphics
 * Implemented by SeanRen for book assignment 3 - 2018/12
 *****************************************************************************/

#ifndef RIGTFORM_H
#define RIGTFORM_H

#include <iostream>
#include <cassert>

#include "matrix4.h"
#include "quat.h"

class RigTForm {
  Cvec3 t_; // translation component
  Quat r_;  // rotation component represented as a quaternion

public:
    RigTForm() : t_(0) {
        assert(norm2(Quat(1,0,0,0) - r_) < CS175_EPS2);
    }
    
    //构造函数可以用：号直接初始化内部成员
    RigTForm(const Cvec3& t, const Quat& r):t_(t),r_(r) {}
    
    //explicit阻止编译器对constructor的输入参数进行隐式转化
    explicit RigTForm(const Cvec3& t):t_(t),r_(1,0,0,0) {}
    
    explicit RigTForm(const Quat& r):r_(r) {}
    
    Cvec3 getTranslation() const {
        return t_;
    }
    
    Quat getRotation() const {
        return r_;
    }
    
    RigTForm& setTranslation(const Cvec3& t) {
        t_ = t;
        return *this;
    }
    
    RigTForm& setRotation(const Quat& r) {
        r_ = r;
        return *this;
    }
    
    Cvec4 operator * (const Cvec4& a) const {
        //A.r * c + A.t
        return  r_ * a + Cvec4(t_,0);
    }
    
    RigTForm operator * (const RigTForm& a) const {
        //translation t 1 + r 1 t 2 and rotation r 1 r 2
        RigTForm rbt;
        rbt.r_ = r_ * a.r_;
        Cvec4 c = r_ * Cvec4(a.t_,0);
        rbt.t_ = t_ + Cvec3(c(0),c(1),c(2));
        return rbt;
    }
    
    static RigTForm identity() {
        RigTForm rbt;
        rbt.r_=Quat(1,0,0,0);
        return rbt;
    }
    static RigTForm makeXRotation(const double ang){
        RigTForm rbt;
        rbt.r_=Quat::makeXRotation(ang);
        return rbt;
    }
    static RigTForm makeYRotation(const double ang){
        RigTForm rbt;
        rbt.r_=Quat::makeYRotation(ang);
        return rbt;
    }
    static RigTForm makeZRotation(const double ang){
        RigTForm rbt;
        rbt.r_=Quat::makeZRotation(ang);
        return rbt;
    }
    static RigTForm makeTranslation(const double tx,const double ty,const double tz){
//        RigTform rbt;
//        rbt.t_ = Cvec4(tx,ty,tz,0);
//        rbt.r_ = Quat(1,0,0,0);
        return RigTForm(Cvec3(tx,ty,tz));
    }
    
};

inline RigTForm inv(const RigTForm& tform) {
    RigTForm rbt;
    
    Quat invQuat = inv(tform.getRotation());
    rbt.setRotation(invQuat);
    
    //此处如果写为（-inv(r)）*t是有问题的，负的quat仍是相同的旋转，故而应为-（inv（r）*t）
    //Cvec4 tran = Quat(-invQuat(0), -invQuat(1), -invQuat(2), -invQuat(3)) * Cvec4(tform.getTranslation(),0);
    Cvec4 tran = -(invQuat * Cvec4(tform.getTranslation(),0));
    rbt.setTranslation(Cvec3(tran(0),tran(1),tran(2)));
    return rbt;
}

inline RigTForm transFact(const RigTForm& tform) {
  return RigTForm(tform.getTranslation());
}

inline RigTForm linFact(const RigTForm& tform) {
  return RigTForm(tform.getRotation());
}

inline RigTForm doQtoOwrtA(const RigTForm& qRbt,const RigTForm& oRbt,const RigTForm& aRbt){
    return aRbt * qRbt * inv(aRbt) * oRbt;
}

inline RigTForm makeMixedFrame(const RigTForm& oRbt,const RigTForm& aRbt){
    return RigTForm(oRbt.getTranslation(), aRbt.getRotation());
}

//--------------------------------------------------------------------------------
//  Conversion to matrix
//  Created for assignment by SeanRen
//--------------------------------------------------------------------------------
inline Matrix4 rigTFormToMatrix(const RigTForm& tform) {
    Matrix4 T = Matrix4::makeTranslation(tform.getTranslation());
    Matrix4 R = quatToMatrix(tform.getRotation());
    return T*R;
}

#endif
