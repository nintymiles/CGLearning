#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <cassert>
#include <map>
#include <cmath>
#include <string>
#include <stdexcept>
#include <memory>

//#if __GNUG__
//#   include <tr1/memory>
//#endif

#include "cvec.h"
#include "glsupport.h"
#include "geometrymaker.h"

//抽象类，封装了vertex attributes等几何数据，且知道如何绘制自身
//Geometry对象可发起对自身的绘制
// An abstract class that encapsulates geometry data that provides vertex attributes and
// know how to draw itself.
class Geometry {
public:
    //纯虚函数，用于提供几何对象所提供的所有vertex attributes，需要实现类实现
    // return names of vertex attributes provided by this geometry
    virtual const std::vector<std::string>& getVertexAttribNames() = 0;
    
    //对attribIndices提供说明，其数据组织应该满足attribute location确切对应于数组index本身
    // Draw the geometry. attribIndices[i] corresponds to the index of the
    // shader vertex attribute location that the i-th vertex attribute provided
    // by this geometry should bind to. It can be -1 to indicate that this stream is
    // not used. The caller is responsible for enable/disable vertex attribute arrays.
    virtual void draw(int attribIndices[]) = 0;
    
    virtual ~Geometry() {}
};

//提供一个参考样例BufferObjectGeometry，提供功能有：允许使用来自一个或多个vertex buffers的vertices，0个或一整个index buffers，使用不同的primitives进行绘制
//BufferObjectGeometry提供了对绘制复杂性的封装，所封装的复杂性包括vertex data和绘制方式
// ============================================================================
// We provide a flexible implementation of Geometry called BufferObjectGeometry
// that allows drawing using vertices from one or more vertex buffers, zero or
// one index buffers, as well as using different primitives (GL_TRIANGLES,
// GL_POINTS, etc).  The following is its implementation.
//
// For very simple usage cases where such flexibility is not desired, you
// can used one of the SimpleGeometry* or SimpleIndexedGeometry* types defined
// at the end of this header. They are built on top of the BufferObjectGeometry
// structure, and demos its usage
// ============================================================================

//vertex数据格式封装，包括了一个attribute描述列表
// Helper class that describes the format of a vertex. Maintains
// a list of attribute descriptions
//一个Vertex Attribute的封装，包含名称，attribute size，顶点数目以及顶点数据的尺寸等，但是attribute location数据由外部传入。
class VertexFormat {

public:
    //封装vertex attribute设置所需要的参数，本封装只适用于vertex buffer数据使用
    // Parameters that you would pass into glVertexAttribPointer
    struct AttribDesc {
        std::string name;
        GLint size;
        GLenum type;
        GLboolean normalized;
        int offset;
        
        AttribDesc(const std::string& _name, GLint _size, GLenum _type, GLboolean _normalized, int _offset)
        : name(_name), size(_size), type(_type), normalized(_normalized), offset(_offset) {
            assert(_name != "");   // some basic sanity（健全） checks
            assert(_size > 0);
            assert(_offset >= 0);
        }
    };

    //构造函数，同时初始化vertexSize_常量整型，vertexSize为顶点的数据尺寸
    // Initialize to zero attributes？？
    VertexFormat(int vertexSize) : vertexSize_(vertexSize) {}
    
    // append a new attrib description
    VertexFormat& put(const std::string& name, GLint size, GLenum type, GLboolean normalized, int offset) {
        //cpp方式初始化一个AttribDesc类型
        AttribDesc ad(name, size, type, normalized, offset);
        //若map容器中找不到name，则加入attribDescs的index
        if (name2Idx_.find(name) == name2Idx_.end()) {
            //当前attribDescs的数目刚好可作为attribDescs_的自然索引
            name2Idx_[name] = static_cast<int>(attribDescs_.size());
            //确定attribute location值之后再将ad压入attribDescs
            //in other words,name2Idx_ maintains indices to attriDescs for a specific attribute name
            attribDescs_.push_back(ad);
        }
        //如果name在本地缓存中存在，则直接将缓存设置为新attribDesc数据
        else
            attribDescs_[name2Idx_[name]] = ad; //如果 attribute name已经存在，则直接替换数据
        
        return *this;
    }
    
    int getVertexSize() const {
        return vertexSize_;
    }
    
    //获得vertex attribute数目
    int getNumAttribs() const {
        return static_cast<int>(attribDescs_.size());//attribDescs_.size();
    }
    
    //获得具体索引所指向的本地缓存中的attribute
    const AttribDesc& getAttrib(int i) const {
        return attribDescs_[i];
    }
    
    //获得attribute name所对应的本地缓存中的位置index
    // returns the index of the atribute with given name, or -1 if the name is not found
    int getAttribIndexForName(const std::string& name) const {
        std::map<std::string, int>::const_iterator i =  name2Idx_.find(name);
        return i == name2Idx_.end() ? -1 : i->second;
    }
    
    // Calls glVertexAttribPointer with appropirate arguments to bind the attribute
    // indexed by 'attribIndex' within this VertexFormat to vertex attribute location
    // specified by 'glAttribLocation'
    // Notes -- the biggest role of AttributeDesc object is to guide setting of params of glVertexAttribPointer()，同时数据则认为已经上传到VBO中
    void setGlVertexAttribPointer(int attribIndex, int glAttribLocation) const {
        assert(glAttribLocation >= 0);
        const AttribDesc &ad = attribDescs_[attribIndex];
        glVertexAttribPointer(glAttribLocation, ad.size, ad.type, ad.normalized, vertexSize_, reinterpret_cast<const GLvoid*>(ad.offset));
    }

//privtate variables的定义放置到定义末尾是因为引用了public中定义的类型
private:
    const int vertexSize_;
    //AttribDesc数据容器
    std::vector<AttribDesc> attribDescs_;
    //以name:index的map容器
    std::map<std::string, int> name2Idx_;
    
};

//针对GL buffer object的轻量封装，比如vertex buffer，包含vertices及其格式信息，当然包含vertex buffer object的维护
// Light wrapper for a GL buffer object storing vertices, together with format for its vertices.
class FormattedVbo : public GlBufferObject {
    //vertex data format type
    const VertexFormat& format_;
    int length_;
    
public:
    //构造函数以引用方式传入了vertexFormat参数，对其生存周期需要关注
    // The passed in formatDesc_ is stored by reference. Hence the caller
    // should either pass in a static global variable, or ensure its lifespan
    // encompasses the lifespan of the FormmatedVbo
    FormattedVbo(const VertexFormat& formatDesc)
    : format_(formatDesc), length_(0) {}  //构造初始化
    
    //获得缓存对象中的vertex data format
    const VertexFormat& getVertexFormat() const {
        return format_;
    }
    
    // Number of vertices stored in the vertex
    int length() const {
        return length_;
    }
    
    //封装vertex data上传，特别关注vertex buffer的上传方式，分为多次上传和一次型静态上传，所对应的APIs并不相同
    //关注glBufferData和glBufferSubData的用法区别
    // Upload vertex data to the vbo. Specify dynamicUsage = true if you intend
    // to upload different data multiple times
    template<typename Vertex>
    void upload(const Vertex* vertices, int length, bool dynamicUsage = false) {
        assert(sizeof(Vertex) == format_.getVertexSize());
        glBindBuffer(GL_ARRAY_BUFFER, *this);
        length_ = length;
        
        const int size = sizeof(Vertex) * length;
        if (dynamicUsage) {
            // We always call glBufferData with a NULL ptr here, so that OpenGL knows that
            // we're done with old data, and that if the old vbo is in use, it can  allocate
            // us a new one without stalling the pipeline
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
        }
        else {
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        }
#ifndef NDEBUG
        checkGlError(__func__);
#endif
    }
};

//对element array缓存对象的封装
// Light wrapper for a GL buffer object storing indices, together with format for its
// indices, one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, or GL_UNSIGNED_INT
class FormattedIbo : public GlBufferObject {
    GLenum format_;
    int length_;
public:
    //构造函数，index的数据类型只支持3种
    // format must be one of GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT,or GL_UNSIGNED_INT
    // GL_UNSIGNED_SHORT is the default
    FormattedIbo(GLenum format = GL_UNSIGNED_SHORT) : format_(format), length_(0) {
        assert(format == GL_UNSIGNED_BYTE || format == GL_UNSIGNED_SHORT || format == GL_UNSIGNED_INT);
    }
    
    GLenum getIndexFormat() const {
        return format_;
    }
    
    int length() const {
        return length_;
    }
    
    //function template，使用这种声明，就可以支持多种类型Index数据，注意这种方式的使用？？？
    template<typename Index>
    void upload(const Index *indices, int length, bool dynamicUsage = false) {
        assert((format_ == GL_UNSIGNED_BYTE && sizeof(Index) == 1) ||
               (format_ == GL_UNSIGNED_SHORT && sizeof(Index) == 2) ||
               (format_ == GL_UNSIGNED_INT && sizeof(Index) == 4));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *this);
        length_ = length;
        const int size = sizeof(Index) * length;
        if (dynamicUsage) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
        }
        else {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
        }
#ifndef NDEBUG
        checkGlError(__func__);
#endif
    }
    
};

//一个有弹性的Geometry class实现，支持灵活的一个**或*多个vertex buffers使用，借助或不借助index buffers，以及primitives type的灵活指定，强大的设计
// A flexible light weight Geometry implementation allowing drawing using multiple vertex buffers,
// with or without an index buffer, and as different primitives (e.g., triangles, quads, points...).
//
// This* essentially maintains a map of
//   vertex attribute names --> (FormattedVbo, attribute name)
//
// To draw its self, it binds all the vertex attributes that it is wired to, and calls
// the suitable OpenGL calls to draw either indexed or non-index geometry. There are optimizations
// to call glBindBuffer only once for each distince FormattedVbo it wires to.

//将VBO和具体的几何顶点产生关系
class BufferObjectGeometry : public Geometry {
public:
    // Initialize, with zero vertex buffer and index buffer attached. Defaults to TRIANGLES primitive
    BufferObjectGeometry();
    
    // Declares and maps a vertex attribute named 'targetAttribName' to the
    // vertex attribute named 'sourceAttribName' of the 'source' FormattedVbo
    BufferObjectGeometry& wire(const std::string& targetAttribName,
                               std::shared_ptr<FormattedVbo> source,
                               const std::string& sourceAttribName);
    
    // Declares and maps a vertex attribute to the vertex attribute
    // named 'sourceAttribName' of the 'source' FormattedVbo. The declared
    // vertex attribute also holds the name 'sourceAttribName'
    BufferObjectGeometry& wire(std::shared_ptr<FormattedVbo> source, const std::string& sourceAttribName);
    
    // Declares and maps all vertex attributes contained in the 'source' FormattedVbo.
    // Same names are used for each attribute.
    BufferObjectGeometry& wire(std::shared_ptr<FormattedVbo> source);
    
    // Set the index buffer to be used. Pass in a null shared_ptr to mean non-indexed. Default is non-indexed
    BufferObjectGeometry& indexedBy(std::shared_ptr<FormattedIbo> ib);
    
    // Same as indexBy(null shared_ptr)
    BufferObjectGeometry& noIndex();
    
    // Set the primitive type to draw using. Default is GL_TRIANGLES.
    // Anything you can pass to glDrawArrays is fair game
    BufferObjectGeometry& primitiveType(GLenum primitiveType);
    
    // Return if we are in indexed mode
    bool isIndexed() const {
        //return ib_;  //??此处注释掉，是因为不知其含义
        return ib_==NULL;
    }
    
    // Return the primitive types we are drawing using. Default is GL_TRIANGLES
    GLenum getPrimitiveType() const {
        return primitiveType_;
    }
    
    // Methods declared by Geometry
    virtual const std::vector<std::string>& getVertexAttribNames();
    virtual void draw(int attribIndices[]);
    
private:
    typedef std::map<std::string, std::pair<std::shared_ptr<FormattedVbo>, std::string> > Wiring;
    
    GLenum primitiveType_;
    bool wiringChanged_;
    Wiring wiring_;
    std::shared_ptr<FormattedIbo> ib_;
    
    // Internal struct for optimized vb binding order
    // 真正最后设置attribute pointer的依据结构
    struct PerVbWiring {
        // Use bare pointers since shared_ptrs are maintained by wiring_, hence
        // we do not need to worry about keeping it getting freed
        const FormattedVbo* vb;
        
        // A map from (attribute index in vb) --> relative index within BufferObjectGeometry's
        // exposed vertex attributes
        std::vector<std::pair<int, int> > vb2GeoIdx;
        
        PerVbWiring(const FormattedVbo* _vb) : vb(_vb) {}
    };
    
    std::vector<PerVbWiring> perVbWirings_;
    std::vector<std::string> vertexAttribNames_;
    
    // Setups up perVbWiring_ and vertexAttribNames_. Gets called whenever wiringChanged_ is true
    // and we need to draw or return list of vertex attributes.
    void processWiring();
};


// =============================================================================
// The BufferObjectGeometry is poweful and flexible, but for very simple cases,
// we also have a few predefined geometry implementations allowing
//   - some predefined vertex format
//   - index and non indexed rendering.
// These are based on the BufferObjectGeometry and demos its usage.
// =============================================================================


// First, some predefined vertex formats. It's perfectly easy for you to roll your own.
// Note that we define assignment operator (=) from GenericVertex
// of geometrymaker.h so that we can use geometrymaker on any of these format

// A vertex with floating point Position, and Normal;
struct VertexPN {
    Cvec3f p, n;
    
    //常量FORMAT为本struct对象定义范围内的存在
    static const VertexFormat FORMAT;
    
    VertexPN() {}
    
    VertexPN(float x, float y, float z,
             float nx, float ny, float nz)
    : p(x,y,z), n(nx, ny, nz) {}
    
    VertexPN(const Cvec3f& pos, const Cvec3f& normal)
    : p(pos), n(normal) {}
    
    VertexPN(const Cvec3& pos, const Cvec3& normal)
    : p(pos[0], pos[1], pos[2]), n(normal[0], normal[1], normal[2]) {}
    
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPN(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPN& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        return *this;
    }
    
};

//XCode快捷键Option+back删除词语，CMD+back删除整行
//VertexPNX是在VertexPN基础上的延伸定义
// A vertex with floating point Position, Normal, and one set of teXture Coordinates;
struct VertexPNX : public VertexPN {
    Cvec2f x; // texture coordinates
    
    //VertexPNX所属的常量FORMAT
    static const VertexFormat FORMAT;
    
    VertexPNX() {}
    
    VertexPNX(float x, float y, float z,
              float nx, float ny, float nz,
              float u, float v)
    : VertexPN(x, y, z, nx, ny, nz), x(u, v) {}
    
    VertexPNX(const Cvec3f& pos, const Cvec3f& normal, const Cvec2f& texCoords)
    : VertexPN(pos, normal), x(texCoords) {}
    
    VertexPNX(const Cvec3& pos, const Cvec3& normal, const Cvec2& texCoords)
    : VertexPN(pos, normal), x(texCoords[0], texCoords[1]) {}
    
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPNX(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPNX& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        x = v.tex;
        return *this;
    }
};

//在VertexPNX上继续延伸
// A vertex with floating point Position, Normal, Tangent, Binormal, teXture Coord
struct VertexPNTBX : public VertexPNX {
    Cvec3f t, b; // tangent, binormal
    
    //隶属于VertexPNTBX所配备的常量
    static const VertexFormat FORMAT;
    
    VertexPNTBX() {}
    
    VertexPNTBX(float x, float y, float z,
                float nx, float ny, float nz,
                float tx, float ty, float tz,
                float bx, float by, float bz,
                float u, float v)
    : VertexPNX(x, y, z, nx, ny, nz, u, v), t(tx, ty, tz), b(bx, by, bz) {}
    
    VertexPNTBX(const Cvec3f& pos, const Cvec3f& normal,
                const Cvec3f& tangent, const Cvec3f& binormal, const Cvec2f& texCoords)
    : VertexPNX(pos, normal, texCoords), t(tangent), b(binormal) {}
    
    VertexPNTBX(const Cvec3& pos, const Cvec3& normal,
                const Cvec3& tangent, const Cvec3& binormal, const Cvec2& texCoords)
    : VertexPNX(pos, normal, texCoords), t(tangent[0], tangent[1], tangent[2]), b(binormal[0], binormal[1], binormal[2]) {}
    
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    VertexPNTBX(const GenericVertex& v) {
        *this = v;
    }
    
    VertexPNTBX& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        t = v.tangent;
        b = v.binormal;
        x = v.tex;
        return *this;
    }
};

// Simple unindex geometry implementation based on BufferObjectGeometry
template<typename Vertex>
class SimpleUnindexedGeometry : public BufferObjectGeometry {
    std::shared_ptr<FormattedVbo> vbo;
public:
    SimpleUnindexedGeometry() : vbo(new FormattedVbo(Vertex::FORMAT)) {
        wire(vbo);
        primitiveType(GL_TRIANGLES);
    }
    
    //FORMAT变量用作BufferObjectGeometry的几何数据来源
    SimpleUnindexedGeometry(const Vertex* vertices, int numVertices)
    : vbo(new FormattedVbo(Vertex::FORMAT)) {
        wire(vbo);
        primitiveType(GL_TRIANGLES);
        upload(vertices, numVertices);
    }
    
    void upload(const Vertex* vertices, int numVertices) {
        vbo->upload(vertices, numVertices, true);
    }
};


// Simple Index geometry implementation based on BufferObjectGeometry
template<typename Vertex, typename Index>
class SimpleIndexedGeometry : public BufferObjectGeometry {
    std::shared_ptr<FormattedVbo> vbo;
    std::shared_ptr<FormattedIbo> ibo;
public:
    SimpleIndexedGeometry()
    : vbo(new FormattedVbo(Vertex::FORMAT)), ibo(new FormattedIbo(size2IboFmt(sizeof(Index)))) {
        wire(vbo);
        indexedBy(ibo);
        primitiveType(GL_TRIANGLES);
    }
    
    SimpleIndexedGeometry(const Vertex* vertices,  const Index* indices, int numVertices, int numIndices)
    : vbo(new FormattedVbo(Vertex::FORMAT)), ibo(new FormattedIbo(size2IboFmt(sizeof(Index)))) {
        wire(vbo);
        indexedBy(ibo);
        primitiveType(GL_TRIANGLES);
        upload(vertices, indices, numVertices, numIndices);
    }
    
    void upload(const Vertex* vertices, const Index* indices, int numVertices, int numIndices) {
        vbo->upload(vertices, numVertices, true);
        ibo->upload(indices, numIndices, true);
    }
    
private:
    GLenum size2IboFmt(int size) {
        if (size == 1)
            return GL_UNSIGNED_BYTE;
        if (size == 2)
            return GL_UNSIGNED_SHORT;
        if (size == 4)
            return GL_UNSIGNED_INT;
        assert(0);
        throw std::runtime_error("Invalid index buffer format supplied");
    }
};



typedef SimpleUnindexedGeometry<VertexPN> SimpleGeometryPN;
typedef SimpleUnindexedGeometry<VertexPNX> SimpleGeometryPNX;
typedef SimpleUnindexedGeometry<VertexPNTBX> SimpleGeometryPNTBX;

typedef SimpleIndexedGeometry<VertexPN, unsigned short> SimpleIndexedGeometryPN;
typedef SimpleIndexedGeometry<VertexPNX, unsigned short> SimpleIndexedGeometryPNX;
typedef SimpleIndexedGeometry<VertexPNTBX, unsigned short> SimpleIndexedGeometryPNTBX;

#endif
