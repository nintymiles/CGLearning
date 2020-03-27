//
//  Camera.hpp
//  UtahTeapotIOS
//
//  Created by SeanRen on 2020/3/26.
//  Copyright © 2020 SeanRen. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <stdio.h>

#include "matrix4.h"

struct Camera{
    Matrix4 projMat;
    Matrix4 unProjMat;
};

class PerspectiveCamera:public Camera{
public:
    float aspect;
    float fov;
    float near;
    float far;
    
public:
    PerspectiveCamera(float aspect=1.0,float fov=50.0,float near=-0.1,float far=-10000.0):aspect(aspect),fov(fov),near(near),far(far){
        

        updatePorjectonMatrix();
    };
    ~PerspectiveCamera(){};
    
    void updatePorjectonMatrix(){
        //make a right handed projection matrix
        projMat = Matrix4::makeProjection(fov,aspect,near, far);
        unProjMat = inv(projMat);
    }
    
    
};

#endif /* Camera_hpp */
