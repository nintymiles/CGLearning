//
//  Geometry.h
//  Assignment4-RobotsAndPartPicking
//
//  Created by SeanRen on 2020/3/5.
//  Copyright © 2020 zxtech. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h

#include "glsupport.h"
#include "geometrymaker.h"
#include "Sphere.h"
#include "Box.h"
#include "asstcommon.h"


// A vertex with floating point position and normal
struct VertexPN {
    Cvec3f p, n;
    
    VertexPN() {}
    VertexPN(float x, float y, float z,
             float nx, float ny, float nz)
    : p(x,y,z), n(nx, ny, nz)
    {}
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPN(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPN& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        return *this;
    }
};

// --------- Geometry
// Macro used to obtain relative offset of a field within a struct
#define FIELD_OFFSET(StructType, field) &(((StructType *)0)->field)

struct Geometry {
    GlBufferObject vbo, ibo;
    int vboLen, iboLen;
    Sphere bSphere;
    Box bBox;
    
    Geometry(VertexPN *vtx, unsigned short *idx, int vboLen, int iboLen) {
        this->vboLen = vboLen;
        this->iboLen = iboLen;
        
        this->bSphere = computeBoundingSphere(vtx, vboLen);
        this->bBox = computeBoundingBox(vtx, vboLen);
        // Now create the VBO and IBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPN) * vboLen, vtx, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * iboLen, idx, GL_STATIC_DRAW);
        
    }
    
    Sphere computeBoundingSphere(VertexPN *vtx,int vboLen){
        Sphere sphere;
        vector<Cvec3> vertices;
        for(int i=0;i<vboLen;i++){
            vertices.push_back(Cvec3(vtx[i].p[0],vtx[i].p[1],vtx[i].p[2]));
        }
        sphere.setFromPoints(vertices);
        
        return sphere;
    }
    
    Box computeBoundingBox(VertexPN *vtx,int vboLen){
        Box box;
        vector<Cvec3> vertices;
        for(int i=0;i<vboLen;i++){
            vertices.push_back(Cvec3(vtx[i].p[0],vtx[i].p[1],vtx[i].p[2]));
        }
        box.setFromPoints(vertices);
        
        return box;
    }
    
    void draw(const ShaderState& curSS) {
        GLuint vao;
        glGenVertexArrays( 1, &vao );
        glBindVertexArray(vao);
        // Enable the attributes used by our shader
        safe_glEnableVertexAttribArray(curSS.h_aPosition);
        safe_glEnableVertexAttribArray(curSS.h_aNormal);
        
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        safe_glVertexAttribPointer(curSS.h_aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, p));
        safe_glVertexAttribPointer(curSS.h_aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, n));
        
        // bind ibo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        // draw!
        glDrawElements(GL_TRIANGLES, iboLen, GL_UNSIGNED_SHORT, 0);
        
        // Disable the attributes used by our shader
        safe_glDisableVertexAttribArray(curSS.h_aPosition);
        safe_glDisableVertexAttribArray(curSS.h_aNormal);
        glBindVertexArray(0);
    }
};


#endif /* Geometry_h */
