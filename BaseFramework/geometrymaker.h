#ifndef GEOMETRYMAKER_H
#define GEOMETRYMAKER_H

#include <cmath>

#include "cvec.h"

//--------------------------------------------------------------------------------
// Helpers for creating some special geometries such as plane, cubes, and spheres
//--------------------------------------------------------------------------------


// A generic vertex structure containing position, normal, and texture information
// Used by make* functions to pass vertex information to the caller

//通用顶点数据类型GenericVertex，用于一次性封装顶点相关的所有可能数据项目
//GenericVertex可被用于有选择的两次vertex data封装
struct GenericVertex {
    Cvec3f pos;
    Cvec3f normal;
    Cvec2f tex;
    Cvec3f tangent, binormal;
    
    //结构体成员变量的初始化借助cpp的结构构造函数的特定语法来进行
    //在构造函数体后直接使用子构造函数初始化成员变量
    GenericVertex(
                  float x, float y, float z,
                  float nx, float ny, float nz,
                  float tu, float tv,
                  float tx, float ty, float tz,
                  float bx, float by, float bz)
    : pos(x,y,z), normal(nx,ny,nz), tex(tu, tv), tangent(tx, ty, tz), binormal(bx, by, bz)
    {}
};

//vbLen,ibLen只是被传入参数的reference，referece可用来对原来的变量进行值的改变
//相当于参数值是inout方式的，可以被函数修改并保留修改的值
inline void getPlaneVbIbLen(int& vbLen, int& ibLen) {
  vbLen = 4;
  ibLen = 6;
}

//function template
//模版参数暗示接受数组地址作为参数
template<typename VtxOutIter, typename IdxOutIter>
void makePlane(float size, VtxOutIter vtxIter, IdxOutIter idxIter) {
    float h = size / 2.0;
    
    //每一次赋值都将入参的数组指针顺延加1
    *vtxIter = GenericVertex(    -h, 0, -h, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, -1);
    *(++vtxIter) = GenericVertex(-h, 0,  h, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, -1);
    *(++vtxIter) = GenericVertex( h, 0,  h, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, -1);
    *(++vtxIter) = GenericVertex( h, 0, -h, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, -1);
    
    //索引值从0开始算起
    *idxIter = 0;
    *(++idxIter) = 1;
    *(++idxIter) = 2;
    *(++idxIter) = 0;
    *(++idxIter) = 2;
    
    *(++idxIter) = 3;
}

inline void getCubeVbIbLen(int& vbLen, int& ibLen) {
  vbLen = 24;
  ibLen = 36;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeCube(float size, VtxOutIter vtxIter, IdxOutIter idxIter) {
    float h = size / 2.0;

    //--------------------------------------------------------------------------------
    //  定义一个在local生效的macro，通过#define/#undef指令达成目的
    //  --structure的构造函数返回structure地址
    // macro在函数编译时进行替换，但本地macro的执行时机？
    //--------------------------------------------------------------------------------
#define DEFV(x, y, z, nx, ny, nz, tu, tv) { \
    *vtxIter = GenericVertex(x h, y h, z h, \
                             nx, ny, nz, tu, tv, \
                             tan[0], tan[1], tan[2], \
                             bin[0], bin[1], bin[2]); \
    ++vtxIter; \
}
    //此本地macro居然借助了即时的本地变量声明，这也意味着这些macro和本地变量的生存周期是同步的，否则这个macro不成立
    //此处关键：本地macro，本地即时变量之间的交互
    Cvec3f tan(0, 1, 0), bin(0, 0, 1);
    DEFV(+, -, -, 1, 0, 0, 0, 0); // facing +X
    DEFV(+, +, -, 1, 0, 0, 1, 0);
    DEFV(+, +, +, 1, 0, 0, 1, 1);
    DEFV(+, -, +, 1, 0, 0, 0, 1);
    
    tan = Cvec3f(0, 0, 1);
    bin = Cvec3f(0, 1, 0);
    DEFV(-, -, -, -1, 0, 0, 0, 0); // facing -X
    DEFV(-, -, +, -1, 0, 0, 1, 0);
    DEFV(-, +, +, -1, 0, 0, 1, 1);
    DEFV(-, +, -, -1, 0, 0, 0, 1);
    
    tan = Cvec3f(0, 0, 1);
    bin = Cvec3f(1, 0, 0);
    DEFV(-, +, -, 0, 1, 0, 0, 0); // facing +Y
    DEFV(-, +, +, 0, 1, 0, 1, 0);
    DEFV(+, +, +, 0, 1, 0, 1, 1);
    DEFV(+, +, -, 0, 1, 0, 0, 1);
    
    tan = Cvec3f(1, 0, 0);
    bin = Cvec3f(0, 0, 1);
    DEFV(-, -, -, 0, -1, 0, 0, 0); // facing -Y
    DEFV(+, -, -, 0, -1, 0, 1, 0);
    DEFV(+, -, +, 0, -1, 0, 1, 1);
    DEFV(-, -, +, 0, -1, 0, 0, 1);
    
    tan = Cvec3f(1, 0, 0);
    bin = Cvec3f(0, 1, 0);
    DEFV(-, -, +, 0, 0, 1, 0, 0); // facing +Z
    DEFV(+, -, +, 0, 0, 1, 1, 0);
    DEFV(+, +, +, 0, 0, 1, 1, 1);
    DEFV(-, +, +, 0, 0, 1, 0, 1);
    
    tan = Cvec3f(0, 1, 0);
    bin = Cvec3f(1, 0, 0);
    DEFV(-, -, -, 0, 0, -1, 0, 0); // facing -Z
    DEFV(-, +, -, 0, 0, -1, 1, 0);
    DEFV(+, +, -, 0, 0, -1, 1, 1);
    DEFV(+, -, -, 0, 0, -1, 0, 1);
#undef DEFV

    //按照GL_TRIANGLES方式生成顶点索引，每四个连续点生成两个三角形的六个顶点索引。
    for (int v = 0; v < 24; v +=4) {
        *idxIter = v;
        *++idxIter = v + 1;
        *++idxIter = v + 2;
        *++idxIter = v;
        *++idxIter = v + 2;
        *++idxIter = v + 3;
        ++idxIter;
    }
}

inline void getSphereVbIbLen(int slices, int stacks, int& vbLen, int& ibLen) {
  assert(slices > 1);
  assert(stacks >= 2);
  vbLen = (slices + 1) * (stacks + 1);
  ibLen = slices * stacks * 6;
}

template<typename VtxOutIter, typename IdxOutIter>
void makeSphere(float radius, int slices, int stacks, VtxOutIter vtxIter, IdxOutIter idxIter) {
    using namespace std;
    assert(slices > 1);
    assert(stacks >= 2);
    
    const double radPerSlice = 2 * CS175_PI / slices;
    const double radPerStack = CS175_PI / stacks;
    
    vector<double> longSin(slices+1), longCos(slices+1);
    vector<double> latSin(stacks+1), latCos(stacks+1);
    for (int i = 0; i < slices + 1; ++i) {
        longSin[i] = sin(radPerSlice * i);
        longCos[i] = cos(radPerSlice * i);
    }
    for (int i = 0; i < stacks + 1; ++i) {
        latSin[i] = sin(radPerStack * i);
        latCos[i] = cos(radPerStack * i);
    }
    
    for (int i = 0; i < slices + 1; ++i) {
        for (int j = 0; j < stacks + 1; ++j) {
            float x = longCos[i] * latSin[j];
            float y = longSin[i] * latSin[j];
            float z = latCos[j];
            
            Cvec3f n(x, y, z);
            Cvec3f t(-longSin[i], longCos[i], 0);
            Cvec3f b = cross(n, t);
            
            *vtxIter = GenericVertex(
                                     x * radius, y * radius, z * radius,
                                     x, y, z,
                                     1.0/slices*i, 1.0/stacks*j,
                                     t[0], t[1], t[2],
                                     b[0], b[1], b[2]);
            ++vtxIter;
            
            //every two adjacent points in a longitude pair with two adjacent right below points.
            if (i < slices && j < stacks ) {
                *idxIter = (stacks+1) * i + j;
                *++idxIter = (stacks+1) * i + j + 1;
                *++idxIter = (stacks+1) * (i + 1) + j + 1;
                
                *++idxIter = (stacks+1) * i + j;
                *++idxIter = (stacks+1) * (i + 1) + j + 1;
                *++idxIter = (stacks+1) * (i + 1) + j;
                ++idxIter;
            }
        }
    }
}


#endif
