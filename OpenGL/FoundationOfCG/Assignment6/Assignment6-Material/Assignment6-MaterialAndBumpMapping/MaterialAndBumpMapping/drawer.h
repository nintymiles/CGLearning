#ifndef DRAWER_H
#define DRAWER_H

#include <vector>

#include "scenegraph.h"
#include "asstcommon.h"
#include "material.h"
#include "matrix4.h"

class Drawer : public SgNodeVisitor {
protected:
    std::vector<RigTForm> rbtStack_;
    Uniforms& uniforms_;
public:
    Drawer(const RigTForm& initialRbt, Uniforms& extraUniforms)
    : rbtStack_(1, initialRbt)
    , uniforms_(extraUniforms) {}
    
    //visit will push the last rbtStack RBT * node RBT in rbtStack
    virtual bool visit(SgTransformNode& node) {
        rbtStack_.push_back(rbtStack_.back() * node.getRbt());
        return true;
    }
    
    virtual bool postVisit(SgTransformNode& node) {
        rbtStack_.pop_back();
        return true;
    }
    
  //scenegraph中包含了modelview矩阵信息，故而modelview相关矩阵要从extraUniforms中送入shader，否则linked tree结构无法发挥作用。
    virtual bool visit(SgShapeNode& shapeNode) {
        //back() function return the last element in the container
        Matrix4 MVM = rigTFormToMatrix(rbtStack_.back()) * shapeNode.getAffineMatrix();
        Matrix4 NMVM = normalMatrix(MVM);
        //sendModelViewNormalMatrix(material_.getUniforms(), MVM, NMVM);
        
        sendModelViewNormalMatrix(uniforms_, MVM, NMVM);
        shapeNode.draw(uniforms_);
        return true;
    }
    
    virtual bool postVisit(SgShapeNode& shapeNode) {
        return true;
    }
    
    //返回附加的uniforms（Material之外的）
    const Uniforms& getUniforms() const {
        return uniforms_;
    }
};

#endif



