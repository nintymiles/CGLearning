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
//
// But if you do pass in a point behind Z=0 plane, we'll just
// print a warning, and return the center of the screen.
inline Cvec2 getScreenSpaceCoord(const Cvec3& p,
                                 const Matrix4& projection,
                                 double frustNear, double frustFovY,
                                 int screenWidth, int screenHeight) {
  if (p[2] > -CS175_EPS) {
    std::cerr << "WARNING: getScreenSpaceCoord of a point near or behind Z=0 plane. Returning screen-center instead." << std::endl;
    return Cvec2((screenWidth-1)/2.0, (screenHeight-1)/2.0);
  }
  Cvec4 q = projection * Cvec4(p, 1);
  Cvec3 clipCoord = Cvec3(q) / q[3];
  return Cvec2(clipCoord[0] * screenWidth / 2.0 + (screenWidth - 1)/2.0,
               clipCoord[1] * screenHeight / 2.0 + (screenHeight - 1)/2.0);
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
inline double getScreenToEyeScale(double z, double frustFovY, int screenHeight) {
  if (z > -CS175_EPS) {
    std::cerr << "WARNING: getScreenToEyeScale on z near or behind Z=0 plane. Returning 1 instead." << std::endl;
    return 1;
  }
  return -(z * tan(frustFovY * CS175_PI/360.0)) * 2 / screenHeight;
}

//--------------------------------------------------------------------------------
//  实现arcball接口/Implement Arcball Interface 
//--------------------------------------------------------------------------------
inline Quat arcball(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& startScreenPos,const Cvec2& endScreenPos){
    //compute z coordinates of selected 3d points on sphere
    double startScreenZ = sqrt(pow(onScreenRadius,2) - pow((startScreenPos[0] - centerScreenPos[0]),2)  - pow((startScreenPos[1] - centerScreenPos[1]),2));
    double endScreenZ = sqrt(pow(onScreenRadius,2) - pow((endScreenPos[0] - centerScreenPos[0]),2)  - pow((endScreenPos[1] - centerScreenPos[1]),2));
    
    //compute sphere center point to start/end point vectors,then normalize them
    Cvec3 startVector = normalize(Cvec3(startScreenPos,startScreenZ) - centerScreenPos);
    Cvec3 endVector = normalize(Cvec3(endScreenPos,endScreenZ) - centerScreenPos);
    //Phi angle between startVector and endVector,using arc cosine function thorough dot production value of start/end vector;
    double angle = acos(dot(startVector,endVector));
    //rotation axis, first compute cross product of start/end vectors(not necessarily normalized vector),then normalization
    Cvec3 axisVector = normalize(cross(startVector,endVector));
    
    //construct arcball quat by definition
    Quat arcball = Quat(cos(2 * angle), axisVector * sin(2*angle));
    return arcball;
}

inline Quat arcballv2(const Cvec3& centerScreenPos,const double onScreenRadius,const Cvec2& startScreenPos,const Cvec2& endScreenPos){
    //compute z coordinates of selected 3d points on sphere
    double startScreenZ = sqrt(pow(onScreenRadius,2) - pow((startScreenPos[0] - centerScreenPos[0]),2)  - pow((startScreenPos[1] - centerScreenPos[1]),2));
    double endScreenZ = sqrt(pow(onScreenRadius,2) - pow((endScreenPos[0] - centerScreenPos[0]),2)  - pow((endScreenPos[1] - centerScreenPos[1]),2));
    
    //compute sphere center point to start/end point vectors,then normalize them
    Cvec3 startVector = normalize(Cvec3(startScreenPos,startScreenZ) - centerScreenPos);
    Cvec3 endVector = normalize(Cvec3(endScreenPos,endScreenZ) - centerScreenPos);
    
    //construct two quats based start/end vectors
    Quat startInvQuat = inv(Quat(0, startVector));
    Quat endQuat = Quat(0,endVector);
    
    return endQuat * startInvQuat;
}

#endif

