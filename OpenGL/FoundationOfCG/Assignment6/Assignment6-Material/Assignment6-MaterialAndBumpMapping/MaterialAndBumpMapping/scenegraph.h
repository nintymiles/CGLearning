#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <vector>
#include <memory>
#include <stdexcept>


#include "cvec.h"
#include "matrix4.h"
#include "rigtform.h"
#include "glsupport.h" // for Noncopyable
#include "asstcommon.h"
#include "material.h"

using namespace std;

extern shared_ptr<Material> g_overridingMaterial;

//forward declaration
class SgNodeVisitor;

class SgNode : public enable_shared_from_this<SgNode>, Noncopyable {
public:
  virtual bool accept(SgNodeVisitor& vistor) = 0;
  virtual ~SgNode() {}

  // Two nodes are equal if and only if they're the same, i.e.,
  // having the same in memory address
  bool operator == (const SgNode& other) const {
    return this == &other;
  }

  bool operator != (const SgNode& other) const {
    return !(*this == other);
  }

protected:
  SgNode() {}
};

//
// A transform node can have descendents nodes. It uses a
// rigid body transform to represent its frame with respect to
// the parent frame
//
class SgTransformNode : public SgNode {
public:
  virtual bool accept(SgNodeVisitor& visitor);
  virtual RigTForm getRbt() = 0;

  void addChild(shared_ptr<SgNode> child);
  void removeChild(shared_ptr<SgNode> child);

  int getNumChildren() const {
      //此处针对size_type类型执行传统的强制转型
      return (int)children_.size();
  }

  shared_ptr<SgNode> getChild(int i) {
      return children_[i];
  }

private:
    std::vector<shared_ptr<SgNode> > children_;
};

//
// A shape node has no decendents, but can perform drawing. It can also
// provide an affine matrix to be used to modify its geometry.
//
class SgShapeNode : public SgNode {
public:
  virtual bool accept(SgNodeVisitor& visitor);

  virtual Matrix4 getAffineMatrix() = 0;
  virtual void draw(Uniforms& extraUniforms) = 0;
};


// Visitor class for the scene graph nodes. If any of the
// visit/postVisit functions return false, the traverse
// will be terminated.
class SgNodeVisitor {
public:
  virtual bool visit(SgTransformNode& node) { return true; }
  virtual bool visit(SgShapeNode& node) { return true; }

  virtual bool postVisit(SgTransformNode& node) { return true; }
  virtual bool postVisit(SgShapeNode& node) { return true; }
};

//a global function defifnition？
RigTForm getPathAccumRbt(
  shared_ptr<SgTransformNode> source,
  shared_ptr<SgTransformNode> destination,
  int offsetFromDestination = 0);


//----------------------------------------------------
// Concrete scene graph node implementations follow
//----------------------------------------------------

// A SgRoot node is a Transform node with identity Rbt
class SgRootNode : public SgTransformNode {
public:
  SgRootNode() {}

  virtual RigTForm getRbt() {
    return RigTForm();
  }
};

// A SgRbtNode is a Transform node that wraps a RigTForm
class SgRbtNode : public SgTransformNode {
public:
  SgRbtNode(const RigTForm& rbt = RigTForm())
    : rbt_ (rbt) {}

  virtual RigTForm getRbt() {
    return rbt_;
  }

  void setRbt(const RigTForm& rbt) {
    rbt_ = rbt;
  }

private:
  RigTForm rbt_;
};

// A SgGeometryShapeNode is a Shape node that wraps a user geometry class
template<typename Geometry>
class SgGeometryShapeNode : public SgShapeNode {
  shared_ptr<Geometry> geometry_;
  shared_ptr<Material> material_;
  Matrix4 affineMatrix_;
  Cvec3 color_;
public:
  SgGeometryShapeNode(shared_ptr<Geometry> geometry,
                      shared_ptr<Material> material,
                      const Cvec3& color,
                      const Cvec3& translation = Cvec3(0, 0, 0),
                      const Cvec3& eulerAngles = Cvec3(0, 0, 0),
                      const Cvec3& scales = Cvec3(1, 1, 1))
    : geometry_(geometry)
    , material_(material)
    , color_(color)
    , affineMatrix_(Matrix4::makeTranslation(translation) *
                    Matrix4::makeXRotation(eulerAngles[0]) *
                    Matrix4::makeYRotation(eulerAngles[1]) *
                    Matrix4::makeZRotation(eulerAngles[2]) *
                    Matrix4::makeScale(scales)) {}

  virtual Matrix4 getAffineMatrix() {
    return affineMatrix_;
  }

  virtual void draw(Uniforms& extraUniforms){
      Uniforms uniforms = material_->getUniforms();
      if(color_ == NULL)
          color_=Cvec3(1,0,0);
      uniforms.put("uColor",color_);
      //所有的绘制动作都由material发动
      
      //如果存在全局的material，就使用其绘制。
      if(g_overridingMaterial!=NULL)
          g_overridingMaterial->draw(*geometry_,extraUniforms);
      else
          material_->draw(*geometry_,extraUniforms);
  }

};

#endif
