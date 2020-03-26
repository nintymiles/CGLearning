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
#include "MoreTeapotsModel.h"



using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////

static const float g_frustMinFov = 60.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -50.0;    // far plane

static int g_windowWidth = 512;
static int g_windowHeight = 512;
static float g_arcballScreenRadius = 0.5 * (float)min(g_windowWidth,g_windowHeight);

static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton, g_quadTap;
static int g_mouseClickX, g_mouseClickY; // coordinates for mouse click event
static int g_activeShader = 0;


static vector<shared_ptr<ShaderState> > g_shaderStates; // our global shader states

// --------- Geometry


// Vertex buffer and index buffer associated with the ground and cube geometry
static shared_ptr<Geometry> g_ground, g_cube,g_sphere;

// --------- Scene

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space
//static Matrix4 g_skyRbt = Matrix4::makeTranslation(Cvec3(0.0, 0.25, 4.0));
static RigTForm g_skyRbt = RigTForm(Cvec3(0.0, 0.25, 4.0));
//static Matrix4 g_objectRbt[2] = {Matrix4::makeTranslation(Cvec3(0,0,0)),Matrix4::makeTranslation(Cvec3(0,0,-2))};
static RigTForm g_objectRbt[2] = {RigTForm(Cvec3(0.5,0.5,0.5)),RigTForm(Cvec3(0,0,0))};

static Cvec4f g_objectColors[2] = {Cvec4f(1, 0, 0, 1),Cvec4f(0.5, 0, 0.5, 0.3)};
static const Cvec3 g_objectFrameOrigin = Cvec3(0,-0.25,-4.0);


///////////////// END OF G L O B A L S //////////////////////////////////////////////////
shared_ptr<MoreTeapotsModel> moreTeapotsModel;

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
    moreTeapotsModel->UpdateViewport();
    moreTeapotsModel->Render(1.0, 0.0, 0.0);
}

static void display() {
    //glUseProgram(g_shaderStates[g_activeShader]->program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clear framebuffer color&depth
    
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
    else if (g_quadTap){
        
    }
    
    if (g_mouseClickDown) {
        //g_objectRbt[0] = g_objectRbt[0] * m; // Simply right-multiply is WRONG
        RigTForm auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
        g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], auxiliaryRbt);
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
//    rotation_angle_alpha += .02f;
//    if(rotation_angle_alpha >= 1.f)
//        rotation_angle_alpha = 0.f;
//    
//    RigTForm eyeRbt = g_skyRbt;
//    RigTForm aRbt = makeMixedFrame(g_objectRbt[0],eyeRbt);
//    
//    Quat q1Quat = Quat::makeYRotation(rotation_angle_slerp_start) ;
//    Quat q2Quat = Quat::makeYRotation(rotation_angle_slerp_end) ;
//    //examine orientation interoperation by slerp and lerp
//    Quat slerpQuat = slerp(q1Quat, q2Quat, rotation_angle_alpha);
//    //若要插值动画循环播放，每次施加在base object frame（g_slerpBaseRbt)
//    g_objectRbt[1] = doQtoOwrtA(RigTForm(slerpQuat), g_slerpBaseRbt, aRbt);
    
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
    moreTeapotsModel.reset(new MoreTeapotsModel());
    moreTeapotsModel->Init(6,6,1);
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
