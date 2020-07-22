#include <algorithm>

#include "scenegraph.h"

using namespace std;

//每个accept方法，对应着visitor方法的visit和postVisit方法，节点和子节点都会都会被这些visitor的
//对应方法访问。
//在父子体系中，从父节点到单一的叶子节点的路径中，每个节点首先被visit方法访问，然后再依次
//从叶子节点返回到父亲节点执行postVisit方法。
//visit和postVisit方法只要有方法返回false则accept方法结束执行，返回false。
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

//RbtAccumVisitor会一直迭代访问TransformNode节点，首先执行visit函数。
//直到最终找到对应子节点，此时visit函数return false（意味着压栈完毕）；
//否则一直执行到子节点遍历完毕，最后开始从子节点到父级节点反向执行postVisit方法。
//对当前的stack依次执行pop动作直到出栈完毕。
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
