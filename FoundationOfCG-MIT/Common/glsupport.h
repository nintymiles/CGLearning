/*****************************************************************************
 *Original source template from book source site based on GLUT
 *Modified to accomdate for current OS setting through GLFW by SeanRen
 *****************************************************************************/
#ifndef GLSUPPORT_H
#define GLSUPPORT_H

#include <iostream>
#include <stdexcept>

//Note TARGET_OS_MAC includes any apple OS,TARGET_OS_IPHONE includes any device/simulator run on iOS
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_OSX   //定位OSX系统的macro
#include <GLFW/glfw3.h>
#elif TARGET_OS_IPHONE //用于定位IPHONE系统的macro
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif
#endif

#define GLSL(version, shader) "#version " #version "\n" #shader

// Check if there has been an error inside OpenGL and if yes, print the error and
// through a runtime_error exception.
//void checkGlErrors();

// call this function after every OpenGL function call
void checkGlError(const char* op);
void printProgramInfoLog(GLuint program);

// Reads and compiles a pair of vertex shader and fragment shader files into a
// GL shader program. Throws runtime_error on error
void readAndCompileShader(GLuint programHandle,
                          const char *vertexShaderFileName, const char *fragmentShaderFileName);

// Link two compiled vertex shader and fragment shader into a GL shader program
void linkShader(GLuint programHandle, GLuint vertexShaderHandle, GLuint fragmentShaderHandle);

// Reads and compiles a single shader (vertex, fragment, etc) file into a GL
// shader. Throws runtime_error on error
void readAndCompileSingleShader(GLuint shaderHandle, const char* shaderFileName);

// load and compile shader from source by created shaderHandle
void loadAndCompileSingleShader(GLuint shaderHandle, const char* shaderSource);

// Loads and compiles a pair of vertex shader and fragment shader sources
void loadAndCompileShader(GLuint programHandle,const char * vertexShaderSource, const char * fragmentShaderSource);



// Classes inheriting Noncopyable will not have default compiler generated copy
// constructor and assignment operator
class Noncopyable {
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:
    //参数名后跟着&符号的含义？代表reference调用参数，reference在函数期间不会被改变，故而一般前置const
    Noncopyable(const Noncopyable&);
    //阻止copy的方式是重载赋值操作赋，让其返回reference类型
    const Noncopyable& operator= (const Noncopyable&);
};

// Light wrapper around a GL shader (can be geometry/vertex/fragment shader)
// handle. Automatically allocates and deallocates. Can be casted to GLuint.
//总体而言，这个封装确实是一种简单的轻量级封装
class GlShader : Noncopyable {
    
protected:
    //GLShader成员变量，用于GLShader对象中GLES shader objectID的存储
    GLuint handle_;
    
public:
    //构造函数，用于生成特定类型的shader
    GlShader(GLenum shaderType) {
        handle_ = glCreateShader(shaderType); // create shader handle
        //对CretaeShader返回值异常的处理 -- 如果无法生成有效shader对象，则直接执行系统运行错误重量级异常
        if (handle_ == 0)
            throw std::runtime_error("glCreateShader fails");
        checkGlError(__FUNCTION__);
    }
    
    //通过destructor释放内存
    ~GlShader() {
        //删除shader对象
        glDeleteShader(handle_);
    }
    
    // Casts to GLuint so can be used directly by glCompile etc
    //通过新的(GLUint)操作符定义返回shader对象的objectID
    operator GLuint() const {
        return handle_;
    }
};

// Light wrapper around GLSL program handle that automatically allocates
// and deallocates. Can be casted to a GLuint.
//阻止GLProgram被copy，原因是GLProgram是一个特殊对象，每个实例对应着唯一的GLES program对象的管理
//借用CPP的对象管理机制实现内存管理
class GlProgram : Noncopyable {
//protected domain中的变量只会被本层次体系中的对象访问
protected:
    GLuint handle_;
    
public:
    GlProgram() {
        handle_ = glCreateProgram();
        if (handle_ == 0)
            throw std::runtime_error("glCreateProgram fails");
        checkGlError(__FUNCTION__);
    }
    
    ~GlProgram() {
        glDeleteProgram(handle_);
    }
    
    // Casts to GLuint so can be used directly by glUseProgram and so on
    operator GLuint() const {
        return handle_;
    }
};


// Light wrapper around a GL texture object handle that automatically allocates
// and deallocates. Can be casted to a GLuint.
//对缓存类的简单封装方式
class GlTexture : Noncopyable {
protected:
    GLuint handle_;
    
public:
    //默认构造函数
    GlTexture() {
        //以GenTextures生成一个textureID，并将textureID赋予handle_内部变量
        glGenTextures(1, &handle_);
        //检查是否有错误生成
        checkGlError(__FUNCTION__);
    }
    
    //destructor函数直接销毁内部变量所对应的texture缓存，封装为对象形式的好处是在可以借助c++机制自动销毁
    ~GlTexture() {
        glDeleteTextures(1, &handle_);
    }
    
    // Casts to GLuint so can be used directly by glBindTexture and so on
    operator GLuint () const {
        return handle_;
    }
};

// Light wrapper around a GL buffer object handle that automatically allocates
// and deallocates. Can be casted to a GLuint.
class GlBufferObject : Noncopyable {
protected:
    GLuint handle_;
    
public:
    GlBufferObject() {
        glGenBuffers(1, &handle_);
        checkGlError(__FUNCTION__);
    }
    
    ~GlBufferObject() {
        glDeleteBuffers(1, &handle_);
    }
    
    // Casts to GLuint so can be used directly glBindBuffer and so on
    operator GLuint() const {
        return handle_;
    }
};


// Safe versions of various functions that handle GLSL shader attributes
// and variables: These mainly issue a warning when specified attributes
// and variables do not exist in the compiled GLSL program (e.g., due to
// driver optimization), and return without doing anything when the user
// tries to change these attributes or variables.

inline GLint safe_glGetUniformLocation(const GLuint program, const char varname[]) {
  GLint r = glGetUniformLocation(program, varname);
  if (r < 0)
    std::cerr << "WARN: "<< varname << " cannot be bound (it either doesn't exist or has been optimized away). safe_glUniform calls will silently ignore it.\n" << std::endl;
  return r;
}

inline GLint safe_glGetAttribLocation(const GLuint program, const char varname[]) {
  GLint r = glGetAttribLocation(program, varname);
  if (r < 0)
    std::cerr << "WARN: "<< varname << " cannot be bound (it either doesn't exist or has been optimized away). safe_glAttrib calls will silently ignore it.\n" << std::endl;
  return r;
}

inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

inline void safe_glUniform1i(const GLint handle, const GLint a) {
  if (handle >= 0)
    glUniform1i(handle, a);
}

inline void safe_glUniform2i(const GLint handle, const GLint a, const GLint b) {
  if (handle >= 0)
    glUniform2i(handle, a, b);
}

inline void safe_glUniform3i(const GLint handle, const GLint a, const GLint b, const GLint c) {
  if (handle >= 0)
    glUniform3i(handle, a, b, c);
}

inline void safe_glUniform4i(const GLint handle, const GLint a, const GLint b, const GLint c, const GLint d) {
  if (handle >= 0)
    glUniform4i(handle, a, b, c, d);
}

inline void safe_glUniform1f(const GLint handle, const GLfloat a) {
  if (handle >= 0)
    glUniform1f(handle, a);
}

inline void safe_glUniform2f(const GLint handle, const GLfloat a, const GLfloat b) {
  if (handle >= 0)
    glUniform2f(handle, a, b);
}

inline void safe_glUniform3f(const GLint handle, const GLfloat a, const GLfloat b, const GLfloat c) {
  if (handle >= 0)
    glUniform3f(handle, a, b, c);
}

inline void safe_glUniform4f(const GLint handle, const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d) {
  if (handle >= 0)
    glUniform4f(handle, a, b, c, d);
}

inline void safe_glEnableVertexAttribArray(const GLint handle) {
  if (handle >= 0)
    glEnableVertexAttribArray(handle);
}

inline void safe_glDisableVertexAttribArray(const GLint handle) {
  if (handle >= 0)
    glDisableVertexAttribArray(handle);
}

inline void safe_glVertexAttribPointer(const GLint handle, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
  if (handle >= 0)
    glVertexAttribPointer(handle, size, type, normalized, stride, pointer);
}

inline void safe_glVertexAttrib1f(const GLint handle, const GLfloat a) {
  if (handle >= 0)
    glVertexAttrib1f(handle, a);
}

inline void safe_glVertexAttrib2f(const GLint handle, const GLfloat a, const GLfloat b) {
  if (handle >= 0)
    glVertexAttrib2f(handle, a, b);
}

inline void safe_glVertexAttrib3f(const GLint handle, const GLfloat a, const GLfloat b, const GLfloat c) {
  if (handle >= 0)
    glVertexAttrib3f(handle, a, b, c);
}

inline void safe_glVertexAttrib4f(const GLint handle, const GLfloat a, const GLfloat b, const GLfloat c, const GLfloat d) {
  if (handle >= 0)
    glVertexAttrib4f(handle, a, b, c, d);
}

#if TARGET_OS_OSX
inline void safe_glVertexAttrib4Nub(const GLint handle, const GLubyte a, const GLubyte b, const GLubyte c, const GLubyte d) {
  if (handle >= 0)
    glVertexAttrib4Nub(handle, a, b, c, d);
}
#endif


#endif
