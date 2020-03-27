//
//  FrustumGeometry.hpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/25.
//  Copyright © 2020 zxtech. All rights reserved.
//

#ifndef FrustumGeometry_hpp
#define FrustumGeometry_hpp

#include <stdio.h>
#include <map>
#include <vector>

#include "cvec.h"
#include "Geometry.h"

#include "matrix4.h"

using namespace std;

class FrustumGeometry{
    std::map<std::string, std::vector<int>> pointMap;
    std::vector<Cvec3> vertices;
    std::vector<Cvec3> colors;
    
    Matrix4 projMat;
    
    void addLine(string a,string b,Cvec3 color);
    void addPoint(string id, Cvec3 color);
    void setPoint(string p,float x,float y,float z);
    void update();
    
    
public:
    //FrustumGeometry(){FrustumGeometry(Matrix4());};
    FrustumGeometry(Matrix4 projMat);
    ~FrustumGeometry(){};
    
    void updateWithProjMat(Matrix4 proj);
    
    friend vector<VertexPN> makeFrustVertexPNData(const FrustumGeometry&);
};

vector<VertexPN> makeFrustVertexPNData(const FrustumGeometry&);

#endif /* FrustumGeometry_hpp */
