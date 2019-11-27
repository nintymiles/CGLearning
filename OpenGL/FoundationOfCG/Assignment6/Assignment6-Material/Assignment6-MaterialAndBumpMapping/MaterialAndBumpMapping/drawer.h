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
  Material& material_;
public:
  Drawer(const RigTForm& initialRbt, Material& material)
    : rbtStack_(1, initialRbt)
    , material_(material) {}

    //visit will push the last rbtStack RBT * node RBT in rbtStack
  virtual bool visit(SgTransformNode& node) {
    rbtStack_.push_back(rbtStack_.back() * node.getRbt());
    return true;
  }

  virtual bool postVisit(SgTransformNode& node) {
    rbtStack_.pop_back();
    return true;
  }

  virtual bool visit(SgShapeNode& shapeNode) {
    //back() function return the last element in the container
    Matrix4 MVM = rigTFormToMatrix(rbtStack_.back()) * shapeNode.getAffineMatrix();
    Matrix4 NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(material_.getUniforms(), MVM, NMVM);
    shapeNode.draw(material_);
    return true;
  }

  virtual bool postVisit(SgShapeNode& shapeNode) {
    return true;
  }

  const Material& getCurMat() const {
    return material_;
  }
};

#endif



