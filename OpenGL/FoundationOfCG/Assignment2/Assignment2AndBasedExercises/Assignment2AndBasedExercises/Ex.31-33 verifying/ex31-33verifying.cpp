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
#include "geometrymaker.h"
#include "ppm.h"
#include "glsupport.h"

#include "perfMonitor.h"

using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////

//indict whether to use source directly
static const bool g_GlSourceFlag = false;


static const float g_frustMinFov = 60.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -50.0;    // far plane
static const float g_groundY = -2.0;      // y coordinate of the ground
static const float g_groundSize = 10.0;   // half the ground length

static int g_windowWidth = 512;
static int g_windowHeight = 1024;
static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton;
static int g_mouseClickX, g_mouseClickY; // coordinates for mouse click event
static int g_activeShader = 1;
static int g_activeCube = 1;

static GLFWwindow* window;


struct ShaderState {
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

  ShaderState(const char* vsfn, const char* fsfn,bool sourceFlag) {
      if(sourceFlag)
          loadAndCompileShader(program, vsfn, fsfn);
      else
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

    checkGlError(__FUNCTION__);
  }

};

static const int g_numShaders = 2;

static const char * const g_shaderFiles[g_numShaders][2] = {
  {"./shaders/basic-gl3.vshader", "./shaders/solid-gl3.fshader"},
  {"./shaders/basic-gl3.vshader", "./shaders/diffuse-gl3.fshader"}
};

const char* basicVert = GLSL
(
 410 core,
 
 uniform mat4 uProjMatrix;
 uniform mat4 uModelViewMatrix;
 uniform mat4 uNormalMatrix;
 uniform float uXCoordOffset;
 
 in vec3 aPosition;
 in vec3 aNormal;
 
 out vec3 vNormal;
 out vec3 vPosition;
 
 void main() {
     vNormal = vec3(uNormalMatrix * vec4(aNormal, 0.0));
     
     // send position (eye coordinates) to fragment shader
     vec4 tPosition = uModelViewMatrix * vec4(aPosition, 1.0);
     vPosition = vec3(tPosition);
     gl_Position = uProjMatrix * tPosition; 
     gl_Position.x += uXCoordOffset;
 }
 
 );

const char* solidVert = GLSL
(
 410 core,
 
 uniform vec3 uColor;
 
 out vec4 fragColor;
 
 void main() {
     fragColor = vec4(uColor, 1.0);
 }
 );

const char* diffuseVert = GLSL
(
 410 core,
 
 uniform vec3 uLight;
 uniform vec3 uLight2;
 uniform vec3 uColor;
 
 in vec3 vNormal;
 in vec3 vPosition;
 
 out vec4 fragColor;
 
 void main() {
     vec3 tolight = normalize(uLight - vPosition);
     vec3 tolight2 = normalize(uLight2 - vPosition);
     vec3 normal = normalize(vNormal);
     
     float diffuse = max(0.0, dot(normal, tolight));
     diffuse += max(0.0, dot(normal, tolight2));
     vec3 intensity = uColor * diffuse;
     
     fragColor = vec4(intensity, 1.0f);
 }
 );

static const char * const g_shaderSources[g_numShaders][2] = {
  {basicVert, diffuseVert},
  {basicVert, solidVert}
};

//static const char * const g_shaderFilesGl2[g_numShaders][2] = {
//  {"./shaders/basic-gl2.vshader", "./shaders/diffuse-gl2.fshader"},
//  {"./shaders/basic-gl2.vshader", "./shaders/solid-gl2.fshader"}
//};

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


// Vertex buffer and index buffer associated with the ground and cube geometry
static shared_ptr<Geometry> g_ground, g_cube;

// --------- Scene

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space
static Matrix4 g_skyRbt = Matrix4::makeTranslation(Cvec3(0.0, 0.25, 4.0));
//初始tramsformation，将object frame的原点保持不动，每个cube使用一个object matrix。由于在shader中使用了offset，故此处对象帧的起点都为原点。
static Matrix4 g_objectRbt[2] = {Matrix4::makeTranslation(Cvec3(-1,0,0)),Matrix4::makeTranslation(Cvec3(1,0,0))};
static Cvec3f g_objectColors[1] = {Cvec3f(1, 0, 0)};
static Matrix4 g_auxiliaryRbt;

///////////////// END OF G L O B A L S //////////////////////////////////////////////////

//--------------------------------------------------------------------------------
//  GLFW global variables
//--------------------------------------------------------------------------------
GLfloat cursor_x        = 0.f;
GLfloat cursor_y        = 0.f;

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

// takes a projection matrix and send to the the shaders
static void sendProjectionMatrix(const ShaderState& curSS, const Matrix4& projMatrix) {
  GLfloat glmatrix[16];
    Matrix4 scaleMatrix =Matrix4::makeScale(Cvec3(3,3,3));
    scaleMatrix(3,3) = 3.0;
//    Matrix4 contractMatrix = Matrix4::makeScale(Cvec3(1,1,1));
//    contractMatrix(3,3) = 1.0;
    Matrix4 projTMatrix =  projMatrix * scaleMatrix  ;
  projTMatrix.writeToColumnMajorMatrix(glmatrix); // send projection matrix
  safe_glUniformMatrix4fv(curSS.h_uProjMatrix, glmatrix);
}

// takes MVM and its normal matrix to the shaders
static void sendModelViewNormalMatrix(const ShaderState& curSS, const Matrix4& MVM, const Matrix4& NMVM) {
  GLfloat glmatrix[16];
  MVM.writeToColumnMajorMatrix(glmatrix); // send MVM
  safe_glUniformMatrix4fv(curSS.h_uModelViewMatrix, glmatrix);

  NMVM.writeToColumnMajorMatrix(glmatrix); // send NMVM
  safe_glUniformMatrix4fv(curSS.h_uNormalMatrix, glmatrix);
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

static void drawStuff() {
    // short hand for current shader state
    const ShaderState& curSS = *g_shaderStates[g_activeShader];
    
    // build & send proj. matrix to vshader
    const Matrix4 projmat = makeProjectionMatrix();
    sendProjectionMatrix(curSS, projmat);
    
    // use the skyRbt as the eyeRbt
    const Matrix4 eyeRbt = g_skyRbt;
    const Matrix4 invEyeRbt = inv(eyeRbt);
    
    const Cvec3 eyeLight1 = Cvec3(invEyeRbt * Cvec4(g_light1, 1)); // g_light1 position in eye coordinates
    const Cvec3 eyeLight2 = Cvec3(invEyeRbt * Cvec4(g_light2, 1)); // g_light2 position in eye coordinates
    safe_glUniform3f(curSS.h_uLight, eyeLight1[0], eyeLight1[1], eyeLight1[2]);
    safe_glUniform3f(curSS.h_uLight2, eyeLight2[0], eyeLight2[1], eyeLight2[2]);
    
    // draw ground
    // ===========
    //
    safe_glUniform1f(curSS.h_uXCoordOffset, 0.f);
    const Matrix4 groundRbt = Matrix4();  // identity
    Matrix4 MVM = invEyeRbt * groundRbt;
    Matrix4 NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, 0.1, 0.95, 0.1); // set color
    g_ground->draw(curSS);
    
    // draw cubes
    // ==========
    safe_glUniform1f(curSS.h_uXCoordOffset, 0.f);
    MVM = invEyeRbt * g_objectRbt[0];
    NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, g_objectColors[0][0], g_objectColors[0][1], g_objectColors[0][2]);
    g_cube->draw(curSS);
    
    safe_glUniform1f(curSS.h_uXCoordOffset, 0.f);
    MVM = invEyeRbt * g_objectRbt[1];
    NMVM = normalMatrix(MVM);
    sendModelViewNormalMatrix(curSS, MVM, NMVM);
    safe_glUniform3f(curSS.h_uColor, g_objectColors[0][1], g_objectColors[0][0], g_objectColors[0][1]);
    g_cube->draw(curSS);
    
}

static void display() {
  glUseProgram(g_shaderStates[g_activeShader]->program);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                   // clear framebuffer color&depth

  drawStuff();

//   show the back buffer (where we rendered stuff)
//  glfwSwapBuffers(window);

//  checkGlErrors();
}

static void reshape(GLFWwindow* window,const int w, const int h) {
  g_windowWidth = w;
  g_windowHeight = h;
  glViewport(0, 0, w, h);
  cerr << "Size of window is now " << w << "x" << h << endl;
  updateFrustFovY();
  //glutPostRedisplay();
}

static void displayWindow(GLFWwindow* window){
    display();
}

static void motion(const float x, const float y) {
  const double dx = x - g_mouseClickX;
  const double dy = g_windowHeight - y - 1 - g_mouseClickY;

  Matrix4 m;
  if (g_mouseLClickButton && !g_mouseRClickButton) { // left button down?
    m = Matrix4::makeXRotation(-dy) * Matrix4::makeYRotation(dx);
  }
  else if (g_mouseRClickButton && !g_mouseLClickButton) { // right button down?
    m = Matrix4::makeTranslation(Cvec3(dx, dy, 0) * 0.01);
  }
  else if (g_mouseMClickButton || (g_mouseLClickButton && g_mouseRClickButton)) {  // middle or (left and right) button down?
    m = Matrix4::makeTranslation(Cvec3(0, 0, -dy) * 0.01);
  }

    
  if (g_mouseClickDown) {
      if(g_activeCube == 0){
          //g_objectRbt[0] *= m; // Simply right-multiply is WRONG
          g_auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
          g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], g_auxiliaryRbt);
      }else{
          g_auxiliaryRbt = makeMixedFrame(g_objectRbt[1], g_skyRbt);
          g_objectRbt[1] = doQtoOwrtA(m, g_objectRbt[1], g_auxiliaryRbt);
      }
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
                << "v\t\tCycle view\n"
                << "drag left mouse to rotate\n" << endl;
                break;
            case GLFW_KEY_S:
                glFlush();
                writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
                break;
            case GLFW_KEY_O:
                g_activeCube ^= 1;
                break;
            case GLFW_KEY_F:
                g_activeShader ^= 1;
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
    window = glfwCreateWindow(g_windowWidth, g_windowHeight, "GLFW Based OpenGL Application", NULL, NULL);
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
    if (g_GlSourceFlag)
      g_shaderStates[i].reset(new ShaderState(g_shaderSources[i][0], g_shaderSources[i][1],g_GlSourceFlag));
    else
      g_shaderStates[i].reset(new ShaderState(g_shaderFiles[i][0], g_shaderFiles[i][1],g_GlSourceFlag));
  }
}

static void initGeometry() {
  initGround();
  initCubes();
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
      
      glfwSetWindowRefreshCallback(window, displayWindow);
      
      OpenGL_Helper::PerfMonitor perfMonitor;
      float fps;

      //mac os x的窗口调用会限制帧数
      while( !glfwWindowShouldClose(window) ){
          display();
          perfMonitor.Update(fps);
          cout << "Current FPS at " << glfwGetTime() << " is " << fps << endl;
          
          motion(cursor_x, cursor_y);
          
          //exchange between front and back buffer
          glfwSwapBuffers( window );
          //glfwPollEvents();
          glfwWaitEvents();
      }
      
      
      glfwTerminate();

    return 0;
  }
  catch (const runtime_error& e) {
    cout << "Exception caught: " << e.what() << endl;
    return -1;
  }
}
