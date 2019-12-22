#ifndef MESH_H
#define MESH_H

#include <fstream>
#include <vector>
#include <map>
#include <utility>

#include "cvec.h"

class Mesh {
  typedef int vertex_index;
  typedef int edge_index;
  typedef int face_index;

  struct face_t {
    Cvec <int, 4> vertex_;                                // this will be either a tri or a quad (face_t::vertex[3] == -1  => this is a tri)
    Cvec <int, 4> edge_;
  };
  struct vertex_t {
    Cvec3 position_;
    Cvec3 normal_;
    int halfedge_;
  };
  struct edge_t {
    Cvec <int, 2> halfedge_;
  };

  std::vector <face_t> face_;
  std::vector <vertex_t> vertex_;
  std::vector <edge_t> edge_;

  std::vector <Cvec3> f_;
  std::vector <Cvec3> e_;
  std::vector <Cvec3> v_;

  bool not_manifold_;
  bool with_boundary_;

  int fn__(const int i) const {
    return face_[i].vertex_[3] == -1 ? 3 : 4;
  }
  void init_topology__() {
    std::map <std::pair <int, int>, Cvec <int, 2> > E;
    for (std::size_t i = 0; i < face_.size(); ++i) {
      const int n = fn__(i);
      for (int j = 0; j < n; ++j) {
        const int vj = i | (j<<28);
        const int k = (j+1) % n;
        std::pair <int, int> e(face_[i].vertex_[j], face_[i].vertex_[k]);
        if (e.first < e.second) {
          const int t = e.first;
          e.first = e.second;
          e.second = t;
        }
        if (E.find(e) == E.end()) {
          E[e] = Cvec <int, 2> (vj, -1);
        }
        else {
          Cvec <int, 2>& v = E[e];
          if (v[1] != -1)
            not_manifold_ = true;

          v[1] = vj;
        }
      }
    }
    edge_.resize(E.size());
    int e = 0;
    for (std::map <std::pair <int, int>, Cvec <int, 2> >::iterator i = E.begin(); i != E.end(); ++i, ++e) {
      edge_[e].halfedge_ = i->second;
      for (int j = 0; j < 2; ++j) {
        if (i->second[j] != -1)
          face_[i->second[j] & ((1<<28)-1)].edge_[i->second[j] >> 28] = e | (j<<28);
        else
          with_boundary_ = true;
      }
    }
  }
  void resize__() {
    v_.resize(vertex_.size());
    f_.resize(face_.size());
    e_.resize(edge_.size());
  }
  void load__(const char filename[]) {
    using namespace std;

    ifstream f(filename);
    if (!f) {
      throw std::runtime_error(std::string("Cannot open file ") + filename);
    }
    // Sets bits to report IO error using exception
    f.exceptions(ios::eofbit | ios::failbit | ios::badbit);


    int nv, nt, nq;  // number of: vertices, tris, quads
    f >> nv >> nt >> nq;
    vertex_.resize(nv);
    face_.resize(nt+nq);
    for (int i = 0; i < nv; ++i) {
      f >> vertex_[i].position_[0] >> vertex_[i].position_[1] >> vertex_[i].position_[2];
    }
    for (int i = 0; i < nt; ++i) {
      f >> face_[i].vertex_[0] >> face_[i].vertex_[1] >> face_[i].vertex_[2];
      face_[i].vertex_[3] = -1;
    }
    for (int i = 0; i < nq; ++i) {
      f >> face_[nt+i].vertex_[0] >> face_[nt+i].vertex_[1] >> face_[nt+i].vertex_[2] >> face_[nt+i].vertex_[3];
    }
    for (int i = 0; i < nt; ++i) {
      for (int j = 0; j < 3; ++j) {
        vertex_[face_[i].vertex_[j]].halfedge_ = i | (j<<28);
      }
    }
    for (int i = 0; i < nq; ++i) {
      for (int j = 0; j < 4; ++j) {
        vertex_[face_[nt+i].vertex_[j]].halfedge_ = i | (j<<28);
      }
    }
    init_topology__();
    resize__();
    Cvec3 center(0);
    for (std::size_t i = 0; i < vertex_.size(); ++i) {
      center += vertex_[i].position_;
    }
    center /= vertex_.size();
    for (std::size_t i = 0; i < vertex_.size(); ++i) {
      vertex_[i].position_ -= center;
    }
    double rms = 0;
    for (std::size_t i = 0; i < vertex_.size(); ++i) {
      rms += dot(vertex_[i].position_, vertex_[i].position_);
    }
    rms = std::sqrt(rms / vertex_.size());
    for (std::size_t i = 0; i < vertex_.size(); ++i) {
      vertex_[i].position_ *= 1/rms;
    }
    for (std::size_t i = 0; i < vertex_.size(); ++i) {
      vertex_[i].normal_[0] = -5e37;
    }
  }
  void subdivide__() {
    if (not_manifold_)
      throw std::runtime_error("Subdivision does not support non manifold mesh yet.");
    if (with_boundary_)
      throw std::runtime_error("Subdivision does not support mesh with boundaries yet.");
    std::vector <face_t> f;
    std::vector <vertex_t> v;
    std::vector <edge_t> e;
    std::vector <int> findex;
    v.resize(v_.size() + e_.size() + f_.size());
    e.resize(4*edge_.size());
    f.resize(2*edge_.size());
    findex.resize(face_.size());
    for (std::size_t i = 0; i < v_.size(); ++i) {
      v[i].position_ = v_[i];                                                     // v-vertices
    }
    for (std::size_t i = 0; i < e_.size(); ++i) {
      v[i+v_.size()].position_ = e_[i];                                               // e-vertices
    }
    for (std::size_t i = 0; i < f_.size(); ++i) {
      v[i+v_.size()+e_.size()].position_ = f_[i];                                           // f-vertices
    }
    int fi = 0;
#ifndef NDEBUG
    for (std::size_t i = 0; i < v.size(); ++i) {
      v[i].halfedge_ = -1;
    }
#endif
    for (std::size_t i = 0; i < face_.size(); ++i) {
      findex[i] = fi;
      const int n = fn__(i);
      for (int j = 0; j < n; ++j, ++fi) {
        const int k = (j+n-1) % n;
        const int ej = face_[i].edge_[j] & ((1<<28)-1);
        const int ek = face_[i].edge_[k] & ((1<<28)-1);
        f[fi].vertex_[0] = face_[i].vertex_[j];                     // the v-vertex
        f[fi].vertex_[1] = v_.size() + ej;
        f[fi].vertex_[2] = v_.size() + e_.size() + i;                 // the f-vertex
        f[fi].vertex_[3] = v_.size() + ek;
        v[f[fi].vertex_[0]].halfedge_ = fi | (0 << 28);
        v[f[fi].vertex_[1]].halfedge_ = fi | (1 << 28);
        v[f[fi].vertex_[2]].halfedge_ = fi | (2 << 28);
        v[f[fi].vertex_[3]].halfedge_ = fi | (3 << 28);
      }
    }
    for (std::size_t i = 0; i < edge_.size(); ++i) {
      const int f0 = edge_[i].halfedge_[0] & ((1<<28)-1);
      const int f1 = edge_[i].halfedge_[1] & ((1<<28)-1);
      const int j0 = edge_[i].halfedge_[0] >> 28;
      const int j1 = edge_[i].halfedge_[1] >> 28;
      const int n0 = fn__(f0);
      const int n1 = fn__(f1);
      const int k0 = (j0+1) % n0;
      const int k1 = (j1+1) % n1;
      e[4*i + 0].halfedge_[0] = (findex[f0] + j0) | (0 << 28);
      e[4*i + 0].halfedge_[1] = (findex[f1] + k1) | (3 << 28);
      e[4*i + 1].halfedge_[0] = (findex[f0] + j0) | (1 << 28);
      e[4*i + 1].halfedge_[1] = (findex[f0] + k0) | (2 << 28);
      e[4*i + 2].halfedge_[0] = (findex[f1] + j1) | (0 << 28);
      e[4*i + 2].halfedge_[1] = (findex[f0] + k0) | (3 << 28);
      e[4*i + 3].halfedge_[0] = (findex[f1] + j1) | (1 << 28);
      e[4*i + 3].halfedge_[1] = (findex[f1] + k1) | (2 << 28);
      for (std::size_t j = 4*i; j < 4*i+4; ++j) {
        for (int k = 0; k < 2; ++k) {
          f[e[j].halfedge_[k] & ((1<<28)-1)].edge_[e[j].halfedge_[k] >> 28] = j | (k<<28);
        }
      }
    }
#ifndef NDEBUG
    for (std::size_t i = 0; i < v.size(); ++i) {
      assert(v[i].halfedge_ != -1);
    }
#endif
    vertex_.swap(v);
    edge_.swap(e);
    face_.swap(f);
    resize__();
  }

public:
  struct VertexIterator;                                    // forward declaration (needed by Vertex class)

  // Default contructor. Assignment operator/constructor
  Mesh() : not_manifold_(false), with_boundary_(false) {}
  Mesh(const Mesh& m) {
    *this = m;
  }
  Mesh& operator = (const Mesh& m) {
    face_ = m.face_;
    vertex_ = m.vertex_;
    edge_ = m.edge_;
    f_ = m.f_;
    e_ = m.e_;
    v_ = m.v_;
    not_manifold_ = m.not_manifold_;
    with_boundary_ = m.with_boundary_;
    return *this;
  }

  // Mesh::Vertex class
  struct Vertex {
    Mesh& m_;
    const int v_;

    Vertex(Mesh& m, const int v) : m_(m), v_(v)                 {}
    Cvec3 getPosition() const {
      return m_.vertex_[v_].position_;
    }
    Cvec3 getNormal() const {
      assert(m_.vertex_[v_].normal_[0] > -1e37 || !"Error: This normal is uninitialized, you can set it with setNormal()");
      return m_.vertex_[v_].normal_;
    }
    void setPosition(const Cvec3& p) const {
      m_.vertex_[v_].position_ = p;
    }
    void setNormal(const Cvec3& n) const {
      m_.vertex_[v_].normal_ = n;
    }
    int getIndex() const {
      return v_;
    }
    VertexIterator getIterator() const {
      assert((m_.vertex_[v_].halfedge_&((1<<28)-1)) < (int)m_.face_.size());
      return VertexIterator(m_, m_.vertex_[v_].halfedge_);
    }
  };

  // Mesh::Face class
  struct Face {
    Mesh& m_;
    const int f_;

    Face(Mesh& m, const int f) : m_(m), f_(f)                 {}
    int getNumVertices() const {
      return m_.fn__(f_);
    }
    Cvec3 getNormal() const {
      return cross(m_.vertex_[m_.face_[f_].vertex_[1]].position_ - m_.vertex_[m_.face_[f_].vertex_[0]].position_,
                   m_.vertex_[m_.face_[f_].vertex_[2]].position_ - m_.vertex_[m_.face_[f_].vertex_[0]].position_).normalize();
    }
    Vertex getVertex(const int i) const {
      assert(i >= 0 && i < getNumVertices());
      return Vertex(m_, m_.face_[f_].vertex_[i]);
    }

  };

  // Mesh::Edge class
  struct Edge {
    Mesh& m_;
    const int e_;

    Edge(Mesh& m, const int e) : m_(m), e_(e)                 {}
    Vertex getVertex(const int i) const {
      assert(i >= 0 && i < 2);
      int faceIdx = m_.edge_[e_].halfedge_[0] & ((1<<28)-1);
      int vertIdxWithinFace = ((m_.edge_[e_].halfedge_[0] >> 28) + i) % 4;
      if (m_.face_[faceIdx].vertex_[vertIdxWithinFace] == -1) {
        assert(vertIdxWithinFace == 3);
        vertIdxWithinFace = 0;
      }
      return Vertex(m_, m_.face_[faceIdx].vertex_[vertIdxWithinFace]);
    }
    Face getFace(const int i) const {
      assert(i >= 0 && i < 2);
      return Face(m_, m_.edge_[e_].halfedge_[i] & ((1<<28)-1));
    }
    bool is_valid() const {
      return getVertex(0).v_ != -1 && getVertex(1).v_ != -1;
    }
  };

  // Mesh::VertexIterator
  struct VertexIterator {
    Mesh& m_;
    int h_;

    VertexIterator(Mesh& m, const int h) : m_(m), h_(h)             {}
    Vertex getVertex() const {
      const int v(h_ >> 28);
      const int f(h_ & ((1<<28)-1));
      return Vertex(m_, m_.face_[f].vertex_[(v+1) % m_.fn__(f)]);
    }
    Face getFace() const {
      return Face(m_, h_ & ((1<<28)-1));
    }
    VertexIterator& operator ++ () {
      const int f(h_ & ((1<<28)-1)), v(h_ >> 28), vj((v+m_.fn__(f)-1) % m_.fn__(f)), e(m_.face_[f].edge_[vj] & ((1<<28)-1)), ei(m_.face_[f].edge_[vj] >> 28);
      h_ = m_.edge_[e].halfedge_[ei ^ 1];
      return *this;
    }
    bool operator == (const VertexIterator& vi) const {
      return &m_ == &vi.m_ && h_ == vi.h_;
    }
    bool operator != (const VertexIterator& vi) const {
      return &m_ != &vi.m_ || h_ != vi.h_;
    }
  };

  int getNumFaces() const {
    return face_.size();
  }
  int getNumEdges() const {
    return edge_.size();
  }
  int getNumVertices() const {
    return vertex_.size();
  }

  Vertex getVertex(const int i) {
    return Vertex(*this, i);
  }
  Edge getEdge(const int i) {
    return Edge(*this, i);
  }
  Face getFace(const int i) {
    return Face(*this, i);
  }

  Cvec3 getNewFaceVertex(const Face& f) const {
    return f_[f.f_];
  }
  Cvec3 getNewEdgeVertex(const Edge& e) const {
    return e_[e.e_];
  }
  Cvec3 getNewVertexVertex(const Vertex& v) const {
    return v_[v.v_];
  }

  void setNewFaceVertex(const Face& f, const Cvec3& p) {
    f_[f.f_] = p;
  }
  void setNewEdgeVertex(const Edge& e, const Cvec3& p) {
    e_[e.e_] = p;
  }
  void setNewVertexVertex(const Vertex& v, const Cvec3& p) {
    v_[v.v_] = p;
  }

  void subdivide() {
    subdivide__();
  }
  void load(const char filename[]) {
    load__(filename);
  }
};



#endif
