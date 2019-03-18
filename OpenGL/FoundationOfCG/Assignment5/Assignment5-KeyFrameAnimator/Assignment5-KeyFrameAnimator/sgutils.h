#ifndef SGUTILS_H
#define SGUTILS_H

#include <vector>

#include "scenegraph.h"

using namespace std;

struct RbtNodesScanner : public SgNodeVisitor {
  typedef vector<shared_ptr<SgRbtNode> > SgRbtNodes;

  SgRbtNodes& nodes_;

  RbtNodesScanner(SgRbtNodes& nodes) : nodes_(nodes) {}

  virtual bool visit(SgTransformNode& node) {
    using namespace std;
    shared_ptr<SgRbtNode> rbtPtr = dynamic_pointer_cast<SgRbtNode>(node.shared_from_this());
    if (rbtPtr)
      nodes_.push_back(rbtPtr);  //把指定元素追加到容器末尾
    return true;
  }
};

inline void dumpSgRbtNodes(shared_ptr<SgNode> root, vector<shared_ptr<SgRbtNode>>& rbtNodes) {
  RbtNodesScanner scanner(rbtNodes);
  root->accept(scanner);
}


#endif
