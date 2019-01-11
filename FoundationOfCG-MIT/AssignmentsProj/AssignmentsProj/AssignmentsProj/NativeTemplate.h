//
//  NativeTemplate.h
//  iOSBlueTriangle
//
//  Created by Parminder Singh on 8/29/13.
//  Email: parminder_ce@yahoo.co.in
//  Copyright (c) 2013 macbook. All rights reserved.
//

#ifndef iOSBlueTriangle_NativeTemplate_h
#define iOSBlueTriangle_NativeTemplate_h


//! OpenGL ES 3.0 Cookcook code

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
//! Android Header files for NDK and OpenGLES 3.0

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#define LOGI printf
#define LOGE printf

#endif

//! Load the Shader for compilation and linking
GLuint loadAndCompileShader(GLenum shaderType, const char* sourceCode);

//! Create program object and link program.
GLuint linkShader(GLuint vertShaderID, GLuint fragShaderID);

//! Create vertex and fragment shader executable program.
GLuint createProgramExec(const char* vertexSource, const char* fragmentSource);
    
//! Print information or error message.
void printOpenGLESInfo();

//! Initialize the shader.
bool GraphicsInit();

//! Set the Viewport with provide width and height.
bool GraphicsResize( int width, int height );

//! Render the triangle geometry.
bool GraphicsRender();

void TouchEventDown( float x, float y,unsigned long tapCount,bool pressStatus );

void TouchEventMove( float x, float y );

void TouchEventRelease( float x, float y,unsigned long tapCount,bool pressStatus );

#endif
