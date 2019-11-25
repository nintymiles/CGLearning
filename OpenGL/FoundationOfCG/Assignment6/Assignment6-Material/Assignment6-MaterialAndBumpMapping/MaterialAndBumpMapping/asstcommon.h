#ifndef ASSTCOMMON_H
#define ASSTCOMMON_H

#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>

#include "glsupport.h"
#include "matrix4.h"
#include "uniforms.h"

#define DEPRECATED

DEPRECATED struct ShaderState {
  GlProgram program;

  // Handles to uniform variables
  GLint h_uLight, h_uLight2;
  GLint h_uProjMatrix;
  GLint h_uModelViewMatrix;
  GLint h_uNormalMatrix;
  GLint h_uColor;
  GLint h_uIdColor;

  // Handles to vertex attributes
  GLint h_aPosition;
  GLint h_aNormal;

  ShaderState(const char* vsfn, const char* fsfn) {
    readAndCompileShader(program, vsfn, fsfn);

    const GLuint h = program; // short hand

    // Retrieve handles to uniform variables
    h_uLight = safe_glGetUniformLocation(h, "uLight");
    h_uLight2 = safe_glGetUniformLocation(h, "uLight2");
    h_uProjMatrix = safe_glGetUniformLocation(h, "uProjMatrix");
    h_uModelViewMatrix = safe_glGetUniformLocation(h, "uModelViewMatrix");
    h_uNormalMatrix = safe_glGetUniformLocation(h, "uNormalMatrix");
    h_uColor = safe_glGetUniformLocation(h, "uColor");
    h_uIdColor = safe_glGetUniformLocation(h, "uIdColor");

    // Retrieve handles to vertex attributes
    h_aPosition = safe_glGetAttribLocation(h, "aPosition");
    h_aNormal = safe_glGetAttribLocation(h, "aNormal");

    checkGlError(__FUNCTION__);
  }

};

// the responsibility of ShaderState: read and compile shaders,then check and store uniform locations. therefore it is not needed any more for current material system.
// takes MVM and its normal matrix to the shaders
//DEPRECATED inline void sendModelViewNormalMatrix(const ShaderState& curSS, const Matrix4& MVM, const Matrix4& NMVM) {
//  GLfloat glmatrix[16];
//  MVM.writeToColumnMajorMatrix(glmatrix); // send MVM
//  safe_glUniformMatrix4fv(curSS.h_uModelViewMatrix, glmatrix);
//
//  NMVM.writeToColumnMajorMatrix(glmatrix); // send NMVM
//  safe_glUniformMatrix4fv(curSS.h_uNormalMatrix, glmatrix);
//}

// takes MVM and its normal matrix to the shaders by using uniforms
inline void sendModelViewNormalMatrix(Uniforms& uniforms, const Matrix4& MVM, const Matrix4& NMVM) {
//    GLfloat glmatrix[16];
//    MVM.writeToColumnMajorMatrix(glmatrix); // send MVM
    uniforms.put("uModelViewMatrix",MVM);
    
//    NMVM.writeToColumnMajorMatrix(glmatrix); // send NMVM
    uniforms.put("uNormalMatrix",NMVM);
}

#endif
