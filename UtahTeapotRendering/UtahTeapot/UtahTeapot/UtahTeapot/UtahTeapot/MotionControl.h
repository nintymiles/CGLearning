//
//  MotionControl
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/13.
//  Copyright © 2020 zxtech. All rights reserved.
//

#ifndef MotionControl_H
#define MotionControl_H

#include <stdio.h>
#include <vector>
#include <string>

#include "cvec.h"
#include "quat.h"
#include "matrix4.h"



/******************************************************************
 * Motion control helper class
 * This class is mainly used for objects motion control in samples.
 *
 */
class MotionControl{
private:
    // arcball
    Cvec2 vec_ball_center_;
    float ball_radius_;
    Quat quat_ball_now_;
    Quat quat_ball_down_;
    Cvec2 vec_ball_now_;
    Cvec2 vec_ball_down_;
    Quat quat_ball_rot_;
    
    bool dragging_;
    bool pinching_;
    
    // Pinch related info
    Cvec2 vec_pinch_start_;
    Cvec2 vec_pinch_start_center_;
    float pinch_start_distance_SQ_;
    
    // Camera shift
    Cvec3 vec_offset_;
    Cvec3 vec_offset_now_;
    
    // Camera Rotation
    float camera_rotation_;
    float camera_rotation_start_;
    float camera_rotation_now_;
    
    // Momentum support
    bool momentum_;
    double time_stamp_;
    Cvec2 vec_drag_delta_;
    Cvec2 vec_last_input_;
    Cvec3 vec_offset_last_;
    Cvec3 vec_offset_delta_;
    float momemtum_steps_;
    
    Cvec2 vec_flip_;
    float flip_z_;
    
    Matrix4 mat_rotation_;
    Matrix4 mat_transform_;
    
    Cvec3 vec_pinch_transform_factor_;
    
    Cvec3 PointOnSphere(Cvec2& point);
    void BallUpdate();
    void InitParameters();
    
public:
    MotionControl();
    virtual ~MotionControl();
    void BeginDrag(const Cvec2& vec);
    void EndDrag();
    void Drag(const Cvec2& vec);
    void Update();
    void Update(const double time);
    
    Matrix4& GetRotationMatrix();
    Matrix4& GetTransformMatrix();
    
    void BeginPinch(const Cvec2& v1, const Cvec2& v2);
    void EndPinch();
    void Pinch(const Cvec2& v1, const Cvec2& v2);
    
    void SetFlip(const float x, const float y, const float z) {
        vec_flip_ = Cvec2(x, y);
        flip_z_ = z;
    }
    
    void SetPinchTransformFactor(const float x, const float y, const float z) {
        vec_pinch_transform_factor_ = Cvec3(x, y, z);
    }
    
    void Reset(const bool bAnimate);
};



#endif /* MotionControl_H */
