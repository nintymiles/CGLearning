////////////////////////////////////////////////////////////////////////
//
//   Harvard University
//   CS175 : Computer Graphics
//   Professor Steven Gortler
//
////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#include "cvec.h"
#include "matrix4.h"
#include "geometrymaker.h"
#include "ppm.h"
#include "glsupport.h"
#include "FileWrapper.h"

#include "quat.h"
#include "rigtform.h"




using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////

// --------- IMPORTANT --------------------------------------------------------
// Before you start working on this assignment, set the following variable
// properly to indicate whether you want to use OpenGL 2.x with GLSL 1.0 or
// OpenGL 3.x+ with GLSL 1.3.
//
// Set g_Gl2Compatible = true to use GLSL 1.0 and g_Gl2Compatible = false to
// use GLSL 1.3. Make sure that your machine supports the version of GLSL you
// are using. In particular, on Mac OS X currently there is no way of using
// OpenGL 3.x with GLSL 1.3 when GLUT is used.
//
// If g_Gl2Compatible=true, shaders with -gl2 suffix will be loaded.
// If g_Gl2Compatible=false, shaders with -gl3 suffix will be loaded.
// To complete the assignment you only need to edit the shader files that get
// loaded
//
// Original assignment source designed for early OpenGL versions 1.0/1.3. Because
// We need to work on OpenGL ES enviroment,so remove g_GL2Compatible flag and related
// shader resources.
// ----------------------------------------------------------------------------
//static const bool g_Gl2Compatible = false;

static const float g_frustMinFov = 60.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -50.0;    // far plane

//地面相关的两个变量基本勾画出了地面的形状，位于y轴-2单位处，边长为20.
static const float g_groundY = -2.0;      // y coordinate of the ground 地面所处的y轴方位
//将地面尺寸定义为静态常量float类型，一半的地面宽度
static const float g_groundSize = 10.0;   // half the ground length

//窗口的宽高定位为static类型，作为全局缓存，可以随时存取
static int g_windowWidth = 512;
static int g_windowHeight = 512;

static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton;
static int g_mouseClickX, g_mouseClickY; // coordinates for mouse click event

//当前激活的shader pair
static int g_activeShader = 0;
/**
 ##### Log Program Info: #####
 WARNING: Output of vertex shader 'vNormal' not read by fragment shader
 WARNING: Output of vertex shader 'vPosition' not read by fragment shader
 出现这个日志说明了第二个shader成功加载
 */

//这个ShaderState结构封装了program，shader以及各种state数据，但是对于uniform和vertex是hard code的形式，所以耦合度很高
struct ShaderState {
    //利用CPP类的自然结构封装glProgram对象的生存周期管理
    GlProgram program;
    
    // Handles to uniform variables
    GLint h_uLight, h_uLight2;
    GLint h_uProjMatrix;
    GLint h_uModelViewMatrix;
    GLint h_uNormalMatrix;
    GLint h_uColor;
    GLint h_uXCoordOffset;
    
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
        h_uXCoordOffset = safe_glGetUniformLocation(h, "uXCoordOffset");
        
        // Retrieve handles to vertex attributes
        h_aPosition = safe_glGetAttribLocation(h, "aPosition");
        h_aNormal = safe_glGetAttribLocation(h, "aNormal");
        
        printProgramInfoLog(h);
        checkGlError(__FUNCTION__);
    }
    
};

//将shaders数量定义为静态const整型，shaders数量的变量声明类型及赋值
static const int g_numShaders = 2;
//将每对shader文件名定义为2维数组
static const char * const g_shaderFiles[g_numShaders][2] = {
    {"basic-gles3.vshader", "diffuse-gles3.fshader"},
    {"basic-gles3.vshader", "solid-gles3.fshader"}
};

//用vector对象存储ShaderState的共享指针,声明ShaderState的管理缓存
static vector<shared_ptr<ShaderState> > g_shaderStates; // our global shader states

// --------- Geometry

//相对偏移量macro，其原理是什么？利用struct实例的->操作符指向其成员变量的内存初始地址，这个是一种经验用法
//这个macro意味着以array of structures的方式组织vertex data,如此才能利用借助了CPP机制的这个macro
// Macro used to obtain relative offset of a field within a struct
#define FIELD_OFFSET(StructType, field) &(((StructType *)0)->field)

// A vertex with floating point position and normal
// VertexPN的初始化方式两种类型：一种从构造函数组数字部件方式构建，另一种则是利用特殊构造函数从GenericVertex结构中获取数据
struct VertexPN {
    Cvec3f p, n;
    
    VertexPN() {}
    VertexPN(float x, float y, float z,
             float nx, float ny, float nz)
    : p(x,y,z), n(nx, ny, nz)
    {}

    //--------------------------------------------------------------------------------
    //  从this地址处使用copy constructor从GenericVertex类型地址中提取数据
    //--------------------------------------------------------------------------------
    // Define copy constructor and assignment operator from GenericVertex so we can
    // use make* functions from geometrymaker.h
    
    //借助其它struct以及操作符“=”重载实现类型借用默认构造函数直接实现初始化
    VertexPN(const GenericVertex& v) {
        *this = v;
    }

    //重载操作符“=”，并且此次重载在构造函数中是生效的。重载的目的是对GenericVertex中的数据进行选择性的使用
    VertexPN& operator = (const GenericVertex& v) {
        p = v.pos;
        n = v.normal;
        return *this;
    }
};

//此处Geometry结构用于对具备相同的vertex数据组织结构及primitive mode的drawElements方式的统一绘制
//在此方式下，Geometry只需关心vertex和element index缓存数据的绑定及使用
//struct在cpp中的定义就是access control全为public的class
struct Geometry {
    //GLBufferObject的变量声明即是GL buffer object的生成，GLBufferObject负责vertex buffer的整个生命周期管理
    GlBufferObject vbo, ibo;
    //vbo及ibo的数量变量
    int vboLen, iboLen;
    
    Geometry(VertexPN *vtx, unsigned short *idx, int vboLen, int iboLen) {
        //设置vertex buffer缓存，以vboLen来设置buffer中的元素数目
        this->vboLen = vboLen;
        this->iboLen = iboLen;
        
        // Now create the VBO and IBO
        //绑定在成员变量vbo对象中生成的缓存对象
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //上传顶点缓存数据到GPU
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPN) * vboLen, vtx, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        //上传元素索引缓存数据到GPU
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * iboLen, idx, GL_STATIC_DRAW);
    }
    
    //整个Geometry的使用过程中对vertex buffer object只进行了绑定，并没有对应的解除绑定的动作，这是因为目前使用的绘制方式都是primitive相同的简单绘制，在不同的绘制中vertex attributes的数目也并未发生变化，故而不需要解除绑定。
    void draw(const ShaderState& curSS) {
        // Enable the attributes used by our shader
        //对shader中的特定vertex attribute开启vertex array访问方式
        safe_glEnableVertexAttribArray(curSS.h_aPosition);
        safe_glEnableVertexAttribArray(curSS.h_aNormal);
        
        // bind vbo
        //绑定已经就位的Vertex Array Buffer Object
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //设定顶点数据在buffer中的具体寻址方式
        //注意FIELD_OFFSET的使用方式，偏移量为VertexPN的p成员变量的相对偏移量？这个相对偏移量对每个VertexPN不知道地址的情形下的实现机制是什么？VertexPN -> p中操作符->的本质难道就是结构(类)的相对位置偏移量，这个是比较高级的使用方式。也即class/struct对象实例的内存存放时连续的，故而每个变量的访问的实质都是相对的地址（指针）偏移量
        safe_glVertexAttribPointer(curSS.h_aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, p));
        safe_glVertexAttribPointer(curSS.h_aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, n));
        
        // bind ibo
        //绑定element索引缓存
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        // draw!
        glDrawElements(GL_TRIANGLES, iboLen, GL_UNSIGNED_SHORT, 0);
        
        // Disable the attributes used by our shader
        //关闭vertex attributes的vertex array访问方式
        safe_glDisableVertexAttribArray(curSS.h_aPosition);
        safe_glDisableVertexAttribArray(curSS.h_aNormal);
    }
};


// Vertex buffer and index buffer associated with the ground and cube geometry
static shared_ptr<Geometry> g_ground, g_cube,g_sphere;

// --------- Scene

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space

//static Matrix4 g_skyRbt = Matrix4::makeTranslation(Cvec3(0.0, 0.25, 4.0));
static RigTForm g_skyRbt = RigTForm(Cvec3(0.0, 0.25, 4.0));
//static Matrix4 g_objectRbt[2] = {Matrix4::makeTranslation(Cvec3(0,0,0)),Matrix4::makeTranslation(Cvec3(0,0,-2))};
static RigTForm g_objectRbt[2] = {RigTForm(Cvec3(0,0,0)),RigTForm(Cvec3(0,0,-2))};

static Cvec3f g_objectColors[2] = {Cvec3f(1, 0, 0),Cvec3f(0.5, 0, 0.5)};

///////////////// END OF G L O B A L S //////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//  touch event variables
//--------------------------------------------------------------------------------
//记录touch事件当前的接触位置
GLfloat touch_location_x;
GLfloat touch_location_y;
//sphere rotation angle about cube
GLfloat rotation_angle;
GLfloat rotation_angle_alpha;
GLfloat rotation_angle_slerp_start=75.f;
GLfloat rotation_angle_slerp_end=255.f;
RigTForm g_slerpBaseRbt = RigTForm(Cvec3(0,0,-2));

//静态函数的意义？在函数返回类型前加关键字static，函数就定义成静态函数。函数的定义和生命在默认情况下都是extern的，但静态函数只是在声明他的文件当中可见，不能被其他文件所用
static void initGround() {
    // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
    VertexPN vtx[4] = {
        VertexPN(-g_groundSize, g_groundY, -g_groundSize, 0, 1, 0),
        VertexPN(-g_groundSize, g_groundY,  g_groundSize, 0, 1, 0),
        VertexPN( g_groundSize, g_groundY,  g_groundSize, 0, 1, 0),
        VertexPN( g_groundSize, g_groundY, -g_groundSize, 0, 1, 0),
    };
    unsigned short idx[] = {0, 1, 2, 0, 2, 3};
    g_ground.reset(new Geometry(&vtx[0], &idx[0], 4, 6));
}

static void initCubes() {
    int ibLen, vbLen;
    //首先通过函数方式确定vbLen和ibLen，适用于特定算法安排的方式
    getCubeVbIbLen(vbLen, ibLen);
    
    // Temporary storage for cube geometry
    //用于所生成cube集合数据的存储，存在于static函数中，其实例的存在性会比较持久？
    vector<VertexPN> vtx(vbLen);
    //unsigned short用于element indices很合适
    vector<unsigned short> idx(ibLen);
    
    //vtx和idx连续存储结构的初始地址被作为参数，用于存储cube数据
    makeCube(1, vtx.begin(), idx.begin());
    g_cube.reset(new Geometry(&vtx[0], &idx[0], vbLen, ibLen));
}

static void initSphere(){
    int ibLen, vbLen;
    int slices = 15;
    int stacks = 30;
    getSphereVbIbLen(slices, stacks, vbLen, ibLen);
    
    vector<VertexPN> vtx(vbLen);
    vector<unsigned short> idx(ibLen);
    
    makeSphere(1, slices, stacks, vtx.begin(), idx.begin());
    g_sphere.reset(new Geometry(&vtx[0], &idx[0], vbLen, ibLen));
}

// takes a projection matrix and send to the the shaders
static void sendProjectionMatrix(const ShaderState& curSS, const Matrix4& projMatrix) {
    //实际往shader中传递的矩阵格式，一元float数组，所有矩阵的基类型都是GLfloat
    GLfloat glmatrix[16];
    
    //对矩阵缩放的测试代码，注意用完之后删除
    //    Matrix4 scaleMatrix =Matrix4::makeScale(Cvec3(3,3,3));
    //    scaleMatrix(3,3) = 1.0;
    //    Matrix4 contractMatrix = Matrix4::makeScale(Cvec3(1,1,1));
    //    contractMatrix(3,3) = 1.0;
    //    Matrix4 projTMatrix =  scaleMatrix * contractMatrix * projMatrix  ;
    
    //    projTMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
    
    //使用之前将Matrix4中的数据按照列为主方式写入一元数组
    projMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
    //以uniform方式设置到shader中
    safe_glUniformMatrix4fv(curSS.h_uProjMatrix, glmatrix);
}

// takes MVM and its normal matrix to the shaders
static void sendModelViewNormalMatrix(const ShaderState& curSS, const Matrix4& MVM, const Matrix4& NMVM) {
    GLfloat glmatrix[16];
    
    //将用于运算的model view matrix转化为shader ready的格式，然后上传到shader中
    MVM.writeToColumnMajorMatrix(glmatrix); // send MVM
    safe_glUniformMatrix4fv(curSS.h_uModelViewMatrix, glmatrix);
    
    //normal matrix from model view matrix
    NMVM.writeToColumnMajorMatrix(glmatrix); // send NMVM
    safe_glUniformMatrix4fv(curSS.h_uNormalMatrix, glmatrix);
}

// update g_frustFovY from g_frustMinFov, g_windowWidth, and g_windowHeight
//根据screen ratio的特点调整最佳默认FOV值
static void updateFrustFovY() {
    if (g_windowWidth >= g_windowHeight)
        g_frustFovY = g_frustMinFov;
    else {
        //此处的RAD_PER_DEG的名称应该是HALF_RAD_PER_DEG,因为只是为了符合实际FOV应用时在场景中一般使用一半角度。
        const double RAD_PER_DEG = 0.5 * CS175_PI/180;
        //此处计算的含义？sin(θ)和cos(θ)可以分别作为y和x值，同时在sin(θ)上应用screen ratio（windowHeight>windowWidth),则意味着追求最大的y轴正常FOV（不放大和缩小）。这个update等效于在make projectionMatrix时在frust matrix上针对screen ratio进行的因子设置。
        g_frustFovY = atan2(sin(g_frustMinFov * RAD_PER_DEG) * g_windowHeight / g_windowWidth, cos(g_frustMinFov * RAD_PER_DEG)) / RAD_PER_DEG;
    }
}

static Matrix4 makeProjectionMatrix() {
    return Matrix4::makeProjection(
           g_frustFovY, g_windowWidth / static_cast <double> (g_windowHeight),
           g_frustNear, g_frustFar);
}

static void drawStuff() {
    // short hand for current shader state
    const ShaderState& curSS = *g_shaderStates[g_activeShader];
    
    // build & send proj. matrix to vshader
    const Matrix4 projmat = makeProjectionMatrix();
    sendProjectionMatrix(curSS, projmat);
    
    // use the skyRbt as the eyeRbt
    //const Matrix4 eyeRbt = g_skyRbt;
    const RigTForm eyeRbt = g_skyRbt;
    //const Matrix4 invEyeRbt = inv(eyeRbt);
    const RigTForm invEyeRbt = inv(eyeRbt);
    
    const Cvec3 eyeLight1 = Cvec3(invEyeRbt * Cvec4(g_light1, 1)); // g_light1 position in eye coordinates
    const Cvec3 eyeLight2 = Cvec3(invEyeRbt * Cvec4(g_light2, 1)); // g_light2 position in eye coordinates
    safe_glUniform3f(curSS.h_uLight, eyeLight1[0], eyeLight1[1], eyeLight1[2]);
    safe_glUniform3f(curSS.h_uLight2, eyeLight2[0], eyeLight2[1], eyeLight2[2]);
    
    // draw ground
    // ===========
    //
    //const Matrix4 groundRbt = Matrix4();  // identity
    //ground object frame即world frame
    const RigTForm groundRbt = RigTForm::identity();
    //Matrix4 MVM = invEyeRbt * groundRbt;
    Matrix4 MVM = rigTFormToMatrix(invEyeRbt * groundRbt);
    //从模型试图矩阵中提取出normal矩阵
    Matrix4 NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, 0.1, 0.95, 0.1); // set color
    g_ground->draw(curSS);
    
    // draw cube
    // ==========
    safe_glUniform1f(curSS.h_uXCoordOffset, 1.5f);
    //MVM = invEyeRbt * g_objectRbt[0];
    MVM = rigTFormToMatrix(invEyeRbt * g_objectRbt[0]);
    NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, g_objectColors[0][0], g_objectColors[0][1], g_objectColors[0][2]);
    g_cube->draw(curSS);
    
    safe_glUniform1f(curSS.h_uXCoordOffset, -1.5f);
    //MVM = invEyeRbt * g_objectRbt[0];
    MVM = rigTFormToMatrix(invEyeRbt * g_objectRbt[0]);
    NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    //safe_glUniform3f(curSS.h_uColor, g_objectColors[0][0], g_objectColors[0][1], g_objectColors[0][2]);
    g_cube->draw(curSS);
    
    // draw spere
    // ==========
    //MVM = invEyeRbt * g_objectRbt[1];
    MVM = rigTFormToMatrix(invEyeRbt * g_objectRbt[1]);
    NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, g_objectColors[1][0], g_objectColors[1][1], g_objectColors[1][2]);
    g_sphere->draw(curSS);
}

static void display() {
    glUseProgram(g_shaderStates[g_activeShader]->program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                   // clear framebuffer color&depth
    
    drawStuff();
    
    //iOS automatically control swapping buffer
    //no need to swap buffer manually
    checkGlError(__FUNCTION__);
}

static void reshape(const int w, const int h) {
    g_windowWidth = w;
    g_windowHeight = h;
    glViewport(0, 0, w, h);
    cerr << "Size of window is now " << w << "x" << h << endl;
    updateFrustFovY();
    
}

static void motion(const float x, const float y) {
    const double dx = x - g_mouseClickX;
    const double dy = g_windowHeight - y - 1 - g_mouseClickY;
    
    //Matrix4 m;
    RigTForm m;
    if (g_mouseLClickButton && !g_mouseRClickButton) { // left button down?
        //m = Matrix4::makeXRotation(-dy) * Matrix4::makeYRotation(dx);
        m = RigTForm::makeXRotation(-dy) * RigTForm::makeYRotation(dx);
    }
    else if (g_mouseRClickButton && !g_mouseLClickButton) { // right button down?
        //m = Matrix4::makeTranslation(Cvec3(dx, dy, 0) * 0.01);
        m = RigTForm(Cvec3(dx, dy, 0) * 0.01);
    }
    else if (g_mouseMClickButton || (g_mouseLClickButton && g_mouseRClickButton)) {  // middle or (left and right) button down?
        //m = Matrix4::makeTranslation(Cvec3(0, 0, -dy) * 0.01);
        m = RigTForm(Cvec3(0, 0, -dy) * 0.01);
    }
    
    if (g_mouseClickDown) {
        g_objectRbt[0] = g_objectRbt[0] * m; // Simply right-multiply is WRONG
    }
    
    //旋转角度的匀速累加是通过每次施加一个固定角度的变换，而以角度值累加的方式进行选择，则意味着，每次都要从初始点进行变换。
//    rotation_angle += .02f;
//    if(rotation_angle >= 360.f)
//        rotation_angle = 0.f;
    
//    GLfloat angular_value_per_time=3.f;
//    Matrix4 eyeRbt = g_skyRbt;
//    Matrix4 aRbt = makeMixedFrame(g_objectRbt[0],eyeRbt);
//    //如果要一直围绕原点运动，则在3个轴的方向都要保持匀速的相同角度值的同步运动
//
//    Matrix4 q1RbtByQuat = quatToMatrix(Quat::makeYRotation(angular_value_per_time));
//    Matrix4 q1Rbt = Matrix4::makeYRotation(angular_value_per_time) ;
//    Matrix4 q2Rbt = Matrix4::makeXRotation(angular_value_per_time) ;
//    Matrix4 q3Rbt = Matrix4::makeZRotation(angular_value_per_time) ;
//    //针对auxiliary frame施加固定角度的旋转值
//    g_objectRbt[1] = doQtoOwrtA(q1RbtByQuat*q2Rbt*q3Rbt, g_objectRbt[1], aRbt);
    
//    GLfloat angular_value_per_time=3.f;
//    RigTForm eyeRbt = g_skyRbt;
//    RigTForm aRbt = makeMixedFrame(g_objectRbt[0],eyeRbt);
//    //如果要一直围绕原点运动，则在3个轴的方向都要保持匀速的相同角度值的同步运动
//
//    RigTForm q1Rbt = RigTForm::makeYRotation(angular_value_per_time) ;
//    RigTForm q2Rbt = RigTForm::makeXRotation(angular_value_per_time) ;
//    RigTForm q3Rbt = RigTForm::makeZRotation(angular_value_per_time) ;
//    //针对auxiliary frame施加固定角度的旋转值
//    g_objectRbt[1] = doQtoOwrtA(q1Rbt*q2Rbt*q3Rbt, g_objectRbt[1], aRbt);
  
    
    //--------------------------------------------------------------------------------
    //  Slerp between rotation 1(rotate 5 degree about x axis) and rotation 2(rotate 155 degree about x axis)
    //--------------------------------------------------------------------------------
    rotation_angle_alpha += .02f;
    if(rotation_angle_alpha >= 1.f)
        rotation_angle_alpha = 0.f;
    
    RigTForm eyeRbt = g_skyRbt;
    RigTForm aRbt = makeMixedFrame(g_objectRbt[0],eyeRbt);
    
    Quat q1Quat = Quat::makeYRotation(rotation_angle_slerp_start) ;
    Quat q2Quat = Quat::makeYRotation(rotation_angle_slerp_end) ;
    //examine orientation interoperation by slerp and lerp
    Quat slerpQuat = slerp(q1Quat, q2Quat, rotation_angle_alpha);
    //若要插值动画循环播放，每次施加在base object frame（g_slerpBaseRbt)
    g_objectRbt[1] = doQtoOwrtA(RigTForm(slerpQuat), g_slerpBaseRbt, aRbt);
    
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;
}


static void mouse(const float x, const float y,u_long tapCount,bool press) {
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;  // conversion from window-coordinate-system to OpenGL window-coordinate-system
    
    //every time initialze all mouse status to false
    g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=false;
    
    g_mouseLClickButton |= (press && tapCount == 1);
    g_mouseRClickButton |= (press && tapCount == 2);
    g_mouseMClickButton |= (press && tapCount == 3);
    
//    g_mouseLClickButton &= !(!press && tapCount == 1);
//    g_mouseRClickButton &= !(!press && tapCount == 2);
//    g_mouseMClickButton &= !(!press && tapCount == 3);
    
    
    g_mouseClickDown = g_mouseLClickButton || g_mouseRClickButton || g_mouseMClickButton;
}

//static void keyboard(const unsigned char key, const int x, const int y) {
//  switch (key) {
//  case 27:
//    exit(0);                                  // ESC
//  case 'h':
//    cout << " ============== H E L P ==============\n\n"
//    << "h\t\thelp menu\n"
//    << "s\t\tsave screenshot\n"
//    << "f\t\tToggle flat shading on/off.\n"
//    << "o\t\tCycle object to edit\n"
//    << "v\t\tCycle view\n"
//    << "drag left mouse to rotate\n" << endl;
//    break;
//  case 's':
//    glFlush();
//    writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
//    break;
//  case 'f':
//    g_activeShader ^= 1;
//    break;
//  }
//}


static void initGLState() {
    //设置color buffer被清理后的默认颜色值
    glClearColor(128./255., 200./255., 255./255., 0.);
    //重置depth buffer默认值为0.f，一般情况下这个值是窗口z值的最小值，这说明z的范围为[0..1]
    glClearDepthf(0.f);
    
    //指定纹理pixel数据压缩和解压缩的对其方式
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    
    //启用面部剔除，状态集中设置时没有顺序之分，状态值集中管理
    glEnable(GL_CULL_FACE);
    
    //剔除背向面三角形
    glCullFace(GL_BACK);
    
    //启用深度缓存检测
    glEnable(GL_DEPTH_TEST);
    //深度缓存比较方式的函数设置，在本程序的矩阵计算体系上，若不设置此参数，则无法执行深度相关的预算，导致z-buffer紊乱，无法正常渲染到FBO中。
    glDepthFunc(GL_GREATER);
    
    //glReadBuffer(GL_BACK);
    
}

static void initShaders() {
  g_shaderStates.resize(g_numShaders);
  for (int i = 0; i < g_numShaders; ++i) {
//    if (g_Gl2Compatible)
//      g_shaderStates[i].reset(new ShaderState(GetBundleFileName(g_shaderFilesGl2[i][0]), GetBundleFileName(g_shaderFilesGl2[i][1])));
//    else
      g_shaderStates[i].reset(new ShaderState(GetBundleFileName(g_shaderFiles[i][0]), GetBundleFileName(g_shaderFiles[i][1])));
  }
}

static void initGeometry() {
    initGround();
    initCubes();
    initSphere();
}

bool GraphicsInit()
{
    //printOpenGLESInfo();
    initGLState();
    initShaders();
    initGeometry();
    
    return true;
}

bool GraphicsResize( int width, int height )
{
    //    g_windowWidth = width;
    //    g_windowHeight = height;
    //    glViewport(0, 0, width, height);
    reshape(width, height);
    
    return true;
}

//--------------------------------------------------------------------------------
//  redraw scene per frame
//--------------------------------------------------------------------------------
bool GraphicsRender() {
    try {
        display();
        motion(touch_location_x, touch_location_y);
        
        return 0;
    }
    catch (const runtime_error& e) {
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
}

void TouchEventDown( float x, float y,unsigned long tapCount,bool pressStatus )
{
    touch_location_x = x;
    touch_location_y = y;
    
    mouse(x,y,tapCount,pressStatus);
}

void TouchEventMove( float x, float y )
{
    touch_location_x = x;
    touch_location_y = y;
}

void TouchEventRelease( float x, float y,unsigned long tapCount,bool pressStatus )
{
    //todo
    if(tapCount == 4){
        g_activeShader ^= 1; //g_activeShader变量和1相同则设置为0，不同则设置为1
    }
    //--------------------------------------------------------------------------------
    //  after touch moving event，touch up event doesn't have correcsponding touch down event tapCount
    //--------------------------------------------------------------------------------
    //mouse(x,y,tapCount,pressStatus);
    if(tapCount == 5){
        writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
    }
}
