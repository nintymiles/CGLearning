#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "glsupport.h"

// Currently the following GL states are supported:
//
// - glPolygonMode  (Default: GL_FRONT_AND_BACK, GL_FILL)
// - glBlendFunc    (Default: GL_ONE, GL_ZERO)
// - glCullFace     (Default: GL_BACK)
//
// The following flags for glEnable/glDisable are supported
//
// - GL_BLEND       (Default: off)
// - GL_CULL_FACE   (Default: on)

class RenderStates {
  GLenum glFront, glBack; // for polygonMode
  GLenum glBlendSrcFactor, glBlendDstFactor; // for blendFunc
  GLenum glCullFaceMode; // for cullFace
  unsigned int flags;

public:
  RenderStates();

  RenderStates& polygonMode(GLenum face, GLenum mode);
  RenderStates& blendFunc(GLenum sfactor, GLenum dfactor);
  RenderStates& cullFace(GLenum mode);

  RenderStates& enable(GLenum target);
  RenderStates& disable(GLenum target);

  void apply() const;
  void captureFromGl();
};

#endif