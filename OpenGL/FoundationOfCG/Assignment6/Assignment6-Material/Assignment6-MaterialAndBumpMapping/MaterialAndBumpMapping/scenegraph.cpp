#include <algorithm>

#include "scenegraph.h"

using namespace std;

//一直迭代直到最终找到对应子节点，visit步骤执行完毕，程序return false（意味着压stack完毕）；
//否则一直执行到子步骤执行完毕，最后执行到postVisit。也意味着没有子节点，则当前stack压入后直接再pop出去。
bool SgTransformNode::accept(SgNodeVisitor& visitor) {
    //本节点是否允许请求和访问
    if (!visitor.visit(*this))
        return false;
    //对子节点进行遍历请求和访问
    for (long i = 0, n = children_.size(); i < n; ++i) {
        if (!children_[i]->accept(visitor))
            return false;
    }
    //子节点遍历完成后，进行后访问请求和访问动作
    return visitor.postVisit(*this);
}

void SgTransformNode::addChild(shared_ptr<SgNode> child) {
    children_.push_back(child);
}

void SgTransformNode::removeChild(shared_ptr<SgNode> child) {
    children_.erase(find(children_.begin(), children_.end(), child));
}

//in general,shapeNode doesn't have a child node
bool SgShapeNode::accept(SgNodeVisitor& visitor) {
    if (!visitor.visit(*this))
        return false;
    //shapeNode没有子节点，直接进行后访问postVisit
    return visitor.postVisit(*this);
}

class RbtAccumVisitor : public SgNodeVisitor {
protected:
    vector<RigTForm> rbtStack_;
    SgTransformNode& target_;
    bool found_;
public:
    RbtAccumVisitor(SgTransformNode& target)
    : target_(target)
    , found_(false) {}
    
    const RigTForm getAccumulatedRbt(int offsetFromStackTop = 0) {
        RigTForm accumulatedRbt;
        long stackSize = rbtStack_.size();
        //g_world is just a root node and it is not a rbtnode,so we dont need to explicitly exclude g_world(source) node
        if((stackSize - offsetFromStackTop)>=1 ){
            for(auto start=begin(rbtStack_) , last = end(rbtStack_) - offsetFromStackTop ; start!=last; ++start ){
                accumulatedRbt = accumulatedRbt * (*start) ;
            }
        }
        
        return accumulatedRbt;
    }
    
    virtual bool visit(SgTransformNode& node) {
        //      shared_ptr<SgRbtNode> rbtNode = dynamic_pointer_cast<SgRbtNode>(node.shared_from_this());
        //      if(rbtNode!=NULL){
        //          rbtStack_.push_back(rbtNode->getRbt());
        //      }
        rbtStack_.push_back(node.getRbt());
        if(node == target_)
            return false;
        else
            return true;
    }
    
    virtual bool postVisit(SgTransformNode& node) {
        //rbtStack_.pop_back();
        if(node == target_){
            return false;
        }else{
            rbtStack_.pop_back();
            return true;
        }
    }
};

RigTForm getPathAccumRbt(
                         shared_ptr<SgTransformNode> source,
                         shared_ptr<SgTransformNode> destination,
                         int offsetFromDestination) {
    
    RbtAccumVisitor accum(*destination);
    source->accept(accum);
    return accum.getAccumulatedRbt(offsetFromDestination);
}
