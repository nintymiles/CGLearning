#ifndef PICKER_H
#define PICKER_H

#include <vector>
#include <map>
#include <memory>
#include <stdexcept>


#include "cvec.h"
#include "scenegraph.h"
#include "asstcommon.h"
#include "ppm.h"
#include "drawer.h"

using namespace std;

class Picker : public SgNodeVisitor {
    vector<shared_ptr<SgNode> > nodeStack_;
    
    typedef map<int, shared_ptr<SgRbtNode> > IdToRbtNodeMap;
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
    //initialRbt is actually viewing (inverted eye) rbt
    Picker(const RigTForm& initialRbt, Uniforms& uniforms);
    Picker(const RigTForm& initialRbt, Uniforms& uniforms,shared_ptr<SgRbtNode> selectedNode,shared_ptr<SgRootNode> worldNode,RigTForm eyeRbt,RigTForm motionRbt);
    
    virtual bool visit(SgTransformNode& node);
    virtual bool postVisit(SgTransformNode& node);
    virtual bool visit(SgShapeNode& node);
    virtual bool postVisit(SgShapeNode& node);
    
    shared_ptr<SgRbtNode> getRbtNodeAtXY(int x, int y);
};


#endif
