#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "glsupport.h"

using namespace std;

//void checkGlErrors() {
//  const GLenum errCode = glGetError();
//
//  if (errCode != GL_NO_ERROR) {
//    string error("GL Error: ");
//    //gluErrorString(errCode)
//    error += reinterpret_cast<const char*>(errCode+"");
//    cerr << error << endl;
//    throw runtime_error(error);
//  }
//}

void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error= glGetError()) {
        printf("after %s() glError (0x%x)\n", op, error);
    }
}

// Dump text file into a character vector, throws exception on error
static void readTextFile(const char *fn, vector<char>& data) {
  // Sets ios::binary bit to prevent end of line translation, so that the
  // number of bytes we read equals file size
  ifstream ifs(fn, ios::binary);
  if (!ifs)
    throw runtime_error(string("Cannot open file ") + fn);

  // Sets bits to report IO error using exception
  ifs.exceptions(ios::eofbit | ios::failbit | ios::badbit);
  ifs.seekg(0, ios::end);
  size_t len = ifs.tellg();

  data.resize(len);

  ifs.seekg(0, ios::beg);
  ifs.read(&data[0], len);
}



// Print info regarding an GL object
static void printInfoLog(GLuint obj, const string& filename) {
  GLint infologLength = 0;
  GLint charsWritten  = 0;
//#if TARGET_OS_OSX
//    glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
//    if (infologLength > 0) {
//        string infoLog(infologLength, ' ');
//        glGetInfoLogARB(obj, infologLength, &charsWritten, &infoLog[0]);
//        std::cerr << "##### Log [" << filename << "]:\n" << infoLog << endl;
//    }
//#else
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength) {
        string infoLog(infologLength, ' ');
        glGetShaderInfoLog(obj, infologLength, &charsWritten, &infoLog[0]);
        std::cerr << "##### Log [" << filename << "]:\n" << infoLog << endl;
    }
//#endif
}

void readAndCompileSingleShader(GLuint shaderHandle, const char *fn) {
  vector<char> source;
  readTextFile(fn, source);

  const char *ptrs[] = {&source[0]};
  const GLint lens[] = {static_cast<GLint>(source.size())};
  glShaderSource(shaderHandle, 1, ptrs, lens);   // load the shader sources

  glCompileShader(shaderHandle);

  printInfoLog(shaderHandle, fn);

  GLint compiled = 0;
  glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
  if (!compiled)
    throw runtime_error("fails to compile GL shader");
}

void linkShader(GLuint programHandle, GLuint vs, GLuint fs) {
  glAttachShader(programHandle, vs);
  glAttachShader(programHandle, fs);

  glLinkProgram(programHandle);

  glDetachShader(programHandle, vs);
  glDetachShader(programHandle, fs);

  GLint linked = 0;
  glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
  printInfoLog(programHandle, "linking");

  if (!linked)
    throw runtime_error("fails to link shaders");
}


void readAndCompileShader(GLuint programHandle, const char * vertexShaderFileName, const char * fragmentShaderFileName) {
  GlShader vs(GL_VERTEX_SHADER);
  GlShader fs(GL_FRAGMENT_SHADER);

  readAndCompileSingleShader(vs, vertexShaderFileName);
  readAndCompileSingleShader(fs, fragmentShaderFileName);

  linkShader(programHandle, vs, fs);
}

void loadAndCompileSingleShader(GLuint shaderHandle, const char* shaderSource) {
    if(shaderHandle){
        glShaderSource(shaderHandle, 1, &shaderSource, NULL); // load the shader sources
        
        glCompileShader(shaderHandle);
        
        printInfoLog(shaderHandle, shaderSource);
        
        GLint compiled = 0;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
            throw runtime_error("fails to compile GL shader");
    }
}

void loadAndCompileShader(GLuint programHandle,const char * vertexShaderSource, const char * fragmentShaderSource){
    GlShader vs(GL_VERTEX_SHADER);
    GlShader fs(GL_FRAGMENT_SHADER);
    
    loadAndCompileSingleShader(vs, vertexShaderSource);
    loadAndCompileSingleShader(fs, vertexShaderSource);
    
    linkShader(programHandle, vs, fs);
}
