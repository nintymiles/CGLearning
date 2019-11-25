#ifndef UNIFORMS_H
#define UNIFORMS_H

#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
//#if __GNUG__
//#   include <tr1/memory>
//#endif

#include "cvec.h"
#include "matrix4.h"
#include "glsupport.h"
#include "texture.h"

// Private namespace for some helper functions. You should ignore this unless you
// are interested in the internal implementation.

// Private name _helper中服装了用于自定义CVec矢量类型族的const uniform设置，涵盖了基础结构可能用到的所有类型如何设置uniform值
namespace _helper {
    inline void genericGlUniformi(GLint location, int i) {
        ::glUniform1i(location, i);
    }
    inline void genericGlUniformf(GLint location, float f) {
        ::glUniform1f(location, f);
    }
    inline void genericGlUniformv(GLint location, int size, const GLint *v) {
        ::glUniform1iv(location, size, v);
    }
    inline void genericGlUniformv(GLint location, int size, const GLfloat *v) {
        ::glUniform1fv(location, size, v);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<int, 1> *v) {
        ::glUniform1iv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<int, 2> *v) {
        ::glUniform2iv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<int, 3> *v) {
        ::glUniform3iv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<int, 4> *v) {
        ::glUniform4iv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<float, 1> *v) {
        ::glUniform1fv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<float, 2> *v) {
        ::glUniform2fv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<float, 3> *v) {
        ::glUniform3fv(location, size, &v[0][0]);
    }
    inline void genericGlUniformv(GLint location, int size, const Cvec<float, 4> *v) {
        ::glUniform4fv(location, size, &v[0][0]);
    }
    inline void genericGlUniformMatrix4v(GLint location, int size, const Cvec<float, 16> *m) {
        ::glUniformMatrix4fv(location, size, GL_FALSE, &m[0][0]);
    }
    
    //获得矢量类型所对应的GL类型
    template<typename T, int n>
    inline GLenum getTypeForCvec();   // should replace with STATIC_ASSERT
    
    template<>
    inline GLenum getTypeForCvec<int, 1>() { return GL_INT; }
    template<>
    inline GLenum getTypeForCvec<int, 2>() { return GL_INT_VEC2; }
    template<>
    inline GLenum getTypeForCvec<int, 3>() { return GL_INT_VEC3; }
    template<>
    inline GLenum getTypeForCvec<int, 4>() { return GL_INT_VEC4; }
    template<>
    inline GLenum getTypeForCvec<float, 1>() { return GL_FLOAT; }
    template<>
    inline GLenum getTypeForCvec<float, 2>() { return GL_FLOAT_VEC2; }
    template<>
    inline GLenum getTypeForCvec<float, 3>() { return GL_FLOAT_VEC3; }
    template<>
    inline GLenum getTypeForCvec<float, 4>() { return GL_FLOAT_VEC4; }
    template<>
    inline GLenum getTypeForCvec<bool, 1>() { return GL_BOOL; }
    template<>
    inline GLenum getTypeForCvec<bool, 2>() { return GL_BOOL_VEC2; }
    template<>
    inline GLenum getTypeForCvec<bool, 3>() { return GL_BOOL_VEC3; }
    template<>
    inline GLenum getTypeForCvec<bool, 4>() { return GL_BOOL_VEC4; }
}

//Uniforms类的功能描述及所适用的类型、以及使用方法、初始状态
// The Uniforms keeps a map from strings to values
//
// 当前Uniforms类可实际支持的uniform variable typetype
// Currently the value can be of the following type:
// - Single int, float, or Matrix4
// - Cvec<T, n> with T=int or float, and n = 1, 2, 3, or 4
// - shared_ptr<Texture>  //支持纹理上传并同时设置uniform texture type值
// - arrays of any of the above
//
// You either use uniform.put("varName", val) or
// uniform.put("varArrayName", vals, numVals); //以matrix4 uniform为例考虑
//
// A Uniforms instance will start off empty, and you can use
// its put member function to populate it.

class Uniforms {
public:
    Uniforms& put(const std::string& name, int value) {
        Cvec<int, 1> v(value);
        valueMap[name].reset(new CvecsValue<int, 1>(&v, 1));
        return *this;
    }
    
    Uniforms& put(const std::string& name, float value) {
        Cvec<float, 1> v(value);
        valueMap[name].reset(new CvecsValue<float, 1>(&v, 1));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const Matrix4& value) {
        valueMap[name].reset(new Matrix4sValue(&value, 1));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const std::shared_ptr<Texture>& value) {
        valueMap[name].reset(new TexturesValue(&value, 1));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<int, n>& v) {
        valueMap[name].reset(new CvecsValue<int, n>(&v, 1));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<float, n>& v) {
        valueMap[name].reset(new CvecsValue<float, n>(&v, 1));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<double, n>& v) {
        Cvec<float, n> u;
        for (int i = 0; i < n; ++i) {
            u[i] = float(v[i]);
        }
        valueMap[name].reset(new CvecsValue<float, n>(&u, 1));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const int *values, int count) {
        valueMap[name].reset(new CvecsValue<int, 1>(reinterpret_cast<const Cvec<int, 1>*>(values), count));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const float *values, int count) {
        valueMap[name].reset(new CvecsValue<int, 1>(reinterpret_cast<const Cvec<float, 1>*>(values), count));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const Matrix4 *values, int count) {
        valueMap[name].reset(new Matrix4sValue(values, count));
        return *this;
    }
    
    Uniforms& put(const std::string& name, const std::shared_ptr<Texture> *values, int count) {
        valueMap[name].reset(new TexturesValue(values, count));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<int, n> *v, int count) {
        valueMap[name].reset(new CvecsValue<int, n>(v, count));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<float, n> *v, int count) {
        valueMap[name].reset(new CvecsValue<float, n>(v, count));
        return *this;
    }
    
    template<int n>
    Uniforms& put(const std::string& name, const Cvec<double, n> *v, int count) {
        valueMap[name].reset(new CvecsValue<float, n>(v, count));
        return *this;
    }
    
    // Future work: add put for different sized matrices, and array of basic types
protected:
    
    //下面的实现细节很恐怖，提前警告
    // Ghastly implementation details follow. Viewer be warned.
    
    friend class Material;
    class ValueHolder;
    class Value;
    
    typedef std::map<std::string, ValueHolder> ValueMap;
    
    //声明Uniforms容器的存储变量
    ValueMap valueMap;
    
    //从valueMap中查找name所指定的值
    const Value* get(const std::string& name) const {
        std::map<std::string, ValueHolder>::const_iterator i = valueMap.find(name);
        return i == valueMap.end() ? NULL : i->second.get();
    }

    class ValueHolder {
        Value *value_;
        
    public:
        ValueHolder() : value_(NULL) {}
        ValueHolder(Value* value) : value_(value) {}
        ValueHolder(const ValueHolder& u) : value_(u.value_ ? u.value_->clone() : NULL) {}
        ~ValueHolder() {
            if (value_)
                delete value_;
        }
        void reset(Value* value) {
            if (value_)
                delete value_;
            value_ = value;
        }
        
        Value *get() const {
            return value_;
        }
        
        ValueHolder& operator= (const ValueHolder& u) {
            reset(u.value_ ? u.value_->clone() : NULL);
            return *this;
        }
    };

    //uniform变量的基础封装
    class Value {
    public:
        //每个uniform变量都对应有类型
        // One of the uniform type as returned by glGetActiveUniform, used for matching
        const GLenum type;
        
        //基本知识点--uniform变量的尺寸特征封装？uniform变量的尺寸封装，若非集合类型，则值为1，若集合类型，则为集合元素的数目
        // 1 for non-array type, otherwise the number of elements in the array
        const GLint size;
        
        //成员函数，实现clone功能，返回类型指针
        virtual Value* clone() const = 0;
        virtual ~Value() {}
        
        //针对texture类型的特定成员函数
        // If type is one of GL_SAMPLER_*, the getTextures should provide a pointer to
        // the array of shared_ptr<Texture> stored by the uniform. And apply should
        // use the boundTexUnits argument as the argument for glUniform*.
        //
        // Otherwise, boundTexUnit should be ignored and whatever values contained in
        // the Value instance should be set to given location.
        //
        // `count' specifies how many actural uniforms are specified by the shader, and
        // should be used as input parameter to glUniform*
        virtual void apply(GLint location, GLsizei count, const GLint *boundTexUnits) const = 0;
        virtual const std::shared_ptr<Texture> * getTextures() const { return NULL; };
        
    protected:
        Value(GLenum aType, GLint aSize) : type(aType), size(aSize) {}
    };
    

    template<typename T, int n>
    class CvecsValue : public Value {
        std::vector<Cvec<T, n> > vs_;
        
    public:
        CvecsValue(const Cvec<T, n> *vs, int size)
        : Value(_helper::getTypeForCvec<T,n>(), size), vs_(vs, vs + size) { //vs_(vs, vs + size)，以iterator范围从其它容器初始化
            assert(size > 0);
        }
        
        // construct from Cvecs of another type
        template<typename S>
        CvecsValue(const Cvec<S, n> *vs, int size)
        : Value(_helper::getTypeForCvec<T,n>(), size), vs_(size) {
            assert(size > 0);
            for (int i = 0; i < size; ++i) {
                for (int d = 0; d < n; ++d) {
                    vs_[i][d] = T(vs[i][d]);
                }
            }
        }
        
        virtual Value* clone() const {
            return new CvecsValue(*this);
        }
        
        virtual void apply(GLint location, GLsizei count, const GLint *boundTexUnit) const {
            assert(count <= size);
            _helper::genericGlUniformv(location, count, &vs_[0]);
        }
    };

    //support Matrix4 list to be uploaded as uniform matrices data。
    class Matrix4sValue : public Value {
        // we use cvecs here instead of matrix4s here since matrix4
        // doesn't allow double element (yet), and to pass the data
        // into glUniformMatrix4fv, we need to have the internal buffer
        // to be typed float.
        std::vector<Cvec<float, 16> > ms_;
    public:
        Matrix4sValue(const Matrix4 *m, int size)
        : Value(GL_FLOAT_MAT4, size), ms_(size)
        {
        assert(size > 0);
        for (int i = 0; i < size; ++i)
            m[i].writeToColumnMajorMatrix(&ms_[i][0]);
        }
        
        virtual Value* clone() const {
            return new Matrix4sValue(*this);
        }
        
        virtual void apply(GLint location, GLsizei count, const GLint *boundTexUnit) const {
            assert(count <= size);
            _helper::genericGlUniformMatrix4v(location, count, &ms_[0]);
        }
    };

    //纹理直的再封装，目的是什么？
    class TexturesValue : public Value {
        //Texture的本地缓存，使用vector的意义在于支持cubeMap texture？
        std::vector<std::shared_ptr<Texture> > texs_;
    public:
        //重要概念：构造函数的参数为texture数组，以及数组的尺寸数目
        TexturesValue(const std::shared_ptr<Texture> *tex, int size)
        : Value(tex[0]->getSamplerType(), size), texs_(tex, tex + size) //texs_(tex, tex + size)这个初始化的含义？将本地的texture缓存初始化为所传入数组的内容
        {
        assert(size > 0);
        for (int i = 0; i < size; ++i) {
            assert(tex[i]->getSamplerType() == type); //循环的含义？ 检查每个Texture对象的类型是否和TextureValue中指定的类型一致
        }
        }
        
        //关注这个clone函数的实现机制
        virtual Value* clone() const {
            //基础知识点：1--new的作用是动态寻址内存并可初始化对象，2--new的工作方式方式是返回所寻址对象的指针，并没有直接命名变量的功能， 3--new方式的初始化方式支持 We can initialize a dynamically allocated object using direct initialization . We can use traditional construction (using parentheses 括号), and under the new standard, we can also use list initialization (with curly braces 花括号):
            //重点理解：此处new语句的真正含义就是，首先为TextureValues动态寻址内存，然后初始化为当前指针的内容。*this的含义为引用this指针所指向的当前对象。那么这个语句确实实现了clone功能
            return new TexturesValue(*this);
        }
        
        //texture uniform值非texture本身所含值，为所指定的物体texture unit index，故而不能从具体类型的值本身获得，需要额外传入
        virtual void apply(GLint location, GLsizei count, const GLint *boundTexUnits) const {
            assert(count <= size);
            _helper::genericGlUniformv(location, count, boundTexUnits); //为纹理uniform设置texture unit值
        }
        
        virtual const std::shared_ptr<Texture> *getTextures() const {
            //返回第一个容器元素地址，此处隐含意义是容器内的连续内存区域都可以访问
            return &texs_[0];
        }
    };
};


#endif
