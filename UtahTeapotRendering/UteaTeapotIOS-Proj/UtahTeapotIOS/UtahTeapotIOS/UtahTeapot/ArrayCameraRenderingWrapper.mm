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
#include "arcball.h"

#include "shadersasst.h"
#include "Geometry.h"
#include "TeapotModel.h"
#include "Camera.h"
#include "PlaneModel.h"

using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////


static const float g_frustMinFov = 50.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -1000.0;    // far plane

static int g_windowWidth = 512;
static int g_windowHeight = 512;
static float g_arcballScreenRadius = 0.5 * (float)min(g_windowWidth,g_windowHeight);

static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton, g_quadTap;
static int g_mouseClickX, g_mouseClickY; // coordinates for mouse click event
static int g_activeShader = 0;


// --------- Scene

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space
//static Matrix4 g_skyRbt = Matrix4::makeTranslation(Cvec3(0.0, 0.25, 4.0));
static RigTForm g_skyRbt = RigTForm(Cvec3(0.0, 0.25, 4.0));
//static Matrix4 g_objectRbt[2] = {Matrix4::makeTranslation(Cvec3(0,0,0)),Matrix4::makeTranslation(Cvec3(0,0,-2))};
static RigTForm g_objectRbt[2] = {RigTForm(Cvec3(0.5,0.5,0.5)),RigTForm(Cvec3(0,0,0))};

static const Cvec3 g_objectFrameOrigin = Cvec3(0,-0.25,-4.0);


///////////////// END OF G L O B A L S //////////////////////////////////////////////////
shared_ptr<TeapotModel> teapotModel;
shared_ptr<PlaneModel> planeModel;
vector<PerspectiveCamera> arrayCamera;

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

static void drawStuff() {
    
    for(auto &camera:arrayCamera){
        SubViewPort svPort = camera.subview;
        int x = svPort.x * g_windowWidth;
        int y = svPort.y * g_windowHeight;
        int width = svPort.width * g_windowWidth;
        int height = svPort.height * g_windowHeight;
        glViewport(x, y, width, height);
        
        planeModel->setPerspectiveCamera(make_shared<PerspectiveCamera>(camera));
        planeModel->Render();
        
        teapotModel->setPerspectiveCamera(make_shared<PerspectiveCamera>(camera));
        teapotModel->mat_model_ = teapotModel->mat_model_ * Matrix4::makeZRotation(0.05);
        teapotModel->Render(1.0, 0.055, 0.027);
    }
    
}

static void display() {
    //glUseProgram(g_shaderStates[g_activeShader]->program);
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
    
    float aspect = g_windowWidth / static_cast <double> (g_windowHeight);
    
    
    for(auto &camera : arrayCamera){
        camera.aspect = aspect;
        camera.far = g_frustFar;
        camera.near = g_frustNear;
        camera.fov = g_frustFovY;
        camera.updatePorjectonMatrix();
    }
    
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
    else if (g_quadTap){
        
    }
    
    if (g_mouseClickDown) {
        //g_objectRbt[0] = g_objectRbt[0] * m; // Simply right-multiply is WRONG
        RigTForm auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
        g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], auxiliaryRbt);
    }
    
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;
}


static void mouse(const float x, const float y,u_long tapCount,bool press) {
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;  // conversion from window-coordinate-system to OpenGL window-coordinate-system
    
    //every time initialze all mouse status to false
    g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=g_quadTap=false;
    
    g_mouseLClickButton |= (press && tapCount == 1);
    g_mouseRClickButton |= (press && tapCount == 2);
    g_mouseMClickButton |= (press && tapCount == 3);
    g_quadTap           |= (press && tapCount == 4);
    
//    g_mouseLClickButton &= !(!press && tapCount == 1);
//    g_mouseRClickButton &= !(!press && tapCount == 2);
//    g_mouseMClickButton &= !(!press && tapCount == 3);
    if(!press)
        g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=g_quadTap=false;
    
    
    g_mouseClickDown = g_mouseLClickButton || g_mouseRClickButton || g_mouseMClickButton;
}


static void initGLState() {
    //设置color buffer默认颜色
    glClearColor(128./255., 200./255., 255./255., 0.);
    //重置depth buffer默认值为0.f
    glClearDepthf(0.f);
    
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    //glReadBuffer(GL_BACK);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

static void initGeometry() {
    teapotModel.reset(new TeapotModel());
    teapotModel->Init();
    
    planeModel.reset(new PlaneModel());
    planeModel->Init();
    
    int amount = 6;
    float size = (float)1/amount;
    
    for ( int y = 0; y < amount; y ++ ) {
        
        for ( int x = 0; x < amount; x ++ ) {
            PerspectiveCamera subcamera;
            subcamera.subview = SubViewPort((float)x/amount,(float)y/amount,size,size);
            arrayCamera.push_back(subcamera);
        }
        
    }
}

bool GraphicsInit()
{
    //printOpenGLESInfo();
    initGLState();
    //initShaders();
    initGeometry();
    
    return true;
}

bool GraphicsResize( int width, int height )
{
    //    g_windowWidth = width;
    //    g_windowHeight = height;
    //    glViewport(0, 0, width, height);
    reshape(width, height);
    double radiusSFactor = getScreenToEyeScale(-4, g_frustFovY, g_windowHeight);
    g_arcballScreenRadius = 2 / radiusSFactor;
    
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

void TouchEventMove( float x, float y,unsigned long touchCount )
{
    touch_location_x = x;
    touch_location_y = y;
    
    //--------------------------------------------------------------------------------
    // arcball interface applying
    //--------------------------------------------------------------------------------
    if(touchCount > 1){
        
        Cvec2 startScreenPos = Cvec2(g_mouseClickX,g_mouseClickY);
        Cvec2 endScreenPos = Cvec2(x,g_windowHeight - y - 1); //convert from window coordnate to OpenGL window coordinate.
        Cvec2 centerScreenPos = getScreenSpaceCoord(g_objectFrameOrigin,makeProjectionMatrix(), 0.0, 0.0, g_windowWidth, g_windowHeight);
        Quat arcballQuat = arcball(Cvec3(centerScreenPos,0), g_arcballScreenRadius, startScreenPos, endScreenPos);
        //ego motion
        //g_skyRbt = g_skyRbt * RigTForm(arcballQuat);
        //让camera围绕sphere转动，以保持sphere一直在视野中心,around the auxiliary frame
        g_skyRbt = doQtoOwrtA(RigTForm(arcballQuat), g_skyRbt, g_objectRbt[1]);

    }
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
    
    if(tapCount == 5){
        writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
    }
    
    mouse(x,y,tapCount,pressStatus);
}
