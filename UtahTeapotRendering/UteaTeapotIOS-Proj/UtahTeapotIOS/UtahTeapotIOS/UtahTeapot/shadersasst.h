#ifndef ASSTCOMMON_H
#define ASSTCOMMON_H

#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>

#include "glsupport.h"
#include "matrix4.h"
#include "FileWrapper.h"

struct ShaderState {
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

// takes MVM and its normal matrix to the shaders
inline void sendModelViewNormalMatrix(const ShaderState& curSS, const Matrix4& MVM, const Matrix4& NMVM) {
  GLfloat glmatrix[16];
  MVM.writeToColumnMajorMatrix(glmatrix); // send MVM
  safe_glUniformMatrix4fv(curSS.h_uModelViewMatrix, glmatrix);

  NMVM.writeToColumnMajorMatrix(glmatrix); // send NMVM
  safe_glUniformMatrix4fv(curSS.h_uNormalMatrix, glmatrix);
}

// takes a projection matrix and send to the the shaders
static void sendProjectionMatrix(const ShaderState& curSS, const Matrix4& projMatrix) {
    GLfloat glmatrix[16];
    projMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
    safe_glUniformMatrix4fv(curSS.h_uProjMatrix, glmatrix);
}


struct FrustumShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    
    FrustumShaderState() {
        static std::string vsfn = "SimpleVertex.glsl";
        static std::string fsfn = "SimpleFragment.glsl";
        
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        
        checkGlError(__FUNCTION__);
    }
    
};

struct SimpleTextureShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    
    SimpleTextureShaderState() {
        static std::string vsfn = "SimpleTextureVertex.glsl";
        static std::string fsfn = "SimpleTextureFragment.glsl";
        
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        
        checkGlError(__FUNCTION__);
    }
    
};

struct TeapotShaderState{
    
    GlProgram program;
    
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;
    
    GLuint matrix_projection_;
    GLuint matrix_view_;
    
    TeapotShaderState() {
        static std::string vsfn =  "PhongShader.vsh";
        static std::string fsfn =  "PhongShader.fsh";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_projection_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        matrix_view_ = safe_glGetUniformLocation(program, "uMVMatrix");
        
        light0_ = safe_glGetUniformLocation(program, "vLight0");
        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
        material_specular_ =
        safe_glGetUniformLocation(program, "vMaterialSpecular");
        
        checkGlError(__FUNCTION__);
    }
    
};

struct MoreTeapotsShaderState{
    
    GlProgram program;
    
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;
    
    GLuint matrix_projection_;
    GLuint matrix_view_;
    
    MoreTeapotsShaderState(std::map<std::string, std::string> &map_parameters) {
        static std::string vsfn = "InstancedPhongShader.vsh";
        static std::string fsfn = "InstancedPhongShader.fsh";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()), map_parameters);
        
        // Retrieve handles to uniform variables
        matrix_projection_ = safe_glGetUniformLocation(program, "uPMatrix");
        matrix_view_ = safe_glGetUniformLocation(program, "uMVMatrix");
        
        light0_ = safe_glGetUniformLocation(program, "vLight0");
        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
        material_specular_ =
        safe_glGetUniformLocation(program, "vMaterialSpecular");
        
        checkGlError(__FUNCTION__);
    }
    
};

//struct TexturedTeapotShaderState{
//
//    GlProgram program;
//
//    GLuint light0_;
//    GLuint material_diffuse_;
//    GLuint material_ambient_;
//    GLuint material_specular_;
//
//    GLuint matrix_projection_;
//    GLuint matrix_view_;
//
//    TexturedTeapotShaderState() {
////        static std::string vsfn = "Cubemap.vsh";
////        static std::string fsfn = "Cubemap.fsh";
//        static std::string vsfn = "PhongShader.vsh";
//        static std::string fsfn = "PhongShader.fsh";
//
//        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
//
//        // Retrieve handles to uniform variables
//        matrix_projection_ = safe_glGetUniformLocation(program, "uMVPMatrix");
//        matrix_view_ = safe_glGetUniformLocation(program, "uMVMatrix");
//
//        light0_ = safe_glGetUniformLocation(program, "vLight0");
//        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
//        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
//        material_specular_ =
//        safe_glGetUniformLocation(program, "vMaterialSpecular");
//
//        checkGlError(__FUNCTION__);
//    }
//
//};

struct TexturedTeapotShaderState{
    
    GlProgram program;
    
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;
    
    GLuint matrix_projection_;
    GLuint matrix_view_;
    GLuint matrix_model_;
    
    TexturedTeapotShaderState() {
        //        static std::string vsfn = "./shaders/Cubemap.vsh";
        //        static std::string fsfn = "./shaders/Cubemap.fsh";
        static std::string vsfn = "EyeFrameCubemap.vsh";
        static std::string fsfn = "EyeFrameCubemap.fsh";
//        static std::string vsfn = "ShaderPlain.vsh";
//        static std::string fsfn = "ShaderPlain.fsh";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_projection_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        matrix_view_ = safe_glGetUniformLocation(program, "uMVMatrix");
        matrix_model_ = safe_glGetUniformLocation(program, "uMMatrix");
        
        light0_ = safe_glGetUniformLocation(program, "vLight0");
        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
        material_specular_ =
        safe_glGetUniformLocation(program, "vMaterialSpecular");
        
        checkGlError(__FUNCTION__);
    }
    
};


#endif
