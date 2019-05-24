#ifndef MATERIAL_H
#define MATERIAL_H

#include <cstddef>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#if __GNUG__
#   include <tr1/memory>
#endif


#include "cvec.h"
#include "matrix4.h"
#include "glsupport.h"
#include "uniforms.h"
#include "renderstates.h"
#include "geometry.h"

struct GlProgramDesc;

class Material {
public:
  Material(const std::string& vsFilename, const std::string& fsFilename);

  void draw(Geometry& geometry, const Uniforms& extraUniforms);

  Uniforms& getUniforms() { return uniforms_; }
  const Uniforms& getUniforms() const { return uniforms_; }

  RenderStates& getRenderStates() { return renderStates_; }
  const RenderStates& getRenderStates() const { return renderStates_; }

protected:
  std::tr1::shared_ptr<GlProgramDesc> programDesc_;

  Uniforms uniforms_;

  RenderStates renderStates_;
};


#endif
