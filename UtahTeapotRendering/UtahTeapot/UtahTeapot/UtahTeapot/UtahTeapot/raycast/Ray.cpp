#include "Ray.h"

#include "Sphere.h"
#include "Box.h"

#include <math.h>

Ray::Ray()
{
    origin = Cvec3();
    direction = Cvec3();
}

Ray::Ray(Cvec3 de, Cvec3 di)
{
    origin = de;
    direction = di;
}

Ray::Ray(const Ray & r)
{
    origin = r.origin;
    direction   = r.direction;
}

Ray& Ray::operator = (const Ray & r)
{
    origin = r.origin;
    direction   = r.direction;
    return *this;
}

void Ray::printInformation() const
{
}

Cvec3 Ray::at(float t){
    if(t<0) return Cvec3(NAN,NAN,NAN);
    return origin+direction*t;
}

//木星之类的大球体选择不精确，是因为形成木星球体的方式为对标准球形几何体的顶点进行伸缩
//故而当从几何体顶点确定球体直径时，所获得的只是标准球体的直径
//要解决这个问题，需要在planetmodel中加入行星的半径信息，可计算BV（sphere）时使用半径信息计算真实行星半径
vector<IntersectionData> Ray::intersectSphere(Sphere* sphere){
    //IntersectionData iData;
    
    Cvec3 v1;
    //两个矢量相减产生新矢量v1，球体原点和射线原点的矢量
    v1 = sphere->center - origin;
    
    //使用标准矢量和非标准矢量的点积来计算余弦边，貌似不太准确。但在这里计算的值一摸一样
    float tca = dot(v1,direction);
    //float ccss = glm::dot(glm::normalize(v1),direction);
    //float tca = sqrt(glm::dot(v1,v1)) * glm::dot(direction,glm::normalize(v1));
    //d2是正弦边的平方，v1平方构成从相机位置和球体中心为最长边平方，tca2为余弦边的平方，
    float d2 = dot(v1,v1) - tca * tca;
    //当d2和radius2刚好相等时，屏幕射线为球体切线，d2>raidus2时，屏幕射线和球体不相交
    float radius2 = sphere->radius * sphere->radius;
    
    //screen-ray和球体不相交
    if ( d2 > radius2 ) return {};
    
    float thc = sqrt( radius2 - d2 );
    
    // t0*ray方向就等于从相机原点到球体表面相交点的距离
    // t0 = first intersect point - entrance on front of sphere
    float t0 = tca - thc;
    // t1为到远距离点的距离
    // t1 = second intersect point - exit point on back of sphere
    float t1 = tca + thc;
    
    // test to see if both t0 and t1 are behind the ray - if so, return null
    if ( t0 < 0 && t1 < 0 ) return {};
    
    // test to see if t0 is behind the ray:
    // if it is, the ray is inside the sphere, so return the second exit point scaled by t1,
    // in order to always return an intersect point that is in front of the ray.
    if ( t0 < 0 ) return {{t1,this->at(t1)}};
    
    // else t0 is in front of the ray, so return the first collision point scaled by t0
    return {{t0,this->at(t0)}};
}

vector<IntersectionData> Ray::intersectBox(Box* box){
    
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    float invdirx = 1 / direction[0],
        invdiry = 1 / direction[1],
        invdirz = 1 / direction[2];
    
    if(invdirx >= 0){
        tmin = (box->min[0] - origin[0]) * invdirx;
        tmax = (box->max[0] - origin[0]) * invdirx;
    }else{
        tmin = (box->max[0] - origin[0]) * invdirx;
        tmax = (box->min[0] - origin[0]) * invdirx;
    }
    
    if(invdiry >= 0 ) {
        tymin = (box->min[1] - origin[1]) * invdiry;
        tymax = (box->max[1] - origin[1]) * invdiry;
    } else {
        tymin = (box->max[1] - origin[1]) * invdiry;
        tymax = (box->min[1] - origin[1]) * invdiry;
        
    }
    
    if ((tmin > tymax) || (tymin > tmax)) return {};
    
    // These lines also handle the case where tmin or tmax is NaN
    // (result of 0 * Infinity). x !== x returns true if x is NaN
    if ( tymin > tmin || tmin != tmin ) tmin = tymin;
    
        if ( tymax < tmax || tmax != tmax ) tmax = tymax;
            
            if ( invdirz >= 0 ) {
                
                tzmin = (box->min[2] - origin[2]) * invdirz;
                tzmax = (box->max[2] - origin[2]) * invdirz;
                
            } else {
                
                tzmin = (box->max[2] - origin[2]) * invdirz;
                tzmax = (box->min[2] - origin[2]) * invdirz;
                
            }
    
    if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) return {};
    
    if ( tzmin > tmin || tmin != tmin ) tmin = tzmin;
        
        if ( tzmax < tmax || tmax != tmax ) tmax = tzmax;
            
            //return point closest to the ray (positive side)
            
            if ( tmax < 0 ) return {};
    
    float t = tmin >= 0 ? tmin : tmax;
    return {{t,this->at(t)}};
}

vector<Cvec3> Ray::intersectTriangle(Cvec3 a,Cvec3 b,Cvec3 c,bool backfaceCulling){
    
    // Compute the offset origin, edges, and normal.
    Cvec3 diff,edge1,edge2,normal;
    
    // from http://www.geometrictools.com/GTEngine/Include/Mathematics/GteIntrRay3Triangle3.h
    edge1 = b-a;
    edge2 = c-a;
    normal = cross(edge1, edge2);
        
    //cross prodcut可以十分方便地应用于determiant的计算
    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N) //确定符号，是因为要计算位于正反面，和korea textbook一致
    float DdN = dot(direction,normal);
    int sign;
        
    if (DdN > 0){
        if (backfaceCulling) return {};
        sign = 1;
    }else if( DdN < 0 ){
        sign = - 1;
        DdN = - DdN;
    } else {
        return {};
    }
        
    //此处Q指向相机原点，于korea text方向相反，故而后面需要负值。
    diff = origin-a;
    
    float DdQxE2 = sign * dot(direction,cross(diff,edge2));
        
    // b1 < 0, no intersection
    if ( DdQxE2 < 0 ) {
        return {};
    }
        
    float DdE1xQ = sign * dot(direction,cross(edge1,diff));
        
    // b2 < 0, no intersection
    if ( DdE1xQ < 0 ) {
        return {};
    }
        
    // b1+b2 > 1, no intersection
    if ( DdQxE2 + DdE1xQ > DdN ) {
        return {};
    }
        
    // Line intersects triangle, check if ray does.
    float QdN = - sign * dot(diff,normal);
        
    // t<0,则位于射线的反方向上？
    // t < 0, no intersection
    if ( QdN < 0 ) {
        return {};
    }
        
    // t值确定后，就可以确定相交点。
    // Ray intersects triangle.
    return {this->at(QdN/DdN)};

}
