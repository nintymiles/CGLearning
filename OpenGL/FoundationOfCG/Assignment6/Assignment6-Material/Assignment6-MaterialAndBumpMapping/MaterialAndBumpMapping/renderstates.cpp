#include <stdexcept>

#include "glsupport.h"
#include "renderstates.h"

using namespace std;

static const unsigned int kBlendBit = 1,
kCullFaceBit = 2;

//默认构造函数实现，初始化成员变量为默认值
//知识点：用默认构造函数初始化成员变量
RenderStates::RenderStates()
: glFront(GL_FILL)
, glBack(GL_FILL)
, glBlendSrcFactor(GL_ONE)
, glBlendDstFactor(GL_ZERO)
, glCullFaceMode(GL_BACK)
, flags(kCullFaceBit) {}

//针对不同绘制模式设置polygonMode的对应成员变量值
RenderStates& RenderStates::polygonMode(GLenum face, GLenum mode) {
    switch (mode) {
        case GL_POINT:
        case GL_LINE:
        case GL_FILL:
            switch (face) {
                case GL_FRONT_AND_BACK:
                    glFront = glBack = mode;
                    return *this;
                case GL_FRONT:
                    glFront = mode;
                    return *this;
                case GL_BACK:
                    glBack = mode;
                    return *this;
                default:
                    ;
            }
        default:
            ;
    }
    
    throw invalid_argument("RenderStates::glPolygonMode: invalid argument");
}

//设置blending状态的混合成员变量值
RenderStates& RenderStates::blendFunc(GLenum sfactor, GLenum dfactor) {
    // future TODO: should check if sfactor and dfactor are valid enums
    glBlendSrcFactor = sfactor;
    glBlendDstFactor = dfactor;
    return *this;
}

//面部剔除的成员变量值设置
RenderStates& RenderStates::cullFace(GLenum mode) {
    glCullFaceMode = mode;
    return *this;
}

//开启状态控制位设置？
//知识点？
RenderStates& RenderStates::enable(GLenum target) {
    switch (target) {
        case GL_BLEND:
            flags |= kBlendBit;
            return *this;
        case GL_CULL_FACE:
            flags |= kCullFaceBit;
            return *this;
        default:
            ;
    }
    throw invalid_argument("RenderStates::glEnable: unsupported target");
}

RenderStates& RenderStates::disable(GLenum target) {
    switch (target) {
        case GL_BLEND:
            flags &= ~kBlendBit;
            return *this;
        case GL_CULL_FACE:
            flags &= ~kCullFaceBit;
            return *this;
        default:
            ;
    }
    throw invalid_argument("RenderStates::glEnable: unsupported target");
}

//将成员变量的状态值应用到管线中
// const member function -- one that says a function promises not to modify the invoking ”funciton“ object
void RenderStates::apply() const {
    static bool firstRun = false;
    static RenderStates currentRs;
    //第一次运行则从OpenGL管线获得状态值并fill到RenderState对象中，避免多余的API overheads
    if (firstRun) {
        currentRs.captureFromGl();
        firstRun = false;
    }
    
    //和当前不同，则调用状态设置API
    if (glFront != currentRs.glFront) {
        glPolygonMode(GL_FRONT, glFront);
        currentRs.glFront = glFront;
    }
    if (glBack != currentRs.glBack) {
        glPolygonMode(GL_BACK, glFront);
        currentRs.glBack = glBack;
    }
    if (glBlendSrcFactor != currentRs.glBlendSrcFactor || glBlendDstFactor != currentRs.glBlendDstFactor) {
        glBlendFunc(glBlendSrcFactor, glBlendDstFactor);
        currentRs.glBlendSrcFactor = glBlendSrcFactor;
        currentRs.glBlendDstFactor = glBlendDstFactor;
    }
    
    if (glCullFaceMode != currentRs.glCullFaceMode) {
        glCullFace(glCullFaceMode);
        currentRs.glCullFaceMode = glCullFaceMode;
    }
    
    //位&运算操作决定blending状态的启用关闭？
    if ((flags & kBlendBit) != (currentRs.flags & kBlendBit)) {
        if (flags & kBlendBit)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        currentRs.flags = (currentRs.flags & (~kBlendBit)) | (flags & kBlendBit);
    }
    
    if ((flags & kCullFaceBit) != (currentRs.flags & kCullFaceBit)) {
        if (flags & kCullFaceBit)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
        currentRs.flags = (currentRs.flags & (~kCullFaceBit)) | (flags & kCullFaceBit);
    }
}

//从OpenGL管线获得对应变量的当前状态值
void RenderStates::captureFromGl() {
    GLint values[2];
    
    glGetIntegerv(GL_POLYGON_MODE, values);
    glFront = values[0];
    glBack = values[1];
    
    glGetIntegerv(GL_BLEND_SRC_RGB, values); // Ignore glBlendFuncSeparate fo rnow
    glBlendSrcFactor = values[0];
    
    glGetIntegerv(GL_BLEND_DST_RGB, values);
    glBlendDstFactor = values[0];
    
    glGetIntegerv(GL_CULL_FACE_MODE, values);
    glCullFaceMode = values[0];
    
    flags = 0;
    if (glIsEnabled(GL_BLEND))
        flags |= kBlendBit;
    
    if (glIsEnabled(GL_CULL_FACE))
        flags |= kCullFaceBit;
    
    checkGlError(__func__);
}


