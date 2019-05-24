#include <stdexcept>
#include <string>
#include <cstddef>

#include "geometry.h"

using namespace std;

//基础知识点--struct中const常量定义的存在范围？ const在cpp中的定义和初始化分离方式？
//对头文件中的const FORMAT变量进行初始化赋值，并且这个常量的范围存在于对应的定义范围
const VertexFormat VertexPN::FORMAT = VertexFormat(sizeof(VertexPN))
                                      .put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPN, p))
                                      .put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPN, n));

const VertexFormat VertexPNX::FORMAT = VertexFormat(sizeof(VertexPNX))
                                       .put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, p))
                                       .put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, n))
                                       .put("aTexCoord", 2, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, x));

const VertexFormat VertexPNTBX::FORMAT = VertexFormat(sizeof(VertexPNTBX))
                                         .put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, p))
                                         .put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, n))
                                         .put("aTangent", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNTBX, t))
                                         .put("aBinormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNTBX, b))
                                         .put("aTexCoord", 2, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, x));


BufferObjectGeometry::BufferObjectGeometry()
  : wiringChanged_(true),
  primitiveType_(GL_TRIANGLES)
{}

BufferObjectGeometry& BufferObjectGeometry::wire(
  const string& targetAttribName,
  shared_ptr<FormattedVbo> source,
  const string& sourceAttribName) {
  wiringChanged_ = true;
  wiring_[targetAttribName] = make_pair(source, sourceAttribName);
  return *this;
}

BufferObjectGeometry& BufferObjectGeometry::wire(shared_ptr<FormattedVbo> source, const string& sourceAttribName) {
  return wire(sourceAttribName, source, sourceAttribName);
}

BufferObjectGeometry& BufferObjectGeometry::wire(shared_ptr<FormattedVbo> source) {
  const VertexFormat& vfd = source->getVertexFormat();
  for (int i = 0, n = vfd.getNumAttribs(); i < n; ++i) {
    wire(source, vfd.getAttrib(i).name);
  }
  return *this;
}

BufferObjectGeometry& BufferObjectGeometry::indexedBy(shared_ptr<FormattedIbo> ib) {
  ib_ = ib;
  return *this;
}

BufferObjectGeometry& BufferObjectGeometry::primitiveType(GLenum primitiveType) {
  switch (primitiveType) {
  case GL_POINTS:
  case GL_LINE_STRIP:
  case GL_LINE_LOOP:
  case GL_LINES:
  case GL_TRIANGLE_STRIP:
  case GL_TRIANGLE_FAN:
  case GL_TRIANGLES:
  case GL_QUAD_STRIP:
  case GL_QUADS:
  case GL_POLYGON:
    break;
  default:
    assert(0);
  }
  primitiveType_ = primitiveType;
  return *this;
}

const vector<string>& BufferObjectGeometry::getVertexAttribNames() {
  if (wiringChanged_)
    processWiring();
  return vertexAttribNames_;
}

void BufferObjectGeometry::draw(int attribIndices[]) {
  if (wiringChanged_)
    processWiring();

  const unsigned int UNDEFINED_VB_LEN = 0xFFFFFFFF;
  unsigned int vboLen = UNDEFINED_VB_LEN;

  // bind the vertex buffer and set vertex attribute pointers
  for (int i = 0, n = perVbWirings_.size(); i < n; ++i) {
    const PerVbWiring &pvw = perVbWirings_[i];
    const VertexFormat& vfd = pvw.vb->getVertexFormat();

    glBindBuffer(GL_ARRAY_BUFFER, *(pvw.vb));

    vboLen = min(vboLen, (unsigned int)pvw.vb->length());

    for (size_t j = 0; j < pvw.vb2GeoIdx.size(); ++j) {
      int loc = attribIndices[pvw.vb2GeoIdx[j].second];
      if (loc >= 0)
        vfd.setGlVertexAttribPointer(pvw.vb2GeoIdx[j].first, loc);
    }
  }

  if (isIndexed()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ib_);
    glDrawElements(primitiveType_, ib_->length(), ib_->getIndexFormat(), 0);
  }
  else if (vboLen != UNDEFINED_VB_LEN) {
    glDrawArrays(primitiveType_, 0, vboLen);
  }
}

void BufferObjectGeometry::processWiring() {
  perVbWirings_.clear();
  vertexAttribNames_.clear();

  // maps from target vbo to index within perVbWiring_
  map<shared_ptr<FormattedVbo>, int> vbIdx;

  // go through all wiring definitions
  for (Wiring::const_iterator i = wiring_.begin(), e = wiring_.end(); i != e; ++i) {
    shared_ptr<FormattedVbo> vb = i->second.first; // target vbo
    const VertexFormat& vfd = vb->getVertexFormat(); // target vbo's format

    // see if this vbo is already in our vbIdx map
    map<shared_ptr<FormattedVbo>, int>::iterator j = vbIdx.find(vb);
    int idx = 0; // idx of vbo in perVbWiring_, to be set
    if (j == vbIdx.end()) {
      idx = perVbWirings_.size();
      vbIdx[vb] = idx;
      perVbWirings_.push_back(PerVbWiring(vb.get()));
    }
    else {
      idx = j->second;
    }
    const int globalIdx = vertexAttribNames_.size(); // idx within vertexAttribNames_

    perVbWirings_[idx].vb2GeoIdx.push_back(make_pair(vfd.getAttribIndexForName(i->second.second), globalIdx));

    vertexAttribNames_.push_back(i->first);
  }
  wiringChanged_ = false;
}
