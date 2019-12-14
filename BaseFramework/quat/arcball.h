#ifndef ARCBALL_H
#define ARCBALL_H

#include <iostream>
#include <cmath>
#include "cvec.h"
#include "quat.h"

// Return the screen space projection in terms of pixels of a 3d point
// given in eye-frame coordinates. 
//
// Ideally you should never call this for a point behind the Z=0 plane,
// sinch such a point wouldn't be visible.
// 在眼睛之后的点无法被投射显示
//
// But if you do pass in a point behind Z=0 plane, we'll just
// print a warning, and return the center of the screen.
inline Cvec2 getScreenSpaceCoord(const Cvec3& p,
                                 const Matrix4& projection,
                                 double frustNear, double frustFovY,
                                 int screenWidth, int screenHeight) {
    //如果球体中心坐标（eye coordinate）的z值大于0，则这个点实际是位于眼睛之后，无法被投射
    //针对这种情况，输出warning信息，并默认返回屏幕中心作为球体中心投射坐标。这种默认值存在的意义？
    if (p[2] > -CS175_EPS) {
        //std::cerr << "WARNING: getScreenSpaceCoord of a point near or behind Z=0 plane. Returning screen-center instead." << std::endl;
        return Cvec2((screenWidth-1)/2.0, (screenHeight-1)/2.0);
    }
    //将球体中心的眼部坐标应用投射变换
    Cvec4 cliqCoord = projection * Cvec4(p, 1);
    //执行perspective division，变换为normalized device coordinate
    Cvec3 ndCoord = Cvec3(cliqCoord) / cliqCoord[3];
    //将ndc转化为window coordinate
    return Cvec2(ndCoord[0] * screenWidth / 2.0 + (screenWidth - 1)/2.0,
                 ndCoord[1] * screenHeight / 2.0 + (screenHeight - 1)/2.0);
}

// Return the scale between 1 unit in screen pixels and 1 unit in the eye-frame
// (or world-frame, since we always use rigid transformations to represent one
// frame with resepec to another frame)
//
// Ideally you should never call this using a z behind the Z=0 plane,
// sinch such a point wouldn't be visible.  
//
// But if you do pass in a point behind Z=0 plane, we'll just
// print a warning, and return 1
//注意这个z值是eye frame中的z值，如果以ndc中的z值来表示则可能失去比值意义。
inline double getScreenToEyeScale(double z, double frustFovY, int screenHeight) {
    if (z > -CS175_EPS) {
        std::cerr << "WARNING: getScreenToEyeScale on z near or behind Z=0 plane. Returning 1 instead." << std::endl;
        return 1;
    }
    //先计算出z（in eye frame）值所对应的frustum切面的高度，再用这个高度除以屏幕高度就得到比率
    return -(z * tan(frustFovY * CS175_PI/360.0)) * 2 / screenHeight;
}

//计算球体在window coordinates中的z值
//参数：球体中心屏幕坐标，球体屏幕上的半径，球体表面点的screen坐标
inline double calcSphereScreenZ(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& screenPos){
    double squareScreenZ = pow(onScreenRadius,2.0) - pow((screenPos[0] - centerScreenPos[0]),2.0)  - pow((screenPos[1] - centerScreenPos[1]),2.0);
    if(squareScreenZ < 0)
        return 0;
    else
        return sqrt(squareScreenZ);
}

//--------------------------------------------------------------------------------
//  实现trackball接口/Implement Trackball Interface
//--------------------------------------------------------------------------------
inline Quat trackball(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& startScreenPos,const Cvec2& endScreenPos){
    //compute z coordinates of selected 3d points on sphere
    double startScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, startScreenPos);
    double endScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, endScreenPos);
    
    //compute sphere center point to start/end point vectors,then normalize them
    Cvec3 startVector = normalize(Cvec3(startScreenPos,startScreenZ) - centerScreenPos);
    Cvec3 endVector = normalize(Cvec3(endScreenPos,endScreenZ) - centerScreenPos);
    
    if(startVector != Cvec3(0) && endVector!=Cvec3(0) && startVector != endVector){
        //Phi angle between startVector and endVector,using arc cosine function thorough dot production value of start/end vector;
        double angle = acos(dot(startVector,endVector));
        
        Cvec3 axisVector = normalize(cross(startVector,endVector));
        
        //construct trackball quat by definition
        Quat trackball = Quat(cos(0.5 * angle), axisVector * sin(0.5 * angle));
        return trackball;
    }else{
        return Quat::identity();
    }
}

//--------------------------------------------------------------------------------
//  实现arcball接口/Implement Arcball Interface 
//--------------------------------------------------------------------------------
inline Quat arcball(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& startScreenPos,const Cvec2& endScreenPos){
    //compute z coordinates of selected 3d points on sphere
    double startScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, startScreenPos);
    double endScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, endScreenPos);
    
    //compute sphere center point to start/end point vectors,then normalize them
    Cvec3 startVector = normalize(Cvec3(startScreenPos,startScreenZ) - centerScreenPos);
    Cvec3 endVector = normalize(Cvec3(endScreenPos,endScreenZ) - centerScreenPos);

    // incorrect usage for Nan
    //    double angle = acos(dot(startVector,endVector));
    //    if(angle == NAN)
    //        return Quat::identity();
   
    //如果startVector和endVector不为0矢量，且startVector和endVector不相同，然后才计算arcball接口旋转
    if(startVector != Cvec3(0) && endVector!=Cvec3(0) && startVector != endVector){
        //Phi angle between startVector and endVector,using arc cosine function thorough dot production value of start/end vector;
        double angle = acos(dot(startVector,endVector));
        
        
         //rotation axis, first compute cross product of start/end vectors(not necessarily normalized vector),then normalization
        Cvec3 axisVector = normalize(cross(startVector,endVector));
        //construct arcball quat by definition
        Quat arcball = Quat(cos(angle), axisVector * sin(angle));
        return arcball;
    }else{
        return Quat::identity();
    }
    
}

//--------------------------------------------------------------------------------
//  arcball V2 - compute final quat based on arcball definition deriative
//--------------------------------------------------------------------------------
inline Quat arcballv2(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& startScreenPos,const Cvec2& endScreenPos){
    //compute z coordinates of selected 3d points on sphere
    double startScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, startScreenPos);
    double endScreenZ = calcSphereScreenZ(centerScreenPos, onScreenRadius, endScreenPos);
    
    //compute sphere center point to start/end point vectors,then normalize them
    Cvec3 startVector = normalize(Cvec3(startScreenPos,startScreenZ) - centerScreenPos);
    Cvec3 endVector = normalize(Cvec3(endScreenPos,endScreenZ) - centerScreenPos);
    
    if(startVector != Cvec3(0) && endVector!=Cvec3(0) && startVector != endVector){
        //construct two quats based start/end vectors
        Quat startInvQuat = inv(Quat(0, startVector));
        Quat endQuat = Quat(0,endVector);
        
        return endQuat * startInvQuat;
    }else{
        return Quat::identity();
    }
}

#endif

