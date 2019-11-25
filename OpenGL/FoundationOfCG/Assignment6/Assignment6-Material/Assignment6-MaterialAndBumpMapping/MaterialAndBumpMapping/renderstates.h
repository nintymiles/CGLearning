#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "glsupport.h"

// Currently the following GL states are supported:
//
// - glPolygonMode  (Default: GL_FRONT_AND_BACK, GL_FILL)
// - glBlendFunc    (Default: GL_ONE, GL_ZERO)
// - glCullFace     (Default: GL_BACK)
//
// The following flags for glEnable/glDisable are supported
//
// - GL_BLEND       (Default: off)
// - GL_CULL_FACE   (Default: on)

class RenderStates {
    //声明了几种状态
    GLenum glFront, glBack; // for polygonMode
    GLenum glBlendSrcFactor, glBlendDstFactor; // for blendFunc
    GLenum glCullFaceMode; // for cullFace
    unsigned int flags;
    
public:
    RenderStates();
    
    //不管设置什么状态，返回参数为RenderStates对象引用
    //知识点：操作函数的返回值设计
    RenderStates& polygonMode(GLenum face, GLenum mode);
    RenderStates& blendFunc(GLenum sfactor, GLenum dfactor);
    RenderStates& cullFace(GLenum mode);
    
    RenderStates& enable(GLenum target);
    RenderStates& disable(GLenum target);
    
    void apply() const;
    void captureFromGl();
};

#endif
