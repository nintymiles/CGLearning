//
//  main.cpp
//  Assignment5-RobotsAndPartPicking
//
//  Created by SeanRen on 2019/3/12.
//  Copyright © 2019 zxtech. All rights reserved.
//
////////////////////////////////////////////////////////////////////////
//
//   Harvard University
//   CS175 : Computer Graphics
//   Professor Steven Gortler
//
////////////////////////////////////////////////////////////////////////
/*****************************************************************************
 * Original source template from book source site based on GLUT
 * Modified to use GLFW as a window manager by SeanRen
 * Corresponding codes just tested on Mac OS X and iOS
 *****************************************************************************/

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <dirent.h>


//--------------------------------------------------------------------------------
//  GLEW的作用就是将合适版本的OpenGL版本对应的头文件引入，以方便调用对应的OpenGL功能。若不想使用glew
//  则在对应OS上引入合适的OpenGL头文件即可
//--------------------------------------------------------------------------------
#include <GL/glew.h>
//#include <OpenGL/gl3.h>
//#include <OpenGL/gl3ext.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include <list>

#include "cvec.h"
#include "matrix4.h"
#include "quat.h"
#include "rigtform.h"
#include "geometrymaker.h"
#include "arcball.h"
#include "ppm.h"
#include "glsupport.h"
#include "asstcommon.h"
#include "scenegraph.h"
#include "drawer.h"
#include "picker.h"

#include "sgutils.h"


#include "perfMonitor.h"

static void restoreStatusToScenegraph();
static void updateScenegraphStatusDataToKeyFrameList();


using namespace std;      // for string, vector, iostream, and other standard C++ stuff
static void pick();

// G L O B A L S ///////////////////////////////////////////////////

static const float g_frustMinFov = 60.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -50.0;    // far plane
static const float g_groundY = -2.0;      // y coordinate of the ground
static const float g_groundSize = 10.0;   // half the ground length

static int g_windowWidth = 1024;
static int g_windowHeight = 768;
static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton;
static int g_mouseClickX, g_mouseClickY,g_pickingMouseX,g_pickingMouseY; // coordinates for mouse click event
static int g_activeShader = 1;
static int g_activeCube = 0;
static bool g_pickingFlag = false;
static int g_activeEyeFrame = 1;

static GLFWwindow* window;

static const int PICKING_SHADER = 2; // index of the picking shader is g_shaerFiles
static const int g_numShaders = 3; // 3 shaders instead of 2
static const char * const g_shaderFiles[g_numShaders][2] = {
    {"./shaders/basic-gl3.vshader", "./shaders/solid-gl3.fshader"},
    {"./shaders/basic-gl3.vshader", "./shaders/diffuse-gl3.fshader"},
    {"./shaders/basic-gl3.vshader", "./shaders/pick-gl3.fshader"}
};

static vector<shared_ptr<ShaderState> > g_shaderStates; // our global shader states



// --------- Geometry

// Macro used to obtain relative offset of a field within a struct
#define FIELD_OFFSET(StructType, field) &(((StructType *)0)->field)

// A vertex with floating point position and normal
struct VertexPN {
    Cvec3f p, n;
    
    VertexPN() {}
    VertexPN(float x, float y, float z,
             float nx, float ny, float nz)
    : p(x,y,z), n(nx, ny, nz)
    {}
    
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

struct Geometry {
    GlBufferObject vbo, ibo;
    int vboLen, iboLen;
    
    Geometry(VertexPN *vtx, unsigned short *idx, int vboLen, int iboLen) {
        this->vboLen = vboLen;
        this->iboLen = iboLen;
        
        // Now create the VBO and IBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPN) * vboLen, vtx, GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * iboLen, idx, GL_STATIC_DRAW);
    }
    
    void draw(const ShaderState& curSS) {
        GLuint vao;
        glGenVertexArrays( 1, &vao );
        glBindVertexArray(vao);
        // Enable the attributes used by our shader
        safe_glEnableVertexAttribArray(curSS.h_aPosition);
        safe_glEnableVertexAttribArray(curSS.h_aNormal);
        
        // bind vbo
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        safe_glVertexAttribPointer(curSS.h_aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, p));
        safe_glVertexAttribPointer(curSS.h_aNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPN), FIELD_OFFSET(VertexPN, n));
        
        // bind ibo
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        // draw!
        glDrawElements(GL_TRIANGLES, iboLen, GL_UNSIGNED_SHORT, 0);
        
        // Disable the attributes used by our shader
        safe_glDisableVertexAttribArray(curSS.h_aPosition);
        safe_glDisableVertexAttribArray(curSS.h_aNormal);
        glBindVertexArray(0);
    }
};

typedef SgGeometryShapeNode<Geometry> MyShapeNode;

//--------------------------------------------------------------------------------
//  keyframe animator system specified variables
//--------------------------------------------------------------------------------
static list<vector<RigTForm>> keyFrames;  // keyframe list
static list<vector<RigTForm>>::iterator currentKeyFrame = keyFrames.end();

static shared_ptr<SgRootNode> g_world; //used as current edited keyframe scene graph
static vector<shared_ptr<SgRbtNode>> g_editedFrameRbtNodePointers; //current edited keyframe scene graph SgRbtNode pointer list


// ===================================================================
// Declare suitable nodes in the current scene graph
// 下面这些节点是被编辑的关键帧要展现的主题内容
// ===================================================================
static shared_ptr<SgRbtNode> g_skyNode, g_groundNode, g_robot1Node, g_robot2Node;

static shared_ptr<SgRbtNode> g_currentPickedRbtNode; // used later when you do picking

#pragma mark - Scene Variables

// Vertex buffer and index buffer associated with the ground and cube geometry
static shared_ptr<Geometry> g_ground, g_cube,g_sphere;

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space
static RigTForm g_skyRbt = RigTForm::makeTranslation(0.0, 0.25, 4.0);
//初始tramsformation，将object frame的原点保持不动，每个cube使用一个object matrix。由于在shader中使用了offset，故此处对象帧的起点都为原点。
static RigTForm g_objectRbt[3] = {RigTForm(Cvec3(0,0,0)),RigTForm(Cvec3(0,0,0)),RigTForm(Cvec3(0,0,0))};
static Cvec3f g_objectColors[3] = {Cvec3f(1, 0, 0),Cvec3f(0, 0, 1),Cvec3f(0.5, 0.5, 0)};
static RigTForm g_auxiliaryRbt;

static const float g_sphereRaidusScreenRatio = 0.25;
static float g_arcballScale;
static float g_arcballScreenRadius = g_sphereRaidusScreenRatio * min(g_windowWidth,g_windowHeight);
static bool g_arcballUpdateFlag = true;

static RigTForm g_motionRbt;



///////////////// END OF G L O B A L S //////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//  GLFW global variables
//--------------------------------------------------------------------------------
GLfloat cursor_x        = 0.f;
GLfloat cursor_y        = 0.f;

#pragma mark - Geometry Helper Function

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
    getCubeVbIbLen(vbLen, ibLen);
    
    // Temporary storage for cube geometry
    vector<VertexPN> vtx(vbLen);
    vector<unsigned short> idx(ibLen);
    
    makeCube(1, vtx.begin(), idx.begin());
    g_cube.reset(new Geometry(&vtx[0], &idx[0], vbLen, ibLen));
}

static void initSphere() {
    int ibLen, vbLen;
    int slices = 15,stacks = 15;
    getSphereVbIbLen(slices,stacks,vbLen, ibLen);
    
    // Temporary storage for cube geometry
    vector<VertexPN> vtx(vbLen);
    vector<unsigned short> idx(ibLen);
    
    makeSphere(1, slices, stacks, vtx.begin(), idx.begin());
    g_sphere.reset(new Geometry(&vtx[0], &idx[0], vbLen, ibLen));
}

// takes a projection matrix and send to the the shaders
static void sendProjectionMatrix(const ShaderState& curSS, const Matrix4& projMatrix) {
    GLfloat glmatrix[16];
    projMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
    safe_glUniformMatrix4fv(curSS.h_uProjMatrix, glmatrix);
}

// update g_frustFovY from g_frustMinFov, g_windowWidth, and g_windowHeight
static void updateFrustFovY() {
    if (g_windowWidth >= g_windowHeight)
        g_frustFovY = g_frustMinFov;
    else {
        const double RAD_PER_DEG = 0.5 * CS175_PI/180;
        g_frustFovY = atan2(sin(g_frustMinFov * RAD_PER_DEG) * g_windowHeight / g_windowWidth, cos(g_frustMinFov * RAD_PER_DEG)) / RAD_PER_DEG;
    }
}

static Matrix4 makeProjectionMatrix() {
    return Matrix4::makeProjection(
                                   g_frustFovY, g_windowWidth / static_cast <double> (g_windowHeight),
                                   g_frustNear, g_frustFar);
}

//--------------------------------------------------------------------------------
//  compute screen to eye scale for object frame origin in eye frame
//--------------------------------------------------------------------------------
static float computeArcballScale(const Cvec4 objectRbtOrigin){
    float screenToEyeScale;
    double zCoord = objectRbtOrigin[2];
    screenToEyeScale = getScreenToEyeScale(zCoord, g_frustFovY, g_windowHeight);
    
    return screenToEyeScale;
}

static RigTForm getEyeRbt(){
    RigTForm eyeRbt;
    if(g_activeEyeFrame == 1){
        g_currentPickedRbtNode = g_skyNode;
        eyeRbt = getPathAccumRbt(g_world, g_skyNode);
    }else if(g_activeEyeFrame == 2){
        //when you get a path from g_world to g_robot1Node,the path is a direct path and just includes g_world,g_robot1Node.
        // make g_skyCamera RBT left multiply the path to get an appropriate view angle.
        g_currentPickedRbtNode = g_robot1Node;
        eyeRbt = getPathAccumRbt(g_world, g_skyNode) * getPathAccumRbt(g_world, g_robot1Node);
    }else{
        g_currentPickedRbtNode=g_robot2Node;
        eyeRbt = getPathAccumRbt(g_world, g_skyNode) * getPathAccumRbt(g_world, g_robot2Node);
    }
    return eyeRbt;
}

static void drawStuff(const ShaderState& curSS, bool picking){
    //maintain relationship between current keyframe variable and keyframes list
    if(keyFrames.size()==0){
        currentKeyFrame = keyFrames.end();
    }
    if(keyFrames.size()>0){
        if(currentKeyFrame == keyFrames.end())
            currentKeyFrame = keyFrames.begin();
    }
    
    // build & send proj. matrix to vshader
    const Matrix4 projmat = makeProjectionMatrix();
    sendProjectionMatrix(curSS, projmat);
    
    // use the skyRbt as the eyeRbt
    const RigTForm eyeRbt = getEyeRbt();
    const RigTForm invEyeRbt = inv(eyeRbt);
    
    const Cvec3 eyeLight1 = Cvec3(invEyeRbt * Cvec4(g_light1, 1)); // g_light1 position in eye coordinates
    const Cvec3 eyeLight2 = Cvec3(invEyeRbt * Cvec4(g_light2, 1)); // g_light2 position in eye coordinates
    safe_glUniform3f(curSS.h_uLight, eyeLight1[0], eyeLight1[1], eyeLight1[2]);
    safe_glUniform3f(curSS.h_uLight2, eyeLight2[0], eyeLight2[1], eyeLight2[2]);

    if (!picking) {
        Drawer drawer(invEyeRbt, curSS);
        g_world->accept(drawer);
        
        // draw sphere
        //initSphere(); //the raidus of sphere changed constantly,but calling this method frequetly is not effective
        
        RigTForm mvmRbt = invEyeRbt * g_objectRbt[2];
        //update g_arcballScale，计算从眼部空间到window coordinate空间的对象的缩放倍数
        g_arcballScale = computeArcballScale(Cvec4(mvmRbt.getTranslation(),0));
        float screenRadiusScale = g_arcballScreenRadius*g_arcballScale;
        
        Matrix4 scaleMatrix = Matrix4::makeScale(Cvec3(screenRadiusScale,screenRadiusScale,screenRadiusScale));
        Matrix4 MVM = rigTFormToMatrix(mvmRbt) * scaleMatrix;
        Matrix4 NMVM = normalMatrix(MVM);
        sendModelViewNormalMatrix(curSS, MVM, NMVM);
        
        safe_glUniform3f(curSS.h_uColor, g_objectColors[2][0], g_objectColors[2][1], g_objectColors[2][2]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw wireframe
        g_sphere->draw(curSS);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draw filled again
    }else {
        Picker picker(invEyeRbt, curSS,g_currentPickedRbtNode,g_world,getEyeRbt(),g_motionRbt);
        g_world->accept(picker);
        glFlush();
        g_currentPickedRbtNode = picker.getRbtNodeAtXY(g_pickingMouseX, g_pickingMouseY);
        if (g_currentPickedRbtNode == g_groundNode)
            g_currentPickedRbtNode = shared_ptr<SgRbtNode>();   // set to NULL
    }
//    //此处实现SgRootNode中的节点倾倒出的动作。
//    vector<shared_ptr<SgRbtNode>> nodePointer; //cpp can initialize automatically by default
//    dumpSgRbtNodes(g_world, nodePointer);
//
//    vector<RigTForm> currentFrameRbts;
//    //容器类的便利方法
//    for(vector<shared_ptr<SgRbtNode>>::iterator iter = nodePointer.begin(),end = nodePointer.end() ; iter!=end ; iter++){
//
//        //iter.getRbt();
//        shared_ptr<SgRbtNode> myNode = dynamic_pointer_cast<SgRbtNode>(*iter);
//        RigTForm rbt=myNode->getRbt();
//        currentFrameRbts.push_back(rbt);
//    }
    
}

#pragma mark - hotkey 'n' function
static void insertNewKeyFrame(){
    
    //此处实现SgRootNode中的节点倾倒出的动作。
    vector<shared_ptr<SgRbtNode>> nodePointer; //cpp can initialize automatically by default
    dumpSgRbtNodes(g_world, nodePointer);
    
    vector<RigTForm> currentFrameRbts;
    //将scene graph中的RBT数据copy到vector<RigTForm>（单frame数据状态存储）中去
    for(vector<shared_ptr<SgRbtNode>>::iterator iter = nodePointer.begin(),end = nodePointer.end() ; iter!=end ; iter++){
        
        //iter.getRbt();
        shared_ptr<SgRbtNode> myNode = dynamic_pointer_cast<SgRbtNode>(*iter);
        RigTForm rbt=myNode->getRbt();
        currentFrameRbts.push_back(rbt);
    }
    keyFrames.push_back(currentFrameRbts);
    
    if(currentKeyFrame != keyFrames.end()){
        currentKeyFrame++;
    }else{
        currentKeyFrame = keyFrames.begin();
    }
    
}

#pragma mark - '>' andvanced function
static void advanced(){
    if(currentKeyFrame == keyFrames.end())
        return;
    
    currentKeyFrame++;
    
    if(currentKeyFrame == keyFrames.end())
        currentKeyFrame--;
    
    restoreStatusToScenegraph();
}

#pragma mark - '<' retreat function
static void retreat(){
    if(currentKeyFrame == keyFrames.end() || currentKeyFrame == keyFrames.begin())
        return;
    
    currentKeyFrame--;
    
    restoreStatusToScenegraph();
}

#pragma mark - 'd' delete function
static void deleteKeyFrame(){
    
    if(keyFrames.size()==1){
        keyFrames.erase(currentKeyFrame);
        //currentKeyFrame will be set to be end interator automatically
    }else if(keyFrames.size()>1){
        list<vector<RigTForm>>::iterator copyIter = currentKeyFrame;
        if(currentKeyFrame == keyFrames.begin()){
            copyIter++;
        }else{
            copyIter--;
        }
        
        keyFrames.erase(currentKeyFrame);
        currentKeyFrame = copyIter;
        restoreStatusToScenegraph();
    }else{
        
    }
}

#pragma mark - hotkey 'Space' function
static void restoreStatusToScenegraph(){
    vector<RigTForm> currentKeyFrameVector = *currentKeyFrame;
    
    //此处实现SgRootNode中的节点倾倒出的动作。
    vector<shared_ptr<SgRbtNode>> nodePointer; //cpp can initialize automatically by default
    dumpSgRbtNodes(g_world, nodePointer);
    
    int index = 0;
    for(vector<RigTForm>::iterator iter=currentKeyFrameVector.begin(),end=currentKeyFrameVector.end();iter!=end;iter++){
        shared_ptr<SgRbtNode> node = nodePointer[index];
        node->setRbt(*iter);
    }
    
}

#pragma mark - hotkey 'u' update function
static void updateScenegraphStatusDataToKeyFrameList(){
    //if current key frame is not defined,then insert new key frame
    if(currentKeyFrame == keyFrames.end())
        insertNewKeyFrame();
    
    //此处实现SgRootNode中的节点倾倒出的动作。
    vector<shared_ptr<SgRbtNode>> nodePointer; //cpp can initialize automatically by default
    dumpSgRbtNodes(g_world, nodePointer);
    
    vector<RigTForm> currentFrameRbts;
    //容器类的便利方法
    for(vector<shared_ptr<SgRbtNode>>::iterator iter = nodePointer.begin(),end = nodePointer.end() ; iter!=end ; iter++){
        
        //iter.getRbt();
        shared_ptr<SgRbtNode> myNode = dynamic_pointer_cast<SgRbtNode>(*iter);
        RigTForm rbt=myNode->getRbt();
        currentFrameRbts.push_back(rbt);
    }
    
    *currentKeyFrame = currentFrameRbts;
}

//static void drawStuff() {
//    // short hand for current shader state
//    const ShaderState& curSS = *g_shaderStates[g_activeShader];
//    
//    // build & send proj. matrix to vshader
//    const Matrix4 projmat = makeProjectionMatrix();
//    sendProjectionMatrix(curSS, projmat);
//    
//    // use the skyRbt as the eyeRbt
//    const RigTForm eyeRbt = g_skyRbt;
//    const RigTForm invEyeRbt = inv(eyeRbt);
//    
//    const Cvec3 eyeLight1 = Cvec3(invEyeRbt * Cvec4(g_light1, 1)); // g_light1 position in eye coordinates
//    const Cvec3 eyeLight2 = Cvec3(invEyeRbt * Cvec4(g_light2, 1)); // g_light2 position in eye coordinates
//    safe_glUniform3f(curSS.h_uLight, eyeLight1[0], eyeLight1[1], eyeLight1[2]);
//    safe_glUniform3f(curSS.h_uLight2, eyeLight2[0], eyeLight2[1], eyeLight2[2]);
//    
//    // draw ground
//    // ===========
//    //
//    const RigTForm groundRbt = RigTForm::identity();  // identity
//    Matrix4 MVM = rigTFormToMatrix(invEyeRbt * groundRbt);
//    Matrix4 NMVM = normalMatrix(MVM);
//    sendModelViewNormalMatrix(curSS, MVM, NMVM);
//    safe_glUniform3f(curSS.h_uColor, 0.1, 0.95, 0.1); // set color
//    g_ground->draw(curSS);
//    
//    // draw cubes
//    // ==========
//    RigTForm mvmRbt = invEyeRbt * g_objectRbt[0];
//    MVM = rigTFormToMatrix(mvmRbt);
//    NMVM = normalMatrix(MVM);
//    sendModelViewNormalMatrix(curSS, MVM, NMVM);
//    safe_glUniform3f(curSS.h_uColor, g_objectColors[0][0], g_objectColors[0][1], g_objectColors[0][2]);
//    g_cube->draw(curSS);
//    if(g_activeCube == 0){
//        if(g_arcballUpdateFlag)
//            g_arcballScale = computeArcballScale(Cvec4(mvmRbt.getTranslation(),0));
//    }
//    
//    mvmRbt = invEyeRbt * g_objectRbt[1];
//    MVM = rigTFormToMatrix(mvmRbt);
//    NMVM = normalMatrix(MVM);
//    sendModelViewNormalMatrix(curSS, MVM, NMVM);
//    safe_glUniform3f(curSS.h_uColor, g_objectColors[1][0], g_objectColors[1][1], g_objectColors[1][2]);
//    g_cube->draw(curSS);
//    if(g_activeCube == 1){
//        if(g_arcballUpdateFlag)
//            g_arcballScale = computeArcballScale(Cvec4(mvmRbt.getTranslation(),0));
//    }
//    
//    // draw sphere
//    //initSphere(); //the raidus of sphere changed constantly,but calling this method frequetly is not effective
//    float screenRadiusScale = g_arcballScreenRadius*g_arcballScale;
//    Matrix4 scaleMatrix = Matrix4::makeScale(Cvec3(screenRadiusScale,screenRadiusScale,screenRadiusScale));
//    mvmRbt = invEyeRbt * g_objectRbt[2];
//    MVM = rigTFormToMatrix(mvmRbt) * scaleMatrix;
//    NMVM = normalMatrix(MVM);
//    sendModelViewNormalMatrix(curSS, MVM, NMVM);
//    safe_glUniform3f(curSS.h_uColor, g_objectColors[2][0], g_objectColors[2][1], g_objectColors[2][2]);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw wireframe
//    g_sphere->draw(curSS);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draw filled again
//}

static void display() {
    glUseProgram(g_shaderStates[g_activeShader]->program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                   // clear framebuffer color&depth
    
    if(g_pickingFlag)
        pick();
    else
        drawStuff(*g_shaderStates[g_activeShader],false);
    
    //   show the back buffer (where we rendered stuff)
    //  glfwSwapBuffers(window);
}

static void reshape(GLFWwindow* window,const int w, const int h) {
    g_windowWidth = w;
    g_windowHeight = h;
    glViewport(0, 0, w, h);
    cerr << "Size of window is now " << w << "x" << h << endl;
    updateFrustFovY();
    g_arcballScreenRadius = g_sphereRaidusScreenRatio * min(g_windowWidth,g_windowHeight);
}

static void displayWindow(GLFWwindow* window){
    display();
}


static void motion(const float x, const float y) {
    if(x<0 || y<0)
        return;
    
    Cvec2 startScreenPos = Cvec2(g_mouseClickX,g_mouseClickY);
    Cvec2 endScreenPos = Cvec2(x,g_windowHeight - y - 1); //convert from window coordnate to OpenGL window coordinate.
    Cvec2 centerScreenPos = getScreenSpaceCoord(g_objectRbt[0].getTranslation(),makeProjectionMatrix(), 0.0, 0.0, g_windowWidth, g_windowHeight);
    Quat arcballQuat = arcball(Cvec3(centerScreenPos,0), g_arcballScreenRadius, startScreenPos, endScreenPos);
    
    const double dx = x - g_mouseClickX;
    const double dy = g_windowHeight - y - 1 - g_mouseClickY;
    
    g_arcballUpdateFlag = true;
    
    RigTForm m;
    if (g_mouseLClickButton && !g_mouseRClickButton) { // left button down?
  //    m = RigTForm::makeXRotation(-dy) * RigTForm::makeYRotation(dx);
        m = RigTForm(arcballQuat);
    }
    else if (g_mouseRClickButton && !g_mouseLClickButton) { // right button down?
        m = RigTForm(Cvec3(dx, dy, 0) * g_arcballScale/**0.01*/);
    }
    else if (g_mouseMClickButton || (g_mouseLClickButton && g_mouseRClickButton)) {  // middle or (left and right) button down?
        m = RigTForm(Cvec3(0, 0, -dy) * g_arcballScale /**0.01*/);
        g_arcballUpdateFlag = false;
    }
    
    
    if (g_mouseClickDown) {
        if(g_activeCube == 0){
            //g_objectRbt[0] *= m; // Simply right-multiply is WRONG
            g_auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
            g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], g_auxiliaryRbt);
            
        }else if(g_activeCube == 1){
            g_auxiliaryRbt = makeMixedFrame(g_objectRbt[1], g_skyRbt);
            g_objectRbt[1] = doQtoOwrtA(m, g_objectRbt[1], g_auxiliaryRbt);
        }else{
            RigTForm invMouseMotionMatrix = inv(m);
            g_skyRbt = doQtoOwrtA(m, g_skyRbt, g_skyRbt);
        }
        g_motionRbt = m;
        
    }
    
    
    
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;
}

void cursor_position_callback( GLFWwindow* window, double x, double y )
{
    cursor_x = (float) x;
    cursor_y = (float) y;
}

static void mouse(GLFWwindow* window, const int button, const int action, int mods) {
    g_mouseClickX = cursor_x;
    g_mouseClickY = g_windowHeight - cursor_y - 1;  // conversion from GLUT window-coordinate-system to OpenGL window-coordinate-system
    g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=false;
    
    g_mouseLClickButton |= (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS);
    g_mouseRClickButton |= (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS);
    g_mouseMClickButton |= (button == GLFW_MOUSE_BUTTON_4 && action == GLFW_PRESS);
    
    g_mouseLClickButton &= !(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE);
    g_mouseRClickButton &= !(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE);
    g_mouseMClickButton &= !(button == GLFW_MOUSE_BUTTON_4 && action == GLFW_RELEASE);
    
    g_mouseClickDown = g_mouseLClickButton || g_mouseRClickButton || g_mouseMClickButton;
    
    if(g_mouseClickDown){
        g_pickingMouseX = g_mouseClickX;
        g_pickingMouseY = g_mouseClickY;
    }
    
    
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, GLFW_TRUE);
//
//    if (key == GLFW_MOUSE_BUTTON_LEFT | GLFW_MOUSE_BUTTON_RIGHT | GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
//
//    }
//}

static void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS){
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GLFW_TRUE);//exit(0);   // ESC
            case GLFW_KEY_H:
                cout << " ============== H E L P ==============\n\n"
                << "h\t\thelp menu\n"
                << "s\t\tsave screenshot\n"
                << "f\t\tToggle flat shading on/off.\n"
                << "o\t\tCycle object to edit\n"
                << "p\t\tPart Picking\n"
                << "v\t\tCycle view\n"
                << "drag left mouse to rotate\n" << endl;
                break;
            case GLFW_KEY_S:
                glFlush();
                writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
                break;
            case GLFW_KEY_O:
                g_activeCube++;
                if(g_activeCube > 2)
                    g_activeCube=0;
                break;
            case GLFW_KEY_P:
                g_pickingFlag ^= true;
                break;
            case GLFW_KEY_F:
                g_activeShader ^= 1;
                break;
            case GLFW_KEY_V:
                g_activeEyeFrame++;
                if(g_activeEyeFrame > 3)
                    g_activeEyeFrame=1;
                break;
            case GLFW_KEY_LEFT_BRACKET:
                retreat();
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                advanced();
                break;
            case GLFW_KEY_SPACE:
                restoreStatusToScenegraph();
                break;
            case GLFW_KEY_U:
                updateScenegraphStatusDataToKeyFrameList();
                break;
            case GLFW_KEY_D:
                deleteKeyFrame();
                break;
            case GLFW_KEY_N:
                insertNewKeyFrame();
                break;
        }
    }
}

//static void initGlutState(int argc, char * argv[]) {
//  glutInit(&argc, argv);                                  // initialize Glut based on cmd-line args
//  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);  //  RGBA pixel channels and double buffering
//  glutInitWindowSize(g_windowWidth, g_windowHeight);      // create a window
//done  glutCreateWindow("Assignment 2");                       // title the window
//
// GLFW在mainloop中使用while语句不停的循环，fps能跑多高跑多高
//done  glutDisplayFunc(display);                               // display rendering callback
//done  glutReshapeFunc(reshape);                               // window reshape callback
//done  glutMotionFunc(motion);                                 // mouse movement callback
//done  glutMouseFunc(mouse);                                   // mouse click callback
//done  glutKeyboardFunc(keyboard);
//}


static int initGlfwState(){
    if(!glfwInit())
        return -1;
    
    //设置OpenGL core profile
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
    //创建本地窗口
    window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Robots and Part Picking", NULL, NULL);
    if(!window){
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    cout << "GLFW version                : " << glfwGetVersionString() << endl;
    cout << "GL_VERSION                  : " << glGetString( GL_VERSION ) << endl;
    cout << "GL_VENDOR                   : " << glGetString( GL_VENDOR ) << endl;
    cout << "GL_RENDERER                 : " << glGetString( GL_RENDERER ) << endl;
    cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl;
    
    //鼠标指针位置回调
    glfwSetCursorPosCallback(window, cursor_position_callback);
    //鼠标动作回调设置
    glfwSetMouseButtonCallback(window, mouse);
    
    //窗口尺寸变化事件
    glfwSetFramebufferSizeCallback(window, reshape);
    //键盘事件回调
    glfwSetKeyCallback(window, keyboard);
    
    //error callback
    glfwSetErrorCallback(error_callback);
    
    return 0;
    
}

static void initGLState() {
    glClearColor(128./255., 200./255., 255./255., 0.);
    glClearDepth(0.f);
    //glClear(GL_DEPTH_BUFFER_BIT);
    
    //  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //  glPixelStorei(GL_PACK_ALIGNMENT, 1);
    //  glCullFace(GL_BACK);
    //  glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    //  glReadBuffer(GL_BACK);
    
    glEnable(GL_FRAMEBUFFER_SRGB);
}

static void initShaders() {
    g_shaderStates.resize(g_numShaders);
    for (int i = 0; i < g_numShaders; ++i) {
        g_shaderStates[i].reset(new ShaderState(g_shaderFiles[i][0], g_shaderFiles[i][1]));
    }
}

static void initGeometry() {
    initGround();
    initCubes();
    initSphere();
}

static void constructRobot(shared_ptr<SgTransformNode> base, const Cvec3& color) {
    
    const float ARM_LEN = 0.7,ARM_THICK = 0.25,TORSO_LEN = 1.5,TORSO_THICK = 0.25,TORSO_WIDTH = 1, HEAD_LEN = 0.5,HEAD_THICK = 0.45,LIMB_LEN = 0.9,LIMB_THICK = 0.25;
    const int NUM_JOINTS = 10,NUM_SHAPES = 10;
    
    struct JointDesc {
        int parent;
        float x, y, z;
    };
    
    //each joint is just a respected frame
    JointDesc jointDesc[NUM_JOINTS] = {
        {-1}, // torso
        {0, 0, TORSO_LEN/2 + 0.1f, 0}, // header
        {0,  TORSO_WIDTH/2, TORSO_LEN/2, 0}, // upper right arm
        {0,  -TORSO_WIDTH/2, TORSO_LEN/2, 0}, // upper left arm
        {2,  ARM_LEN, 0, 0}, // lower right arm
        {3,  -ARM_LEN, 0, 0}, // lower left arm
        {0,  TORSO_WIDTH/2 - LIMB_THICK/2, -TORSO_LEN/2, 0}, // upper right limb
        {0,  -TORSO_WIDTH/2 + LIMB_THICK/2, -TORSO_LEN/2, 0}, // upper left limb
        {6,  0, -LIMB_LEN, 0}, // lower right limb
        {7,  0, -LIMB_LEN, 0}, // lower left limb
    };
    
    struct ShapeDesc {
        int parentJointId;
        float x, y, z, sx, sy, sz;
        shared_ptr<Geometry> geometry;
    };
    
    ShapeDesc shapeDesc[NUM_SHAPES] = {
        {0, 0,         0, 0, TORSO_WIDTH, TORSO_LEN, TORSO_THICK, g_cube}, // torso
        {1, 0, HEAD_LEN/2, 0, HEAD_LEN/2, HEAD_LEN/2, HEAD_THICK/2, g_sphere}, // head
        {2, ARM_LEN/2, 0, 0, ARM_LEN/2, ARM_THICK/2, ARM_THICK/2, g_sphere}, // upper right arm
        {3, -ARM_LEN/2, 0, 0, ARM_LEN/2, ARM_THICK/2, ARM_THICK/2, g_sphere}, // upper left arm
        {4, ARM_LEN/2, 0, 0, ARM_LEN, ARM_THICK, ARM_THICK, g_cube}, // lower right arm
        {5, -ARM_LEN/2, 0, 0, ARM_LEN, ARM_THICK, ARM_THICK, g_cube}, // lower left arm
        {6, LIMB_THICK/2 , -LIMB_LEN/2, 0, LIMB_THICK/2, LIMB_LEN/2, LIMB_THICK/2, g_sphere}, // upper right limb
        {7, -LIMB_THICK/2, -LIMB_LEN/2, 0, LIMB_THICK/2, LIMB_LEN/2, LIMB_THICK/2, g_sphere}, // upper left limb
        {8, LIMB_THICK/2, -LIMB_LEN/2, 0, LIMB_THICK, LIMB_LEN, LIMB_THICK, g_cube}, // lower right limb
        {9, -LIMB_THICK/2, -LIMB_LEN/2, 0, LIMB_THICK, LIMB_LEN, LIMB_THICK, g_cube}, // lower left limb
        
    };
    
    shared_ptr<SgTransformNode> jointNodes[NUM_JOINTS];
    
    for (int i = 0; i < NUM_JOINTS; ++i) {
        if (jointDesc[i].parent == -1)
            jointNodes[i] = base;
        else {
            jointNodes[i].reset(new SgRbtNode(RigTForm(Cvec3(jointDesc[i].x, jointDesc[i].y, jointDesc[i].z))));
            jointNodes[jointDesc[i].parent]->addChild(jointNodes[i]);
        }
    }
    for (int i = 0; i < NUM_SHAPES; ++i) {
        shared_ptr<MyShapeNode> shape(
                                      new MyShapeNode(shapeDesc[i].geometry,
                                                      color,
                                                      Cvec3(shapeDesc[i].x, shapeDesc[i].y, shapeDesc[i].z),
                                                      Cvec3(0, 0, 0),
                                                      Cvec3(shapeDesc[i].sx, shapeDesc[i].sy, shapeDesc[i].sz)));
        jointNodes[shapeDesc[i].parentJointId]->addChild(shape);
    }
}

static void initScene() {
    g_world.reset(new SgRootNode());
    
    g_skyNode.reset(new SgRbtNode(RigTForm(Cvec3(0.0, 0.25, 4.0))));
    
    g_groundNode.reset(new SgRbtNode());
    g_groundNode->addChild(shared_ptr<MyShapeNode>(
                                                   new MyShapeNode(g_ground, Cvec3(0.1, 0.95, 0.1))));
    
    g_robot1Node.reset(new SgRbtNode(RigTForm(Cvec3(-2, 1, 0))));
    g_robot2Node.reset(new SgRbtNode(RigTForm(Cvec3(2, 1, 0))));
    
    constructRobot(g_robot1Node, Cvec3(1, 0, 0)); // a Red robot
    constructRobot(g_robot2Node, Cvec3(0, 0, 1)); // a Blue robot
    
    g_world->addChild(g_skyNode);
    g_world->addChild(g_groundNode);
    g_world->addChild(g_robot1Node);
    g_world->addChild(g_robot2Node);
}

static void pick() {
    // We need to set the clear color to black, for pick rendering.
    // so let's save the clear color
    GLdouble clearColor[4];
    glGetDoublev(GL_COLOR_CLEAR_VALUE, clearColor);
    
    glClearColor(0, 0, 0, 0);
    
    // using PICKING_SHADER as the shader
    glUseProgram(g_shaderStates[PICKING_SHADER]->program);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawStuff(*g_shaderStates[PICKING_SHADER], true);
    
    // Uncomment below and comment out the glutPostRedisplay in mouse(...) call back
    // to see result of the pick rendering pass
    // glutSwapBuffers();
    
    //Now set back the clear color
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    
    checkGlError(__func__);
}

int main(int argc, char * argv[]) {
    try {
        //initGlutState(argc,argv);
        
        
        //initGLFW
        initGlfwState();
        
        GLenum err = glewInit(); // load the OpenGL extensions
        
        if (GLEW_OK != err){
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
        
        if(GLEW_VERSION_4_1)
            fprintf(stdout, "GLEW VERSION 4.1 Supported \n");
        
        //    cout << (g_Gl2Compatible ? "Will use OpenGL 2.x / GLSL 1.0" : "Will use OpenGL 3.x / GLSL 1.3") << endl;
        //    if ((!g_Gl2Compatible) && !GLEW_VERSION_3_0)
        //      throw runtime_error("Error: card/driver does not support OpenGL Shading Language v1.3");
        //    else if (g_Gl2Compatible && !GLEW_VERSION_2_0)
        //      throw runtime_error("Error: card/driver does not support OpenGL Shading Language v1.0");
        
        
        initGLState();
        initGeometry();
        initShaders();
        initScene();
        
        //      // open current directory:
        //      unique_ptr<DIR> pDir(opendir("."));
        //
        //      // process each directory entry:
        //
        //      struct dirent *dp;
        //      while ((dp = readdir(pDir.get())) != nullptr) {
        //
        //          string filename(dp->d_name);
        //
        //          cout << "process " << filename << endl;
        //      }
        
        glfwSetWindowRefreshCallback(window, displayWindow); //根据窗口刷新回调函数
        
        OpenGL_Helper::PerfMonitor perfMonitor;
        float fps;
        
        //mac os x的窗口调用会限制帧数
        while( !glfwWindowShouldClose(window) ){
            display();
            perfMonitor.Update(fps);
            cout << "Current FPS at timeinterval:" << glfwGetTime() << " is " << fps << endl;
            
            motion(cursor_x, cursor_y);
            
            glfwSwapBuffers( window );
            
            //glfwPollEvents();
            glfwWaitEvents(); //if window is put in background,then didn't return immediatetly until at least one available window event is call.
        }
        
        glfwTerminate();
        
        return 0;
    }
    catch (const runtime_error& e) {
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
}
