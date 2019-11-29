#ifndef MATERIAL_H
#define MATERIAL_H

#include <cstddef>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>

#include "cvec.h"
#include "matrix4.h"
#include "glsupport.h"
#include "uniforms.h"
#include "renderstates.h"
#include "geometry.h"

struct GlProgramDesc;

//材料类的接口定义
class Material {
public:
    //构造函数，接收两个shader源文件
    Material(const std::string& vsFilename, const std::string& fsFilename);
    //绘制接口，传入geometry和uniforms封装参数
    void draw(Geometry& geometry, const Uniforms& extraUniforms);
    
    Uniforms& getUniforms() { return uniforms_; }
    //第一个const代表返回一个const reference，第二个const代表const memeber function，即函数对象不可改变
    const Uniforms& getUniforms() const { return uniforms_; }
    
    RenderStates& getRenderStates() { return renderStates_; }
    const RenderStates& getRenderStates() const { return renderStates_; }
    
protected:
    std::shared_ptr<GlProgramDesc> programDesc_;
    
    Uniforms uniforms_;
    
    RenderStates renderStates_;
};


#endif
