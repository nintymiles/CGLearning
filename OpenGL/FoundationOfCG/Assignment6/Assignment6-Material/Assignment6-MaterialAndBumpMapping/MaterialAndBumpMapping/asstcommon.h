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
    // send MVM
    uniforms.put("uModelViewMatrix",MVM);
    
    // send NMVM
    uniforms.put("uNormalMatrix",NMVM);
}

//// takes a projection matrix and send to the the shaders
//static void sendProjectionMatrix(const ShaderState& curSS, const Matrix4& projMatrix) {
//    GLfloat glmatrix[16];
//    projMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
//    safe_glUniformMatrix4fv(curSS.h_uProjMatrix, glmatrix);
//}

//======================================================================
// STEP 4: Changes the definition of sendProjectionMatrix to use Uniforms
//         instead of ShaderState
//=======================================================================

// takes a projection matrix and send to the the shaders
inline void sendProjectionMatrix(Uniforms& uniforms, const Matrix4& projMatrix) {
    uniforms.put("uProjMatrix", projMatrix);
}

#endif
