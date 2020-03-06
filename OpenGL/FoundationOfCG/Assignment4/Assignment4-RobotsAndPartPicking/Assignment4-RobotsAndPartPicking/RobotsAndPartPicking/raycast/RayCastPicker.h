#ifndef RayCastPicker_H
#define RayCastPicker_H

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>


#include "cvec.h"
#include "scenegraph.h"
#include "asstcommon.h"
#include "ppm.h"
#include "drawer.h"
#include "RayCaster.h"

using namespace std;
//struct Geometry;

class RaycastPicker : public SgNodeVisitor {
  vector<shared_ptr<SgNode>> nodeStack_;
  RayCaster *rayCaster_;
    

  typedef std::map<int, shared_ptr<SgRbtNode> > IdToRbtNodeMap;
  IdToRbtNodeMap idToRbtNode_;

  int idCounter_;
  bool srgbFrameBuffer_;
    shared_ptr<SgRbtNode> selectedRbtNode_;
    shared_ptr<SgRootNode> worldNode_;
    RigTForm eyeRbt_;
    RigTForm motionRbt_;

  Drawer drawer_;

  void addToMap(int id, shared_ptr<SgRbtNode> node);
  shared_ptr<SgRbtNode> find(int id);
  Cvec3 idToColor(int id);
  int colorToId(const PackedPixel& p);

public:
  RaycastPicker(const RigTForm& initialRbt, const ShaderState& curSS);
    RaycastPicker(const RigTForm& initialRbt, const ShaderState& curSS,shared_ptr<SgRbtNode> selectedNode,shared_ptr<SgRootNode> worldNode,RigTForm eyeRbt,RigTForm motionRbt);

  virtual bool visit(SgTransformNode& node);
  virtual bool postVisit(SgTransformNode& node);
  virtual bool visit(SgShapeNode& node);
  virtual bool postVisit(SgShapeNode& node);
    virtual void setRayCaster(Cvec3 screenPos,float width,float height,Matrix4 eyeMat,Matrix4 projMat,bool isPerspective);
    
    shared_ptr<SgRbtNode> getSelectedRbtNode();
    
    vector<IntersectionData> raycast(RayCaster* raycater,SgShapeNode& node);
};


#endif //RayCastPicker_H
