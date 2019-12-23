#include <stdexcept>
#include <string>
#include <cstddef>

#include "geometry.h"

using namespace std;

//基础知识点--struct中const常量定义的存在范围？
//注意VertexPN::FORMAT这种const在cpp中的和定义分离的特殊初始化时机和形式
//注意对struct结构中变量内存访问的这种便捷支持offsetof(VertexPN, p))
//对头文件中struct中的const FORMAT变量进行初始化赋值，并且这个常量的范围存在于对应的定义范围
//
const VertexFormat VertexPN::FORMAT = VertexFormat(sizeof(VertexPN))
.put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPN, p))
.put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPN, n));

const VertexFormat VertexPNX::FORMAT = VertexFormat(sizeof(VertexPNX))
.put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, p))
.put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, n))
.put("aTexCoord", 2, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, x));

const VertexFormat VertexPNTBX::FORMAT = VertexFormat(sizeof(VertexPNTBX))
.put("aPosition", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, p))
.put("aNormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, n))
.put("aTangent", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNTBX, t))
.put("aBinormal", 3, GL_FLOAT, GL_FALSE, offsetof(VertexPNTBX, b))
.put("aTexCoord", 2, GL_FLOAT, GL_FALSE, offsetof(VertexPNX, x));


BufferObjectGeometry::BufferObjectGeometry()
    : wiringChanged_(true),primitiveType_(GL_TRIANGLES){}

//用指定的targetAttributeName绑定source VBO中特定的sourceAttributeName
BufferObjectGeometry& BufferObjectGeometry::wire(
                                                 const string& targetAttribName,
                                                 shared_ptr<FormattedVbo> source,
                                                 const string& sourceAttribName) {
    wiringChanged_ = true;
    wiring_[targetAttribName] = make_pair(source, sourceAttribName);
    return *this;
}

//绑定source VBO种指定的sourceAttributeName
BufferObjectGeometry& BufferObjectGeometry::wire(shared_ptr<FormattedVbo> source, const string& sourceAttribName) {
    return wire(sourceAttribName, source, sourceAttribName);
}

//绑定source VBO种所有attribute name。
BufferObjectGeometry& BufferObjectGeometry::wire(shared_ptr<FormattedVbo> source) {
    const VertexFormat& vfd = source->getVertexFormat();
    for (int i = 0, n = vfd.getNumAttribs(); i < n; ++i) {
        wire(source, vfd.getAttrib(i).name);
    }
    return *this;
}

//索引化绘制参数指定
BufferObjectGeometry& BufferObjectGeometry::indexedBy(shared_ptr<FormattedIbo> ib) {
    ib_ = ib;
    return *this;
}

BufferObjectGeometry& BufferObjectGeometry::primitiveType(GLenum primitiveType) {
    switch (primitiveType) {
        case GL_POINTS:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
        case GL_LINES:
        case GL_TRIANGLE_STRIP:
        case GL_TRIANGLE_FAN:
        case GL_TRIANGLES:
        case GL_QUAD_STRIP:  //QUAD、POLYGON支持
        case GL_QUADS:
        case GL_POLYGON:
            break;
        default:
            assert(0);
    }
    primitiveType_ = primitiveType;
    return *this;
}

const vector<string>& BufferObjectGeometry::getVertexAttribNames() {
    if (wiringChanged_)
        processWiring();
    return vertexAttribNames_;
}

//attribIndices[]中的数据从glProgram中由attributesDesc所维护的数据处理而得
void BufferObjectGeometry::draw(int attribIndices[]) {
    if (wiringChanged_)
        processWiring();
    
    //设置常量值未定义的VBO长度（UNDEFINED_VB_LEN）为最大的无符号整型值？
    const unsigned int UNDEFINED_VB_LEN = 0xFFFFFFFF;
    unsigned int vboLen = UNDEFINED_VB_LEN;
    
    //注：此处若不使用VAO对象，则无法进行绘制。（原来的assignment框架没有使用VAO）
    GLuint vaoId;
    glGenVertexArrays(1,&vaoId);
    glBindVertexArray(vaoId);
    
    vector<GLuint> vtxLoc;
    vector<GLuint> vboBufferIds;
    
    //    for (size_t i = 0; i < numAttribs; ++i) {
    //        //只要有对应的属性Index，则开启vertex array方式
    //        if (attribIndices[i] >= 0)
    //            glEnableVertexAttribArray(attribIndices[i]);
    //        checkGlError("after glEnableVertexAttribArray");
    //        printProgramInfoLog(programDesc_->program);
    //    }
    
    // bind the vertex buffer and set vertex attribute pointers
    for (size_t i = 0, n = perVbWirings_.size(); i < n; ++i) {
        const PerVbWiring &pvw = perVbWirings_[i];
        const VertexFormat& vfd = pvw.vb->getVertexFormat();
        
        //每次绑定一个VBO，然后设置vertexattribpointer，这意味着可以支持多缓存共用。
        //但是这里的问题是，每次绑定一个属性，就要绑定一次VBO缓存，API overhead可以减少？
        //这里意图保证每次绘制调用对相同的VBO只绑定一次，程序简陋，需要再尝试能否改进。
        //这里的改进是否值得？
        bool isVboIdExist = false;
        for(auto vboId:vboBufferIds){
            if(vboId==*(pvw.vb)){
                isVboIdExist = true;
                break;
            }
        }
        
        if(!isVboIdExist){
           //glBindBuffer(GL_ARRAY_BUFFER, *(pvw.vb));
            vboBufferIds.push_back(*(pvw.vb));
            glBindBuffer(GL_ARRAY_BUFFER, *(pvw.vb));
        }
        
        
        
        /** 通过mapperBuffer的方式验证VertexBuffer中的数据并没有问题
         
         GLfloat *vtxMappedBuf;
         
         GLfloat vtxBuf[56] ;
         
         GLint bufferSize = pvw.vb->length() * (pvw.vb->getVertexFormat().getVertexSize());
         vtxMappedBuf = (GLfloat*) glMapBufferRange ( GL_ARRAY_BUFFER, 0, (unsigned int)bufferSize , GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
         if ( vtxMappedBuf == NULL ) {
         printf( "Error mapping vertex buffer object." );
         }
         
         memcpy (  vtxBuf,vtxMappedBuf, bufferSize );
         */
        
        vboLen = min(vboLen, (unsigned int)pvw.vb->length());
        
        for (size_t j = 0; j < pvw.vb2GeoIdx.size(); ++j) {
            int loc = attribIndices[pvw.vb2GeoIdx[j].second];
            if (loc >= 0){
                glEnableVertexAttribArray(loc);
                
                vfd.setGlVertexAttribPointer(pvw.vb2GeoIdx[j].first, loc);
                //put vertex attribute location into vtxLoc such that
                //we can disable vertex array respectively
                vtxLoc.push_back(loc);
            }
            
        }
    }
    
    //    glBindVertexArray(0);
    //
    //    glBindVertexArray(vaoId);
    
    if (isIndexed()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ib_);
        glDrawElements(primitiveType_, ib_->length(), ib_->getIndexFormat(), 0);
    }
    else if (vboLen != UNDEFINED_VB_LEN) { //此处坚持vboLen是否超过最大允许值
        glDrawArrays(primitiveType_, 0, vboLen);
    }
    
    //绘制完成后，禁用对应的Vertex Attribute Location的vertex array模式
    for(GLuint i:vtxLoc){
        glDisableVertexAttribArray(i);
    }

    //清理保存的vertex attribute location
    vtxLoc.clear();
    //clear saved bound vboBufferIds
    vboBufferIds.clear();
    
    glBindVertexArray(0);
}

void BufferObjectGeometry::processWiring() {
    //处理属性关联之前先清理缓存
    perVbWirings_.clear();
    vertexAttribNames_.clear();
    
    // maps from target vbo to index within perVbWirings_
    map<shared_ptr<FormattedVbo>, int> vbIdx;
    
    // go through all wiring definitions
    for (Wiring::const_iterator i = wiring_.begin(), e = wiring_.end(); i != e; ++i) {
        shared_ptr<FormattedVbo> vb = i->second.first; // target vbo
        const VertexFormat& vfd = vb->getVertexFormat(); // target vbo's format
        
        // see if this vbo is already in our vbIdx map
        map<shared_ptr<FormattedVbo>, int>::iterator j = vbIdx.find(vb);
        int idx = 0; // idx of vbo in perVbWiring_, to be set
        if (j == vbIdx.end()) {
            //将size_t类型强制转型为int类型使用
            idx = (int)perVbWirings_.size();
            vbIdx[vb] = idx;
            perVbWirings_.push_back(PerVbWiring(vb.get()));
        }
        else {
            idx = j->second;
        }
        const int globalIdx = (int)vertexAttribNames_.size(); // idx within vertexAttribNames_
        
        perVbWirings_[idx].vb2GeoIdx.push_back(make_pair(vfd.getAttribIndexForName(i->second.second), globalIdx));
        
        vertexAttribNames_.push_back(i->first);
    }
    wiringChanged_ = false;
}
