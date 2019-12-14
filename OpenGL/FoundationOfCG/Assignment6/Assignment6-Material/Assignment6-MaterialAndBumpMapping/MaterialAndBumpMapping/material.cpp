#include <cassert>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>

#include "glsupport.h"
#include "asstcommon.h"
#include "material.h"

using namespace std;

struct GlProgramDesc {
    struct UniformDesc {
        string name;
        GLenum type;
        GLint size;
        GLint location;
    };
    
    struct AttribDesc {
        string name;
        GLenum type;
        GLint size;
        GLint location;
    };
    
    //默认构造函数初始化，同时生成OpenGL program object
    GlProgram program;
    
    vector<UniformDesc> uniforms;
    vector<AttribDesc> attribs;
    
    //构造函数，执行链接vertex/fragement shader的任务，链接成功后获得并将uniform/attribute属性放入缓存中
    //知识点：program链接成功后，随即将其所使用的uniforms/attributes取出到缓存中，这种使用方式几乎是惯例
    GlProgramDesc(GLuint vsHandle, GLuint fsHandle) {
        checkGlError("before GlProgramDesc - linkShader");
        linkShader(program, vsHandle, fsHandle);
        checkGlError("after GlProgramDesc - linkShader");
        
        checkGlError("before GlProgramDesc - glGetProgramiv");
        int numActiveUniforms, numActiveAttribs, uniformMaxLen, attribMaxLen;
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLen);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribMaxLen);
        
        checkGlError("after GlProgramDesc - glGetProgramiv");
        
        //通过API获取到的一些状态信息，根据信息设置缓存变量
        //将bufSize设置为uniform/attribute最大名称长度中的最大的值+1
        const int bufSize = max(uniformMaxLen, attribMaxLen) + 1;
        //将bufSize用作名称buffer的尺寸
        vector<GLchar> buffer(bufSize);
        
        //将uniforms缓存的尺寸设置为激活uniforms的数目
        uniforms.resize(numActiveUniforms);
        //通过遍历将获得所有active uniform，并存储到缓存中
        for (int i = 0; i < numActiveUniforms; ++i) {
            GLsizei charsWritten;
            glGetActiveUniform(program, i, bufSize, &charsWritten, & (uniforms[i].size), & (uniforms[i].type), &buffer[0]);
            assert(charsWritten + 1 <= bufSize);
            //将character buffer中被写入的字符转换为string
            uniforms[i].name = string(buffer.begin(), buffer.begin() + charsWritten);
            uniforms[i].location = glGetUniformLocation(program, &buffer[0]);
            
            
        }
        
        attribs.resize(numActiveAttribs);
        for (int i = 0; i < numActiveAttribs; ++i) {
            GLsizei charsWritten;
            glGetActiveAttrib(program, i, bufSize, &charsWritten, &(attribs[i].size), &(attribs[i].type), &buffer[0]);
            assert(charsWritten + 1 <= bufSize);
            attribs[i].name = string(buffer.begin(), buffer.begin() + charsWritten);
            attribs[i].location = glGetAttribLocation(program, &buffer[0]);
        }
        
//        if (!g_Gl2Compatible)
        //这个api并不存在，可能是某个厂商的扩展接口
        //glBindFragDataLocation(program, 0, "fragColor");
        
        
    }
};

//GLProgramLibrary的目的？将所有OpenGL管线porgram对象管理起来，方便使用。用map维护program和shader缓存
//GLProgramLibrary的组织方式？ 将program和shader放在缓存中维护和使用，然后暴露singleton实例
//GLProgramLibrary的设计目的？实现为无头文件方式，意味着专注于在material类内部使用
class GlProgramLibrary {
    typedef map<pair<string, GLenum>, shared_ptr<GlShader> > GlShaderMap;
    typedef map<pair<string, string>, shared_ptr<GlProgramDesc> > GlProgramDescMap;
    
    GlShaderMap shaderMap;
    GlProgramDescMap programMap;
    
    GlProgramLibrary() {}
    
public:
    //c++中如何实现singleton模式？ 声明静态局部变量，然后再返回变量的引用
    //static关键字的本质和作用？static的本质是storage class specifier
    //static应用在file-scope的声明上时，表达internal linkage的含义，意味着这个方法只能在本文件内部访问
    static GlProgramLibrary& getSingleton() {
        //static静态变量,static storage duration
        static GlProgramLibrary pl;
        return pl;
    }
    
    //从程序缓存获取，或者重新生成一个programDesc对象并放入到缓存中
    shared_ptr<GlProgramDesc> getProgramDesc(const string& vsFilename, const string& fsFilename) {
        GlProgramDescMap::key_type key(vsFilename, fsFilename);
        
        GlProgramDescMap::iterator i = programMap.find(key);
        if (i == programMap.end()) {
            //知识点：shared_ptr类型变量的初始化
            shared_ptr<GlProgramDesc> program(new GlProgramDesc(*getShader(vsFilename, GL_VERTEX_SHADER), *getShader(fsFilename, GL_FRAGMENT_SHADER)));
            programMap[key] = program;
            //将刚生成的programDesc对象作为函数返回值
            return program;
        }
        else {
            //否则直接返回map中的programDesc值
            //知识点：cpp中map类型对与每个key-value属性都有直接的变量指向他们
            return i->second;
        }
    }
    
protected:
    shared_ptr<GlShader> getShader(const string& filename, GLenum shaderType) {
        string f = filename;
        //根据bool flag设置对应的文件名后缀
//        if (g_Gl2Compatible) { // optionally change -gl3 to -gl3 in the end of the filename
//            size_t pos = f.rfind("-gl3");
//            if (pos != string::npos) {
//                f[pos+3] = '2';
//            }
//        }
        
        //shader缓存的使用和维护
        GlShaderMap::key_type key(f, shaderType);  //key_type的类型对应于GlShaderMap定义中的key
        GlShaderMap::iterator i = shaderMap.find(key);
        if (i == shaderMap.end()) { //如果shaderMap中不存在，则加载shader并存入map
            //shared_ptr<GLShader>声明及初始化语法
            shared_ptr<GlShader> shader(new GlShader(shaderType));
            readAndCompileSingleShader(*shader, f.c_str());
            shaderMap[key] = shader;
            return shader;
        }
        else {
            return i->second;
        }
    }
};

//Material构造函数实现
//用GLProgramLibrary中的方法初始化shader files，并准备ProgramDesc对象
//Material对象的作用即是加载shaders、构造对应的Programs，填充uniforms。最后基于这些信息构造进行绘制。
Material::Material(const string& vsFilename, const string& fsFilename)
: programDesc_(GlProgramLibrary::getSingleton().getProgramDesc(vsFilename, fsFilename))
{}

//从本地缓存中获取GL类型的预对应名称的实现方法？实现以简单的for循环方式
static const char * getGlConstantName(GLenum c) {
    struct ValueNamePair {
        GLenum value;
        const char * name;
    };
    
    ValueNamePair valueNamePairs[] = {
        { GL_FLOAT, "GL_FLOAT" },
        { GL_FLOAT_VEC2, "GL_FLOAT_VEC2" },
        { GL_FLOAT_VEC3, "GL_FLOAT_VEC3" },
        { GL_FLOAT_VEC4, "GL_FLOAT_VEC4" },
        { GL_FLOAT_MAT2, "GL_FLOAT_MAT2" },
        { GL_FLOAT_MAT3, "GL_FLOAT_MAT3" },
        { GL_FLOAT_MAT4, "GL_FLOAT_MAT4" },
        { GL_FLOAT_MAT2x3, "GL_FLOAT_MAT2x3" },
        { GL_FLOAT_MAT2x4, "GL_FLOAT_MAT2x4" },
        { GL_FLOAT_MAT3x2, "GL_FLOAT_MAT3x2" },
        { GL_FLOAT_MAT3x4, "GL_FLOAT_MAT3x4" },
        { GL_FLOAT_MAT4x2, "GL_FLOAT_MAT4x2" },
        { GL_FLOAT_MAT4x3, "GL_FLOAT_MAT4x3" },
        { GL_INT, "GL_INT" },
        { GL_INT_VEC2, "GL_INT_VEC2" },
        { GL_INT_VEC3, "GL_INT_VEC3" },
        { GL_INT_VEC4, "GL_INT_VEC4" },
//        { GL_UNSIGNED_INT_VEC2, "GL_UNSIGNED_INT_VEC2" },
//        { GL_UNSIGNED_INT_VEC3, "GL_UNSIGNED_INT_VEC3" },
//        { GL_UNSIGNED_INT_VEC4, "GL_UNSIGNED_INT_VEC4" },
        { GL_SAMPLER_1D, "GL_SAMPLER_1D" },
        { GL_SAMPLER_2D, "GL_SAMPLER_2D" },
        { GL_SAMPLER_CUBE, "GL_SAMPLER_CUBE" },
        { GL_SAMPLER_1D_SHADOW, "GL_SAMPLER_1D_SHADOW"},
        { GL_SAMPLER_2D_SHADOW, "GL_SAMPLER_2D_SHADOW"},
    };
    
    for (int i = 0, n = sizeof(valueNamePairs)/sizeof(valueNamePairs[0]); i < n; ++i) {
        if (valueNamePairs[i].value == c)
            return valueNamePairs[i].name;
    }
    return "Unkonwn";
}

//draw函数是material的关键函数
void Material::draw(Geometry& geometry,const Uniforms& extraUniforms){
    //static storage duration最大textureImageUnits变量
    static GLint maxTextureImageUnits = 0;
    
    checkGlError("before material glGetIntegerv-GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS");
    //使用API查询获得最大的texture image unit数目
    // Initialize maxTextureImageUnits if this is called for the first time
    if (maxTextureImageUnits == 0) {
        //获取最大复合纹理图像图像单元数的API调用
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureImageUnits);
        assert(maxTextureImageUnits > 0); // GL spec says this has to be at least 2
    }
    
    //制定当前的program
    checkGlError("before material glUserProgram");
    glUseProgram(programDesc_->program);
    checkGlError("after material glUserProgram");
    
//    glValidateProgram(programDesc_->program);
//
//    GLint validated = false;
//    // Check the validate status
//    glGetProgramiv ( programDesc_->program, GL_VALIDATE_STATUS, &validated );
//
//    if(!validated){
//        printProgramInfoLog(programDesc_->program);
//    }
//    checkGlError("after validating Program");
    
    //应用当前render sates对象
    renderStates_.apply();  // transit to current states
    checkGlError("after renderStates_.apply");
    
    // Step 1:
    // set the uniforms and bind the textures
    int textureUnit = 0;
    for (int i = 0, n = static_cast<int>(programDesc_->uniforms.size()); i < n; ++i) {
        const GlProgramDesc::UniformDesc& ud = programDesc_->uniforms[i];
        
        const Uniforms* uniformsList[] = {&uniforms_, &extraUniforms};
        int j = 0;
        for (; j < 2; ++j) {
            const Uniforms::Value* u = uniformsList[j]->get(ud.name);
            
            // if the name looks like blah[0], and the uniform is not found, we also try stripping the '[0]'
            if (u == NULL && ud.name.length() >= 3 && ud.name.compare(ud.name.length() - 3, 3, "[0]") == 0)
                u = uniformsList[j]->get(ud.name.substr(0, ud.name.length() - 3));
            
            if (u) {
                //知识点：对texture uniform变量值设置方式的特殊处理？
                if (u->type == ud.type && u->size >= ud.size) {
                    switch (u->type) {
                        case GL_SAMPLER_1D:
                        case GL_SAMPLER_2D:
                        case GL_SAMPLER_CUBE:
                        case GL_SAMPLER_1D_SHADOW:
                        case GL_SAMPLER_2D_SHADOW:
                        {
                        const shared_ptr<Texture> *tex = u->getTextures();
                        
                        // If this assert hits, the Uniform::Value is incorrectly implemented
                        assert(tex != NULL);
                        static const int MAX_TEX_UNITS = 1024;
                        GLint texUnits[MAX_TEX_UNITS];
                        int count = 0;
                        //知识点：这个循环是为了封装纹理数组吗？一次应用多个纹理。每次假定纹理都是以数组的方式上传uniform变量，实际可涵盖单个和集合方式。
                        for (; count < ud.size; ++count) {
                            //针对纹理数组指定绑定的textureUnit时，每次都检查textureUnit是否越界。
                            if (textureUnit == maxTextureImageUnits) {
                                //知识点：stringstream，用字符流输出串联好的字符
                                stringstream s;
                                s << "System allows a maximum of " << maxTextureImageUnits << ". The current shader is trying to use more than that.";
                                throw runtime_error(s.str());
                            }
                            
                            //知识点：用textureUnit作为纹理单元数目，纹理绑定后，将textureUnit计数加1
                            //每次给shader发送对应变量值时，需要先激活texture unit，然后绑定特定的texture object（by textureId）到texture target，这时的texture unit就开始发挥作用，可以上传。
                            glActiveTexture(GL_TEXTURE0 + textureUnit);
                            tex[count]->bind();
                            //将textureUnit计数加1，同时将其累加前的值用作当前的textureUnit值
                            texUnits[count] = textureUnit++;
                        }
                        //将uniform sampler变量的数值专门上传
                        u->apply(ud.location, ud.size, texUnits);
                        }
                            break;
                        default:
                            //非texture的uniform数值绑定
                            u->apply(ud.location, ud.size, NULL);
                            checkGlError("u->apply");
                    }
                }
                else {
                    stringstream s;
                    s << "Uniform variable " << ud.name << ": supplied value and declared variable do not match in type and/or size."
                    << "\nSupplied value: type = " << getGlConstantName(u->type) << ", size = " << u->size
                    << "\nDeclared in shader: type = " << getGlConstantName(ud.type) << ", size = " << ud.size;
                    throw runtime_error(s.str());
                }
                break;
            }
        }
        if (j == 2) {
            stringstream s;
            s << "Uniform variable " << ud.name << ": used in the shader codes, but not supplied. Type = " << getGlConstantName(ud.type) << ", Size = " << ud.size;
            throw runtime_error(s.str());
        }
    }
    
    // Step 2:
    // see what attribs are provided by the geometry
    // 将几何对象的属性和Shader中的属性名称匹配，然后将匹配的attribute location放入数组中。
    const vector<string>& geoAttribNames = geometry.getVertexAttribNames();
    
    const static int MAX_ATTRIB = 64;
    int attribIndices[MAX_ATTRIB];
    const size_t numAttribs = geoAttribNames.size();
    
    if (numAttribs > MAX_ATTRIB) {
        throw runtime_error(string("Number of attributes contained in geometry is greater than maximally supported number of attributes. Consider increasing MAX_ATTRIB."));
    }
    
    for (size_t i = 0; i < numAttribs; ++i) {
        //将attributes的shader location indices初始化为-1
        attribIndices[i] = -1;
    }
    
    // simple and stupid O(n^2) wiring, should use a hashtable to reduce to O(n)
    for (int i = 0, n = programDesc_->attribs.size(); i < n; ++i) {
        const GlProgramDesc::AttribDesc& ad = programDesc_->attribs[i];
        
        size_t j = 0;
        for (; j < numAttribs; ++j) {
            if (geoAttribNames[j] == ad.name) {
                attribIndices[j] = ad.location;
                break;
            }
        }
        if (j == numAttribs) {
            throw runtime_error(string("Vertex attribute ") + ad.name
                                + ": used in the shader codes, but not supplied.");
        }
    }
    
    
    GLboolean debug_attr = true;
    if(debug_attr){
        int numActiveUniforms, numActiveAttribs, uniformMaxLen, attribMaxLen;
        
        GLint program = programDesc_->program;
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);
        
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLen);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribMaxLen);
        
        //checkGlError("after GlProgramDesc - glGetProgramiv");
        
        //通过API获取到的一些状态信息，根据信息设置缓存变量
        //将bufSize设置为uniform/attribute最大名称长度中的最大的值+1
        const int bufSize = max(uniformMaxLen, attribMaxLen) + 1;
        //将bufSize用作名称buffer的尺寸
        vector<GLchar> buffer(bufSize);
        
        string dname;
        GLint dlocation;
        GLenum dtype;
        GLint dsize;
        for (int i = 0; i < numActiveAttribs; ++i) {
            GLsizei charsWritten;
            glGetActiveAttrib(program, i, bufSize, &charsWritten, &dsize, &dtype, &buffer[0]);
            assert(charsWritten + 1 <= bufSize);
            dname = string(buffer.begin(), buffer.begin() + charsWritten);
            dlocation = glGetAttribLocation(program, &buffer[0]);
        }
    }
    
//    for (size_t i = 0; i < numAttribs; ++i) {
//        //只要有对应的属性Index，则开启vertex array方式
//        if (attribIndices[i] >= 0)
//            glEnableVertexAttribArray(attribIndices[i]);
//        checkGlError("after glEnableVertexAttribArray");
//        printProgramInfoLog(programDesc_->program);
//    }
    
    // Now let the geometry draw its self
    // 几何对象根据vbo/ibo中的数据绘制自身，这种方式支持多个VBO？
    checkGlError("before geometry.draw");
    geometry.draw(attribIndices);
    checkGlError("after geometry.draw");
    
    //此处Enable/DisableVertexAttribArray引出的问题不小，几乎所有glError(0x502)都是由其引起。
//    for (size_t i = 0; i < numAttribs; ++i) {
//        if (attribIndices[i] >= 0)
//            glDisableVertexAttribArray(attribIndices[i]);
//    }
//    checkGlError("Material::draw endingPoint");
    
}

void Material::draw(Geometry& geometry){
    Uniforms extraUniforms;
    draw(geometry, extraUniforms);
}
