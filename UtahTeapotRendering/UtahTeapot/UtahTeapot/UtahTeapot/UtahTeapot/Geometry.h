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
#include "shadersasst.h"



// A vertex with floating point position and normal
struct VertexPN {
    Cvec3f p, n;
    
//    //常量FORMAT为本struct对象定义范围内的存在
//    static const VertexFormat FORMAT;
    
    VertexPN() {}
    
    VertexPN(float x, float y, float z,
             float nx, float ny, float nz)
    : p(x,y,z), n(nx, ny, nz) {}
    
    VertexPN(const Cvec3f& pos, const Cvec3f& normal)
    : p(pos), n(normal) {}
    
    VertexPN(const Cvec3& pos, const Cvec3& normal)
    : p(pos[0], pos[1], pos[2]), n(normal[0], normal[1], normal[2]) {}
    
    
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

//XCode快捷键Option+back删除词语，CMD+back删除整行
//VertexPNX是在VertexPN基础上的延伸定义
// A vertex with floating point Position, Normal, and one set of teXture Coordinates;
struct VertexPNX : public VertexPN {
    Cvec2f x; // texture coordinates
    
//    //VertexPNX所属的常量FORMAT
//    static const VertexFormat FORMAT;
    
    VertexPNX() {}
    
    VertexPNX(float x, float y, float z,
              float nx, float ny, float nz,
              float u, float v)
    : VertexPN(x, y, z, nx, ny, nz), x(u, v) {}
    
    VertexPNX(const Cvec3f& pos, const Cvec3f& normal, const Cvec2f& texCoords)
    : VertexPN(pos, normal), x(texCoords) {}
    
    VertexPNX(const Cvec3& pos, const Cvec3& normal, const Cvec2& texCoords)
    : VertexPN(pos, normal), x(texCoords[0], texCoords[1]) {}
    
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPNX(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPNX& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        x = v.tex;
        return *this;
    }
};

//在VertexPNX上继续延伸
// A vertex with floating point Position, Normal, Tangent, Binormal, teXture Coord
struct VertexPNTBX : public VertexPNX {
    Cvec3f t, b; // tangent, binormal
    
//    //隶属于VertexPNTBX所配备的常量
//    static const VertexFormat FORMAT;
    
    VertexPNTBX() {}
    
    VertexPNTBX(float x, float y, float z,
                float nx, float ny, float nz,
                float tx, float ty, float tz,
                float bx, float by, float bz,
                float u, float v)
    : VertexPNX(x, y, z, nx, ny, nz, u, v), t(tx, ty, tz), b(bx, by, bz) {}
    
    VertexPNTBX(const Cvec3f& pos, const Cvec3f& normal,
                const Cvec3f& tangent, const Cvec3f& binormal, const Cvec2f& texCoords)
    : VertexPNX(pos, normal, texCoords), t(tangent), b(binormal) {}
    
    VertexPNTBX(const Cvec3& pos, const Cvec3& normal,
                const Cvec3& tangent, const Cvec3& binormal, const Cvec2& texCoords)
    : VertexPNX(pos, normal, texCoords), t(tangent[0], tangent[1], tangent[2]), b(binormal[0], binormal[1], binormal[2]) {}
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPNTBX(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPNTBX& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        t = v.tangent;
        b = v.binormal;
        x = v.tex;
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
    
    Geometry(VertexPNX *vtx, unsigned short *idx, int vboLen, int iboLen) {
        this->vboLen = vboLen;
        this->iboLen = iboLen;
        
        this->bSphere = computeBoundingSphere(vtx, vboLen);
        this->bBox = computeBoundingBox(vtx, vboLen);
        // Now create the VBO and IBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPNX) * vboLen, vtx, GL_STATIC_DRAW);
        
        if(iboLen>0){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * iboLen, idx, GL_STATIC_DRAW);
        }
        
    }
    
    Geometry(VertexPN *vtx, int vboLen) {
        this->vboLen = vboLen;
        
        // Now create the VBO and IBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPN) * vboLen, vtx, GL_STATIC_DRAW);
        
    }
    
    Sphere computeBoundingSphere(VertexPNX *vtx,int vboLen){
        Sphere sphere;
        vector<Cvec3> vertices;
        for(int i=0;i<vboLen;i++){
            vertices.push_back(Cvec3(vtx[i].p[0],vtx[i].p[1],vtx[i].p[2]));
        }
        sphere.setFromPoints(vertices);
        
        return sphere;
    }
    
    Box computeBoundingBox(VertexPNX *vtx,int vboLen){
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
