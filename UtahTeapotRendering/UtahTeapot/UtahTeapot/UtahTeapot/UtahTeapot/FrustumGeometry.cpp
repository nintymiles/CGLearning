//
//  FrustumGeometry.cpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/25.
//  Copyright © 2020 zxtech. All rights reserved.
//

#include "FrustumGeometry.h"
#include "glsupport.h"

vector<VertexPN> makeFrustVertexPNData(const FrustumGeometry& fg){
    vector<VertexPN> vertexData;
    
    vector<Cvec3> points = fg.vertices;
    vector<Cvec3> colors = fg.colors;
    
    for(int i=0;i<points.size();i++){
        VertexPN v(points[i],colors[i]);
        vertexData.push_back(v);
    }
    
    return vertexData;
}

FrustumGeometry::FrustumGeometry(Matrix4 projMat):projMat(projMat){
    //    var geometry = new BufferGeometry();
    //    var material = new LineBasicMaterial( { color: 0xffffff, vertexColors: FaceColors } );
    
    // colors
    Cvec3 colorFrustum = hexStringToRGB("0xffaa00");
    Cvec3 colorCone = hexStringToRGB("0xff0000");
    Cvec3 colorUp = hexStringToRGB("0x00aaff");
    Cvec3 colorTarget = hexStringToRGB("0xffffff");
    Cvec3 colorCross = hexStringToRGB("0x333333");
    
    // near
    
    addLine( "n1", "n2", colorFrustum );
    addLine( "n2", "n4", colorFrustum );
    addLine( "n4", "n3", colorFrustum );
    addLine( "n3", "n1", colorFrustum );
    
    // far
    
    addLine( "f1", "f2", colorFrustum );
    addLine( "f2", "f4", colorFrustum );
    addLine( "f4", "f3", colorFrustum );
    addLine( "f3", "f1", colorFrustum );
    
    // sides
    
    addLine( "n1", "f1", colorFrustum );
    addLine( "n2", "f2", colorFrustum );
    addLine( "n3", "f3", colorFrustum );
    addLine( "n4", "f4", colorFrustum );
    
    // cone
    
    addLine( "p", "n1", colorCone );
    addLine( "p", "n2", colorCone );
    addLine( "p", "n3", colorCone );
    addLine( "p", "n4", colorCone );
    
    // up
    
    addLine( "u1", "u2", colorUp );
    addLine( "u2", "u3", colorUp );
    addLine( "u3", "u1", colorUp );
    
    // target
    
    addLine( "c", "t", colorTarget );
    addLine( "p", "c", colorCross );
    
    // cross
    
    addLine( "cn1", "cn2", colorCross );
    addLine( "cn3", "cn4", colorCross );
    
    addLine( "cf1", "cf2", colorCross );
    addLine( "cf3", "cf4", colorCross );
    
    
    //LineSegments.call( this, geometry, material );
    
    //    this.camera = camera;
    //    if ( this.camera.updateProjectionMatrix ) this.camera.updateProjectionMatrix();
    //
    //    this.matrix = camera.matrixWorld;
    //    this.matrixAutoUpdate = false;
    
    
    update();
}

void FrustumGeometry::addLine(string a,string b,Cvec3 color){
    addPoint(a, color);
    addPoint(b, color);
    
}

void FrustumGeometry::addPoint(string id, Cvec3 color){
    vertices.push_back(Cvec3(0,0,0));
    colors.push_back(color);
    
    if (pointMap.find(id)==pointMap.end()){
        pointMap[id] = vector<int>();
    }
    
    pointMap[id].push_back((int)vertices.size() - 1);
}

void FrustumGeometry::setPoint(string p,float x,float y,float z){
    //先将ndc坐标投射为eye coordinate
    Cvec4 eyeP4 = inv(projMat) * Cvec4(x,y,z,1.0);
    //点的同质坐标需要转换为标准仿射坐标
    eyeP4 = eyeP4/eyeP4[3];
    Cvec3 eyePos = Cvec3(eyeP4);
    
    vector<int> points = pointMap[p];
    
    if (points.size()>0) {
        
        
        for (int i = 0, l = (int)points.size(); i < l; i ++ ) {
            
            //每个线段两个点在这里会被实实在在地放入到缓存数组中，这个line distances可能无用？
            vertices[points[i]] = eyePos;
        }
        
    }
    
}

void FrustumGeometry::update(){
    // the number for width/height settings
    int w = 1, h = 1;
    
    // we need just camera projection matrix
    // world matrix must be identity
    //camera.projectionMatrix.copy( this.camera.projectionMatrix );
    
    // z coords are 1 when siting on near plane and -1 when sitting on
    // far plane due to right handed ndc coordinates system
    
    // center / target
    setPoint( "c", 0, 0, 1 );
    setPoint( "t", 0, 0, -1 );
    
    // near
    setPoint( "n1", - w, - h, 1 );
    setPoint( "n2", w, - h, 1 );
    setPoint( "n3", - w, h, 1 );
    setPoint( "n4", w, h, 1 );
    
    // far
    setPoint( "f1", - w, - h, -1 );
    setPoint( "f2", w, - h, -1 );
    setPoint( "f3", - w, h, -1 );
    setPoint( "f4", w, h, -1 );
    
    // up
    setPoint( "u1", w * 0.7, h * 1.1, 1 );
    setPoint( "u2", - w * 0.7, h * 1.1, 1 );
    setPoint( "u3", 0, h * 2, 1 );
    
    // cross
    setPoint( "cf1", - w, 0, -1 );
    setPoint( "cf2", w, 0, -1 );
    setPoint( "cf3", 0, - h, -1 );
    setPoint( "cf4", 0, h, -1 );
    
    setPoint( "cn1", - w, 0, 1 );
    setPoint( "cn2", w, 0, 1 );
    setPoint( "cn3", 0, - h, 1 );
    setPoint( "cn4", 0, h, 1 );
    
};
