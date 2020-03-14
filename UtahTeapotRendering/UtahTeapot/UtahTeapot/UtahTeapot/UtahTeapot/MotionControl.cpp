//
//  Camera.cpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/13.
//  Copyright © 2020 zxtech. All rights reserved.
//

#include "MotionControl.h"

#include <fstream>

    const float TRANSFORM_FACTOR = 15.f;
    const float TRANSFORM_FACTORZ = 10.f;
    
    const float MOMENTUM_FACTOR_DECREASE = 0.85f;
    const float MOMENTUM_FACTOR_DECREASE_SHIFT = 0.9f;
    const float MOMENTUM_FACTOR = 0.8f;
    const float MOMENTUM_FACTOR_THRESHOLD = 0.001f;
    
    //----------------------------------------------------------
    //  Ctor
    //----------------------------------------------------------
    MotionControl::MotionControl()
    : ball_radius_(0.75f),
    dragging_(false),
    pinching_(false),
    pinch_start_distance_SQ_(0.f),
    camera_rotation_(0.f),
    camera_rotation_start_(0.f),
    camera_rotation_now_(0.f),
    momentum_(false),
    momemtum_steps_(0.f),
    flip_z_(0.f) {
        // Init offset
        InitParameters();
        
        vec_flip_ = Cvec2(1.f, -1.f);
        flip_z_ = -1.f;
        vec_pinch_transform_factor_ = Cvec3(1.f, 1.f, 1.f);
        
        vec_ball_center_ = Cvec2(0, 0);
        vec_ball_now_ = Cvec2(0, 0);
        vec_ball_down_ = Cvec2(0, 0);
        
        vec_pinch_start_ = Cvec2(0, 0);
        vec_pinch_start_center_ = Cvec2(0, 0);
        
        vec_flip_ = Cvec2(0, 0);
    }
    
    void MotionControl::InitParameters() {
        // Init parameters
        vec_offset_ = Cvec3();
        vec_offset_now_ = Cvec3();
        
        quat_ball_rot_ = Quat();
        quat_ball_now_ = Quat();
        mat_rotation_= quatToMatrix(quat_ball_now_);
        camera_rotation_ = 0.f;
        
        vec_drag_delta_ = Cvec2();
        vec_offset_delta_ = Cvec3();
        
        momentum_ = false;
    }
    
    //----------------------------------------------------------
    //  Dtor
    //----------------------------------------------------------
    MotionControl::~MotionControl() {}
    
    void MotionControl::Update() {
        if (momentum_) {
            float momenttum_steps = momemtum_steps_;
            
            // Momentum rotation
            Cvec2 v = vec_drag_delta_;
            BeginDrag(Cvec2());  // NOTE:This call reset _VDragDelta
            //why？？？
            Drag(Cvec2(v[0] * vec_flip_[0],v[1] * vec_flip_[1]));
            
            // Momentum shift
            vec_offset_ += vec_offset_delta_;
            
            BallUpdate();
            EndDrag();
            
            // Decrease deltas
            vec_drag_delta_ = v * MOMENTUM_FACTOR_DECREASE;
            vec_offset_delta_ = vec_offset_delta_ * MOMENTUM_FACTOR_DECREASE_SHIFT;
            
            // Count steps
            momemtum_steps_ = momenttum_steps * MOMENTUM_FACTOR_DECREASE;
            if (momemtum_steps_ < MOMENTUM_FACTOR_THRESHOLD) {
                momentum_ = false;
            }
        } else {
            vec_drag_delta_ *= MOMENTUM_FACTOR;
            vec_offset_delta_ = vec_offset_delta_ * MOMENTUM_FACTOR;
            BallUpdate();
        }
        
        Cvec3 vec = vec_offset_ + vec_offset_now_;
        Cvec3 vec_tmp(TRANSFORM_FACTOR, -TRANSFORM_FACTOR, TRANSFORM_FACTORZ);
        
        vec *= vec_tmp * vec_pinch_transform_factor_;
        
        mat_transform_ = Matrix4::makeTranslation(vec);
    }
    
    void MotionControl::Update(const double time) {
        if (momentum_) {
            const float MOMENTAM_UNIT = 0.0166f;
            // Activate every 16.6msec
            if (time - time_stamp_ >= MOMENTAM_UNIT) {
                float momenttum_steps = momemtum_steps_;
                
                // Momentum rotation
                Cvec2 v = vec_drag_delta_;
                BeginDrag(Cvec2());  // NOTE:This call reset _VDragDelta
                Drag(v * vec_flip_);
                
                // Momentum shift
                vec_offset_ += vec_offset_delta_;
                
                BallUpdate();
                EndDrag();
                
                // Decrease deltas
                vec_drag_delta_ = v * MOMENTUM_FACTOR_DECREASE;
                vec_offset_delta_ = vec_offset_delta_ * MOMENTUM_FACTOR_DECREASE_SHIFT;
                
                // Count steps
                momemtum_steps_ = momenttum_steps * MOMENTUM_FACTOR_DECREASE;
                if (momemtum_steps_ < MOMENTUM_FACTOR_THRESHOLD) {
                    momentum_ = false;
                }
                time_stamp_ = time;
            }
        } else {
            vec_drag_delta_ *= MOMENTUM_FACTOR;
            vec_offset_delta_ = vec_offset_delta_ * MOMENTUM_FACTOR;
            BallUpdate();
            time_stamp_ = time;
        }
        
        Cvec3 vec = vec_offset_ + vec_offset_now_;
        Cvec3 vec_tmp(TRANSFORM_FACTOR, -TRANSFORM_FACTOR, TRANSFORM_FACTORZ);
        
        vec *= vec_tmp * vec_pinch_transform_factor_;
        
        mat_transform_ = Matrix4::makeTranslation(vec);
    }
    Matrix4& MotionControl::GetRotationMatrix() { return mat_rotation_; }
    
    Matrix4& MotionControl::GetTransformMatrix() { return mat_transform_; }
    
    void MotionControl::Reset(const bool bAnimate) {
        InitParameters();
        Update();
    }
    
    //----------------------------------------------------------
    // Drag control
    //----------------------------------------------------------
    void MotionControl::BeginDrag(const Cvec2& v) {
        if (dragging_) EndDrag();
        
        if (pinching_) EndPinch();
        
        Cvec2 vec = v * vec_flip_;
        vec_ball_now_ = vec;
        vec_ball_down_ = vec_ball_now_;
        
        dragging_ = true;
        momentum_ = false;
        vec_last_input_ = vec;
        vec_drag_delta_ = Cvec2();
    }
    
    void MotionControl::EndDrag() {
        quat_ball_down_ = quat_ball_now_;
        quat_ball_rot_ = Quat();
        
        dragging_ = false;
        momentum_ = true;
        momemtum_steps_ = 1.0f;
    }
    
    void MotionControl::Drag(const Cvec2& v) {
        if (!dragging_) return;
        
        Cvec2 vec = v * vec_flip_;
        vec_ball_now_ = vec;
        
        vec_drag_delta_ = vec_drag_delta_ * MOMENTUM_FACTOR + (vec - vec_last_input_);
        vec_last_input_ = vec;
    }
    
    //----------------------------------------------------------
    // Pinch controll
    //----------------------------------------------------------
    void MotionControl::BeginPinch(const Cvec2& v1, const Cvec2& v2) {
        if (dragging_) EndDrag();
        
        if (pinching_) EndPinch();
        
        BeginDrag(Cvec2());
        
        vec_pinch_start_center_ = (v1 + v2) / 2.f;
        
        Cvec2 vec = v1 - v2;
        float x_diff=vec[0];
        float y_diff=vec[1];
        //vec.Value(x_diff, y_diff);
        
        
        pinch_start_distance_SQ_ = x_diff * x_diff + y_diff * y_diff;
        camera_rotation_start_ = atan2f(y_diff, x_diff);
        camera_rotation_now_ = 0;
        
        pinching_ = true;
        momentum_ = false;
        
        // Init momentum factors
        vec_offset_delta_ = Cvec3();
    }
    
    void MotionControl::EndPinch() {
        pinching_ = false;
        momentum_ = true;
        momemtum_steps_ = 1.f;
        vec_offset_ += vec_offset_now_;
        camera_rotation_ += camera_rotation_now_;
        vec_offset_now_ = Cvec3();
        
        camera_rotation_now_ = 0;
        
        EndDrag();
    }
    
    void MotionControl::Pinch(const Cvec2& v1, const Cvec2& v2) {
        if (!pinching_) return;
        
        // Update momentum factor
        vec_offset_last_ = vec_offset_now_;
        
        Cvec2 vec = v1 - v2;
        float x_diff=vec[0];
        float y_diff=vec[1];
        //vec.Value(x_diff, y_diff);
        
        float fDistanceSQ = x_diff * x_diff + y_diff * y_diff;
        
        float f = pinch_start_distance_SQ_ / fDistanceSQ;
        if (f < 1.f)
        f = -1.f / f + 1.0f;
        else
        f = f - 1.f;
        if (std::isnan(f)) f = 0.f;
        
        vec = (v1 + v2) / 2.f - vec_pinch_start_center_;
        vec_offset_now_ = Cvec3(vec, flip_z_ * f);
        
        // Update momentum factor
        vec_offset_delta_ = vec_offset_delta_ * MOMENTUM_FACTOR +
        (vec_offset_now_ - vec_offset_last_);
        
        //
        // Update ration quaternion
        float fRotation = atan2f(y_diff, x_diff);
        camera_rotation_now_ = fRotation - camera_rotation_start_;
        
        // Trackball rotation
        quat_ball_rot_ = Quat(cosf(-camera_rotation_now_ * 0.5f),0.f, 0.f, sinf(-camera_rotation_now_ * 0.5f));
    }
    
    //----------------------------------------------------------
    // arcball interface simply implementation
    //----------------------------------------------------------
    void MotionControl::BallUpdate() {
        if (dragging_) {
            Cvec3 vec_from = PointOnSphere(vec_ball_down_);
            Cvec3 vec_to = PointOnSphere(vec_ball_now_);
            
            Cvec3 vec = cross(vec_from,vec_to);
            float w = dot(vec_from,vec_to);
            
            Quat qDrag = Quat(w,vec);
            qDrag = qDrag * quat_ball_down_;
            quat_ball_now_ = quat_ball_rot_ * qDrag;
        }
        mat_rotation_=quatToMatrix(quat_ball_now_);
    }
    
    Cvec3 MotionControl::PointOnSphere(Cvec2& point) {
        Cvec3 ball_mouse;
        float mag;
        Cvec2 vec = (point - vec_ball_center_) / ball_radius_;
        mag = dot(vec,vec);
        if (mag > 1.f) {
            float scale = 1.f / sqrtf(mag);
            vec *= scale;
            ball_mouse = Cvec3(vec, 0.f);
        } else {
            ball_mouse = Cvec3(vec, sqrtf(1.f - mag));
        }
        return ball_mouse;
    }
