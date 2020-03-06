//
//  RayCaster.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef RayCaster_h
#define RayCaster_h

#include <stdio.h>
#include <vector>

#include "Ray.h"
#include "Matrix4.h"

#include "scenegraph.h"


class RayCaster{

public:
    Ray* ray;
    float near;
    float far;
    
    RayCaster(){
        ray=new Ray();
    }
    ~RayCaster();
    
    //设置screen-ray
    //On MacOS, mouse coordinate range is actually one quarter of real screen resolution
    void setFromCamera(Cvec3 screenPos,float width,float height,Matrix4 eyeMat,Matrix4 projMat,bool isPerspective){
        float rayOriginX = (screenPos[0]/width) * 2 - 1 + 1/width;
        float rayOriginY = (screenPos[1]/height) * 2 - 1 + 1/height;
        
        if(isPerspective){
            //Matrix4 viewMat= inv(eyeMat);
            Cvec3 camPosition = Cvec3(eyeMat[3],eyeMat[7],eyeMat[11]);
            //矩阵的反转算法应该又问题，当选择z_n值为-1时准确率较高，其它则导致ray的方向矢量偏差极大
            Cvec4 screenPosWorld= (eyeMat * inv(projMat)) * Cvec4(rayOriginX,rayOriginY,-1.0f,1.0f);
            screenPosWorld = screenPosWorld/screenPosWorld[3];
            //screen-ray方向矢量需要标准化
            Cvec3 rayDi =normalize(Cvec3(screenPosWorld) - camPosition);
            
            ray = new Ray(camPosition,rayDi);
        }
        
    }
    
//    vector<IntersectionData> intersectObject(SgShapeNode* object){
//        return object->raycast(this);        
//    }
    
//    vector<IntersectionData> intersectObjects(vector<Model *> objects){
//        vector<IntersectionData> intersects;
//        for(Model *object:objects){
//            if(object->isPickingEnabled()){
//                vector<IntersectionData> intersectData=object->rayCast(this);
//                for(auto data:intersectData)
//                    intersects.push_back(data);
//            }
//        }
//        return intersects;
//    }
};

#endif /* RayCaster_h */
