#include <GL/glew.h>

#include "RayCastPicker.h"
#include "Geometry.h"


using namespace std;

RaycastPicker::RaycastPicker(const RigTForm& initialRbt, const ShaderState& curSS)
  : drawer_(initialRbt, curSS)
  , idCounter_(0)
  , srgbFrameBuffer_(true) {}

RaycastPicker::RaycastPicker(const RigTForm& initialRbt, const ShaderState& curSS,shared_ptr<SgRbtNode> selectedNode,shared_ptr<SgRootNode> worldNode,RigTForm eyeRbt,RigTForm motionRbt):drawer_(initialRbt, curSS)
, idCounter_(0)
, srgbFrameBuffer_(true)
, selectedRbtNode_(selectedNode)
, worldNode_(worldNode)
, eyeRbt_(eyeRbt)
, motionRbt_(motionRbt)
, rayCaster_(new RayCaster()){}

void RaycastPicker::setRayCaster(Cvec3 screenPos,float width,float height,Matrix4 eyeMat,Matrix4 projMat,bool isPerspective){
    rayCaster_->setFromCamera(screenPos, width, height, eyeMat, projMat, isPerspective);
}

bool RaycastPicker::visit(SgTransformNode& node) {
  shared_ptr<SgNode> baseNode = node.shared_from_this();
  nodeStack_.push_back(baseNode);
    if(selectedRbtNode_ == baseNode){
        shared_ptr<SgRbtNode> currentNode = dynamic_pointer_cast<SgRbtNode>(baseNode);
        RigTForm nodeRbt = getPathAccumRbt(worldNode_, currentNode);
        //auxiliary frame wrt nodeRbt and eyeRbt
        RigTForm aRbt = makeMixedFrame(nodeRbt, eyeRbt_);
        RigTForm nodeParentRbt = getPathAccumRbt(worldNode_, currentNode,1);
        RigTForm asRbt = inv(nodeParentRbt) * aRbt;
        RigTForm nodeMotionRbt = doQtoOwrtA(motionRbt_, node.getRbt(), asRbt);
        currentNode->setRbt(nodeMotionRbt);
        return drawer_.visit(*currentNode);
    }
    
  return drawer_.visit(node);
}



bool RaycastPicker::postVisit(SgTransformNode& node) {
  nodeStack_.pop_back();
  return drawer_.postVisit(node);
}

bool RaycastPicker::visit(SgShapeNode& node) {
  // We will increment idcounter every time visit a shape node
    ++idCounter_;
    Cvec3 idColor = idToColor(idCounter_);
    vector<IntersectionData> intersects = raycast(rayCaster_, node);
    if(intersects.size()>0){
        safe_glUniform3f(drawer_.getCurSS().h_uIdColor, 0.3, 0.9, 0.5);
    }else{
        safe_glUniform3f(drawer_.getCurSS().h_uIdColor, idColor[0], idColor[1], idColor[2]);
    }

    shared_ptr<SgNode> baseNode = nodeStack_.back();
    shared_ptr<SgRbtNode> rbtNode = dynamic_pointer_cast<SgRbtNode>(baseNode);
    addToMap(idCounter_, rbtNode);
//    //为选中的节点指定颜色
//    if(selectedRbtNode_ == baseNode){
//        safe_glUniform3f(drawer_.getCurSS().h_uIdColor, 0.3, 0.9, 0.5);
//    }else{
//    safe_glUniform3f(drawer_.getCurSS().h_uIdColor, idColor[0], idColor[1], idColor[2]);
//    }
    
  return drawer_.visit(node);
}

bool RaycastPicker::postVisit(SgShapeNode& node) {
  // TODO
  return drawer_.postVisit(node);
}

vector<IntersectionData> RaycastPicker::raycast(RayCaster* rayCaster,SgShapeNode& node){
    
    shared_ptr<SgNode> baseNode = nodeStack_.back();
    shared_ptr<SgRbtNode> currentRbtNode = dynamic_pointer_cast<SgRbtNode>(baseNode);
    RigTForm nodeWorldRbt = getPathAccumRbt(worldNode_, currentRbtNode,0,1);
    
    SgGeometryShapeNode<Geometry>& geometryNode = dynamic_cast<SgGeometryShapeNode<Geometry>&>(node);
    
    Matrix4 matrixWorld = rigTFormToMatrix(nodeWorldRbt) * geometryNode.getAffineMatrix();
    
    
    //将BV副本转换为world coordinate
    shared_ptr<Geometry> geomP = geometryNode.getGeometry();
    Sphere sphere = geomP->bSphere;
    Box box = geomP->bBox;
    
    sphere.applyMatrix4(matrixWorld);
    
    // 先进行了ray-sphere相交检测
    Ray *ray = rayCaster->ray;
    
//    vector<IntersectionData> intersectPoints = ray->intersectSphere(&sphere);
//    //if (intersectPoints.size()==0) return {};
//    if(intersectPoints.size()>0){
//        return intersectPoints;
//    }
   
    //here ray-box test is a bit more accurate
    box.applyMatrix4(matrixWorld);
    vector<IntersectionData> intersectPoints = ray->intersectBox(&box);
    if(intersectPoints.size()>0){
        selectedRbtNode_ = currentRbtNode;
        return intersectPoints;
    }
    
    return {};
}

shared_ptr<SgRbtNode> RaycastPicker::getSelectedRbtNode(){
    return selectedRbtNode_;
}

//------------------
// Helper functions
//------------------
//
void RaycastPicker::addToMap(int id, shared_ptr<SgRbtNode> node) {
  idToRbtNode_[id] = node;
}

shared_ptr<SgRbtNode> RaycastPicker::find(int id) {
  IdToRbtNodeMap::iterator it = idToRbtNode_.find(id);
  if (it != idToRbtNode_.end())
    return it->second;
  else
    return shared_ptr<SgRbtNode>(); // set to null
}

// encode 2^4 = 16 IDs in each of R, G, B channel, for a total of 16^3 number of objects
static const int NBITS = 4, N = 1 << NBITS, MASK = N-1;

Cvec3 RaycastPicker::idToColor(int id) {
  assert(id > 0 && id < N * N * N);
  Cvec3 framebufferColor = Cvec3(id & MASK, (id >> NBITS) & MASK, (id >> (NBITS+NBITS)) & MASK);
  framebufferColor = framebufferColor / N + Cvec3(0.5/N);

  if (!srgbFrameBuffer_)
    return framebufferColor;
  else {
    // if GL3 is used, the framebuffer will be in SRGB format, and the color we supply needs to be in linear space
    Cvec3 linearColor;
    for (int i = 0; i < 3; ++i) {
      linearColor[i] = framebufferColor[i] <= 0.04045 ? framebufferColor[i]/12.92 : pow((framebufferColor[i] + 0.055)/1.055, 2.4);
    }
    return linearColor;
  }
}

int RaycastPicker::colorToId(const PackedPixel& p) {
  const int UNUSED_BITS = 8 - NBITS;
  int id = p.r >> UNUSED_BITS;
  id |= ((p.g >> UNUSED_BITS) << NBITS);
  id |= ((p.b >> UNUSED_BITS) << (NBITS+NBITS));
  return id;
}
