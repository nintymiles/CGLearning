//
//  main.cpp
//  Assignment4-RobotsAndPartPicking
//
//  Created by SeanRen on 2020/03/09.
//  Copyright © 2020 zxtech. All rights reserved.
//

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

#include "cvec.h"
#include "matrix4.h"
#include "quat.h"
#include "rigtform.h"
#include "geometrymaker.h"
#include "arcball.h"
#include "ppm.h"

#include "Geometry.h"
#include "glsupport.h"
#include "shadersasst.h"
#include "scenegraph.h"
#include "drawer.h"
#include "RayCastPicker.h"
#include "Sphere.h"

#include "MoreTeapotsModel.h"

#include "perfMonitor.h"

using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////

static const float g_frustMinFov = 90.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -2000.0;    // far plane

static int g_windowWidth = 1024;
static int g_windowHeight = 1024;
static float g_windowWScale = 1;
static float g_windowHScale = 1;

static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton;
static int g_mouseClickX, g_mouseClickY,g_pickingMouseX,g_pickingMouseY; // coordinates for mouse click event

static GLFWwindow* window;

static const float g_sphereRaidusScreenRatio = 0.25;
static float g_arcballScale;
static float g_arcballScreenRadius = g_sphereRaidusScreenRatio * min(g_windowWidth,g_windowHeight);
static bool g_arcballUpdateFlag = true;

static RigTForm g_motionRbt;



///////////////// END OF G L O B A L S //////////////////////////////////////////////////

shared_ptr<MoreTeapotsModel> teapotModel;

//--------------------------------------------------------------------------------
//  GLFW global variables
//--------------------------------------------------------------------------------
GLfloat cursor_x        = 0.f;
GLfloat cursor_y        = 0.f;

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
        //意图？达到了将alpha值用在不同对角线位置的效果
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
    //    eyeRbt = getPathAccumRbt(g_world, g_skyNode);
    //    if(g_activeEyeFrame == 1){
    ////        g_currentPickedRbtNode = g_skyNode;
    //        //eyeRbt = getPathAccumRbt(g_world, g_skyNode);
    //    }else if(g_activeEyeFrame == 2){
    //        //when you get a path from g_world to g_robot1Node,the path is a direct path and just includes g_world,g_robot1Node.
    //        // make g_skyCamera RBT left multiply the path to get an appropriate view angle.
    ////        g_currentPickedRbtNode = g_robot1Node;
    //        //eyeRbt = getPathAccumRbt(g_world, g_skyNode) * getPathAccumRbt(g_world, g_robot1Node);
    //    }else{
    ////        g_currentPickedRbtNode=g_robot2Node;
    //        //eyeRbt = getPathAccumRbt(g_world, g_skyNode) * getPathAccumRbt(g_world, g_robot2Node);
    //    }
    return eyeRbt;
}

static void drawStuff(){
    teapotModel->UpdateViewport();
    teapotModel->Render(1.0, 0.0, 0.0);
    
}

static void display() {
    //glUseProgram(program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear framebuffer color&depth
    
    
    drawStuff();
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

static void scaleCallback(GLFWwindow* window,const float wScale, const float hScale) {
    g_windowWScale = wScale;
    g_windowHScale = hScale;
    cerr << "ContentScale of window is now " << wScale << "x" << hScale << endl;
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
    Cvec2 centerScreenPos = getScreenSpaceCoord(RigTForm().getTranslation(),makeProjectionMatrix(), 0.0, 0.0, g_windowWidth, g_windowHeight);
    Quat arcballQuat = trackball(Cvec3(centerScreenPos,0), g_arcballScreenRadius, startScreenPos, endScreenPos);
    
    const double dx = x - g_mouseClickX;
    const double dy = g_windowHeight - y - 1 - g_mouseClickY;
    
    g_arcballUpdateFlag = true;
    
    RigTForm m;
    if (g_mouseLClickButton && !g_mouseRClickButton) { // left button down?
                                                       //m = RigTForm::makeXRotation(-dy) * RigTForm::makeYRotation(dx);
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
        //        if(g_activeCube == 0){
        //            //g_objectRbt[0] *= m; // Simply right-multiply is WRONG
        //            g_auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
        //            g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], g_auxiliaryRbt);
        //
        //        }else if(g_activeCube == 1){
        //            g_auxiliaryRbt = makeMixedFrame(g_objectRbt[1], g_skyRbt);
        //            g_objectRbt[1] = doQtoOwrtA(m, g_objectRbt[1], g_auxiliaryRbt);
        //        }else{
        //            //RigTForm invMouseMotionMatrix = inv(m);
        //            //g_skyRbt = doQtoOwrtA(m, g_skyRbt, g_skyRbt);
        //        }
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
    g_mouseClickY = g_windowHeight - cursor_y - 1;  // conversion from  window-coordinate-system to OpenGL window-coordinate-system
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
                break;
            case GLFW_KEY_P:
                break;
            case GLFW_KEY_F:
                break;
            case GLFW_KEY_V:
                break;
        }
    }
}

static int initGlfwState(){
    if(!glfwInit())
        return -1;
    
    //设置OpenGL core profile
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    
    //创建本地窗口
    window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Utah Teapot Rendering", NULL, NULL);
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
    glfwSetWindowContentScaleCallback(window,scaleCallback);
    //键盘事件回调
    glfwSetKeyCallback(window, keyboard);
    
    //error callback
    glfwSetErrorCallback(error_callback);
    
    return 0;
    
}

static void initGLState() {
    glClearColor(128./255., 200./255., 255./255., 0.);
    glClearDepth(0.f);
    
    //necessary step for picking
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //  glPixelStorei(GL_PACK_ALIGNMENT, 1);
    //  glCullFace(GL_BACK);
    //  glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    
    glEnable(GL_FRAMEBUFFER_SRGB);
}

static void initGeometry() {
    teapotModel.reset(new MoreTeapotsModel());
    teapotModel->Init(10,10,2);
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
        
        
        initGLState();
        initGeometry();
        
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
            //cout << "Current FPS at timeinterval:" << glfwGetTime() << " is " << fps << endl;
            
            motion(cursor_x, cursor_y);
            
            glfwSwapBuffers( window );
            
            glfwPollEvents();
            //glfwWaitEvents(); //if window is put in background,then didn't return immediatetly until at least one available window event is call.
        }
        
        glfwTerminate();
        
        return 0;
    }
    catch (const runtime_error& e) {
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
}
