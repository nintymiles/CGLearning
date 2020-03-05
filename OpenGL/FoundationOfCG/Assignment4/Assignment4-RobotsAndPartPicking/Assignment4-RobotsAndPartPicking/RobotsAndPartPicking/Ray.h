#ifndef Ray_h
#define Ray_h

#include <vector>

#include "cvec.h"
#include "matrix4.h"
#include "quat.h"
#include "Sphere.h"

using std::vector;


class Box;
class IntersectionData; //actually，a structure is an equivalent to a class in C++

//基础相交数据结构定义，如果针对mesh结构几何数据，可能还不足够
struct IntersectionData {
    float distance;
    Cvec3 intersectionPointWorld;
    //Model* object;
};

class Ray{
public:
    Cvec3 origin;
    Cvec3 direction;// destination, dirrection
    
    Ray();
    Ray(Cvec3 de, Cvec3 di);
    Ray(const Ray & r);
    ~Ray();
    
    Ray & operator = (const Ray & r);
    void printInformation() const;
    
    //根据射线属性，将射线方向乘以长度标量t，再加上相机位置，就获得相交点的位置
    Cvec3 at(float t);
    
    //ray-sphere相交的基本计算
    vector<IntersectionData> intersectSphere(Sphere* sphere);
    //ray-box相交
    vector<IntersectionData> intersectBox(Box* box);
    //ray-triangle
    vector<Cvec3> intersectTriangle(Cvec3 a,Cvec3 b,Cvec3 c,bool backfaceculling);
    
    void applyMatrix4(Matrix4 matrix){
        Cvec4 originV4=Cvec4(origin,1.0);
        origin = Cvec3(matrix * originV4);
        
        Cvec4 directionV4 = Cvec4(direction,1.0);
        direction = Cvec3(matrix * directionV4);
    }
    
    void applyQuaternion(Quat q){
        Cvec4 originV4=Cvec4(origin,1.0);
        origin = Cvec3(q * originV4);
        
        Cvec4 directionV4 = Cvec4(direction,1.0);
        direction = Cvec3(q * directionV4);
    }
    
    
};

#endif
