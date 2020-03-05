//
//  Box.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Box_h
#define Box_h

#include <stdio.h>
#include <cmath>
#include <limits>

#include <vector>

#include "cvec.h"
#include "Matrix4.h"

using std::vector;

class Box {
    
public:
    Cvec3 min;
    Cvec3 max;
    
    Box(){}
    ~Box(){}
    
    void makeEmpty(){
        double infinity= std::numeric_limits<double>::infinity();
        min[0]=min[1]=min[2]=infinity; //-INFINITY; //in math.h
        max[0]=max[1]=max[2]=-infinity; //INFINITY; //in math.h
    }
    
    bool isEmpty(){
        return ( max[0] < min[0] ) || ( max[1] < min[1] ) || ( max[2] < min[2] );
    }
    
    void setFromPoints(vector<Cvec3> points){
        makeEmpty();
        for(Cvec3 point:points){
            expandByPoint(point);
        }
    }
    
    void expandByPoint(Cvec3 point) {
        min = minVec(point,min);
        max = maxVec(point,max);
    }
    
    //获取的aabb box中心坐标的算法
    Cvec3 getCenter(){
        //如果box未初始化，返回vec3(0,0,0);
        if(isEmpty()) return Cvec3(0,0,0);
        return (min+max) * 0.5f;
    }
    
    void applyMatrix4(Matrix4 matrix){
        Cvec4 tminV4=Cvec4(min,1.0);
        tminV4 = matrix * tminV4;
        min = Cvec3(tminV4);
        
        Cvec4 tmaxV4=Cvec4(max,1.0);
        tmaxV4 = matrix * tmaxV4;
        max = Cvec3(tmaxV4);
    }


};

#endif /* Box_h */
