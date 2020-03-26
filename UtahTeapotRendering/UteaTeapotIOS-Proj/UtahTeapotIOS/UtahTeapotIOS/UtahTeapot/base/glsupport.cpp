#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <map>

#include "glsupport.h"

using namespace std;


Cvec3 hexStringToRGB(string hexString){
    int num = stoi(hexString,NULL,16);
    int red = num>>16&0xFF;
    int green = num>>8&0xFF;
    int blue = num&0xFF;
    return Cvec3(red/255.0,green/255.0,blue/255.0);
}

//此函数原来借助glu对GLError的封装，但是在GLES环境中，目前不借助第三方utility library。
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
    //将GL error stack中的error逐一取出，直至stack中没有error
    for (GLint error = glGetError(); error; error= glGetError()) {
        printf("Error Occurs, %s() glError (0x%x)\n", op, error);
    }
}

//将文本文件读入一个字符vector结构中，遇到错误抛出异常
// Dump text file into a character vector, throws exception on error
static void readTextFile(const char *fn, vector<char>& data) {
    // Sets ios::binary bit to prevent end of line translation, so that the
    // number of bytes we read equals file size
    
    //使用CPP ifstream打开文件，并使用ios中定义的ios状态来控制文件操作
    ifstream ifs(fn, ios::binary);
    if (!ifs)
        throw runtime_error(string("Cannot open file ") + fn);  //c++中，string类封装了针对string（字符）的“+”操作符
    
    //通过bit开关设置，借助excpetion机制报告IO错误
    // Sets bits to report IO error using exception
    ifs.exceptions(ios::eofbit | ios::failbit | ios::badbit);
    
    //寻找相对于文件结束位置0偏离的position
    ifs.seekg(0, ios::end);
    //获取seekg所定位的poistion，文件的绝对位置就是文件自身的长度
    size_t len = ifs.tellg();
    
    //根据文件长度重新设置字符vector的尺寸
    data.resize(len);
    
    //重新定位文件操作position到文件开头处
    ifs.seekg(0, ios::beg);
    //从当前位置将文件大小的内容读入到data缓存
    ifs.read(&data[0], len);
}

//打印关于shader的log
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

//打印GL program对象信息
void printProgramInfoLog(GLuint program){
    GLint infologLength = 0;
    GLint charsWritten  = 0;
    
    //使用key-value对应方式获取log length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength) {
        //使用cpp string对象类型变量存储info log，string变量有infologLength容量约束
        string infoLog(infologLength, ' ');
        //string对象可被当作char[]类型来操作？这是一个方便的特征
        glGetProgramInfoLog(program, infologLength, &charsWritten, &infoLog[0]);
        std::cerr << "##### Log Program Info: #####\n" << infoLog << endl;
    }
}

//basic shader compiling method
//note here，debug mehtod and non-debug method，also note memory handling
bool compileShader(GLuint shaderHandle,const GLchar *source, const int32_t iSize) {
    if (source == NULL || iSize <= 0) return false;
    
    glShaderSource(shaderHandle, 1, &source, &iSize);  // Not specifying 3rd parameter
                                                       // (size) could be troublesome..
    
    glCompileShader(shaderHandle);
    
#if defined(DEBUG)
    GLint logLength;
    glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0) {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(shaderHandle, logLength, &logLength, log);
        printf("Shader compile log:\n%s", log);
        free(log);
    }
#endif
    
    GLint status;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        glDeleteShader(shaderHandle);
        return false;
    }
    
    return true;
}

//读取单一shader源码，然后挂载编译
void readAndCompileSingleShader(GLuint shaderHandle, const char *fn) {
    vector<char> source;
    readTextFile(fn, source); //把shader source文本作为text格式并存入source
    
//    const GLchar *ptrs = &source[0]; //shader source内存连续快的起始地址
//    const GLint lens = {static_cast<GLint>(source.size())}; //shader source的长度获取
//    glShaderSource(shaderHandle, 1, ptrs, lens);   // load the shader sources
//
//    glCompileShader(shaderHandle);
//
//    //打印shader日志
//    printInfoLog(shaderHandle, fn);
//
//    GLint compiled = 0;
//    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compiled);
    
    const GLchar *ptr = &source[0]; //shader source内存连续快的起始地址
    const GLint len = static_cast<GLint>(source.size()); //shader source的长度获取
    if (!compileShader(shaderHandle,ptr,len))
        throw runtime_error("fails to compile GL shader");
}



void linkShader(GLuint programHandle, GLuint vs, GLuint fs) {
    glAttachShader(programHandle, vs);
    glAttachShader(programHandle, fs);
    
    glLinkProgram(programHandle);
    
    //链接完shader之后，直接取消挂载（分离）shaders对象
    glDetachShader(programHandle, vs);
    glDetachShader(programHandle, fs);
    
    GLint linked = 0;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linked);
    
    printProgramInfoLog(programHandle);
    if (!linked){
        throw runtime_error("fails to link shaders");
    }
}

//整合的shaders pair的编译和链接程序
void readAndCompileShader(GLuint programHandle, const char * vertexShaderFileName, const char * fragmentShaderFileName) {
  GlShader vs(GL_VERTEX_SHADER);
  GlShader fs(GL_FRAGMENT_SHADER);

  readAndCompileSingleShader(vs, vertexShaderFileName);
  readAndCompileSingleShader(fs, fragmentShaderFileName);
    
    

  linkShader(programHandle, vs, fs);
}

//读取单一shader源码，然后挂载编译
void readAndCompileSingleShader(GLuint shaderHandle, const char *fn,std::map<std::string, std::string> &map_parameters) {
    vector<char> source;
    readTextFile(fn, source); //把shader source文本作为text格式并存入source
    
    const char REPLACEMENT_TAG = '*';
    // Fill-in parameters
    std::string str(source.begin(), source.end());
    std::string str_replacement_map(source.size(), ' ');
    
    std::map<std::string, std::string>::const_iterator it =
    map_parameters.begin();
    std::map<std::string, std::string>::const_iterator itEnd =
    map_parameters.end();
    while (it != itEnd) {
        size_t pos = 0;
        while ((pos = str.find(it->first, pos)) != std::string::npos) {
            // Check if the sub string is already touched
            
            size_t replaced_pos = str_replacement_map.find(REPLACEMENT_TAG, pos);
            if (replaced_pos == std::string::npos || replaced_pos > pos) {
                str.replace(pos, it->first.length(), it->second);
                str_replacement_map.replace(pos, it->first.length(), it->first.length(),
                                            REPLACEMENT_TAG);
                pos += it->second.length();
            } else {
                // The replacement target has been touched by other tag, skipping them
                pos += it->second.length();
            }
        }
        it++;
    }
    
    printf("Patched Shdader:\n%s", str.c_str());
    
    std::vector<uint8_t> v(str.begin(), str.end());
    str.clear();
    
    
    const GLchar *ptr = (GLchar *)&v[0]; //shader source内存连续快的起始地址
    const GLint len = static_cast<GLint>(v.size()); //shader source的长度获取
    if (!compileShader(shaderHandle,ptr,len))
        throw runtime_error("fails to compile GL shader");
}

//整合的shaders pair的编译和链接程序
void readAndCompileShader(GLuint programHandle, const char * vertexShaderFileName, const char * fragmentShaderFileName,std::map<std::string, std::string> &map_parameters) {
    
    
    GlShader vs(GL_VERTEX_SHADER);
    GlShader fs(GL_FRAGMENT_SHADER);
    
    readAndCompileSingleShader(vs, vertexShaderFileName,map_parameters);
    readAndCompileSingleShader(fs, fragmentShaderFileName,map_parameters);
    
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
    //根据类型生成shader封装对象GLShader
    GlShader vs(GL_VERTEX_SHADER);
    GlShader fs(GL_FRAGMENT_SHADER);
    
    loadAndCompileSingleShader(vs, vertexShaderSource);
    loadAndCompileSingleShader(fs, vertexShaderSource);
    
    linkShader(programHandle, vs, fs);
}
