////////////////////////////////////////////////////////////////////////
//
//   Harvard Computer Science
//   CS 175: Computer Graphics
//   Professor Steven Gortler
//
////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ppm.h"
#include "glsupport.h"

using namespace std;      // for string, vector, iostream and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////
static GLFWwindow* window;

//--------------------------------------------------------------------------------
//  GLFW global variables
//--------------------------------------------------------------------------------
GLfloat cursor_x        = 0.f;
GLfloat cursor_y        = 0.f;


// !!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!
// Before you start working on this assignment, set the following variable properly
// to indicate whether you want to use OpenGL 2.x with GLSL 1.0 or OpenGL 3.x+ with
// GLSL 1.3.
//
// Set g_Gl2Compatible = true to use GLSL 1.0 and g_Gl2Compatible = false to use GLSL 1.3.
// Make sure that your machine supports the version of GLSL you are using. In particular,
// on Mac OS X currently there is no way of using OpenGL 3.x with GLSL 1.3 when
// GLUT is used.
//
// If g_Gl2Compatible=true, shaders with -gl2 suffix will be loaded.
// If g_Gl2Compatible=false, shaders with -gl3 suffix will be loaded.
// To complete the assignment you only need to edit the shader files that get loaded
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static const bool g_Gl2Compatible = false;


static int g_width             = 512;       // screen width
static int g_height            = 512;       // screen height
static bool g_leftClicked      = false;     // is the left mouse button down?
static bool g_rightClicked     = false;     // is the right mouse button down?
static float g_objScale        = 1.0;       // scale factor for object
static int g_leftClickX, g_leftClickY;      // coordinates for mouse left click event
static int g_rightClickX, g_rightClickY;    // coordinates for mouse right click event

struct ShaderState {
    GlProgram program;
    
    // Handles to uniform variables
    GLint h_uVertexScale;
    GLint h_uTexUnit0, h_uTexUnit1;
    
    // Handles to vertex attributes
    GLint h_aPosition;
    GLint h_aColor;
    GLint h_aTexCoord0, h_aTexCoord1;
    
    ShaderState(const char* vsfn, const char* fsfn) {
        readAndCompileShader(program, vsfn, fsfn);
        
        const GLuint h = program; // short hand
        
        // Retrieve handles to uniform variables
        h_uVertexScale = safe_glGetUniformLocation(h, "uVertexScale");
        h_uTexUnit0 = safe_glGetUniformLocation(h, "uTexUnit0");
        h_uTexUnit1 = safe_glGetUniformLocation(h, "uTexUnit1");
        
        // Retrieve handles to vertex attributes
        h_aPosition = safe_glGetAttribLocation(h, "aPosition");
        h_aColor = safe_glGetAttribLocation(h, "aColor");
        h_aTexCoord0 = safe_glGetAttribLocation(h, "aTexCoord0");
        h_aTexCoord1 = safe_glGetAttribLocation(h, "aTexCoord1");
        
        if (!g_Gl2Compatible)
            glBindFragDataLocation(h, 0, "fragColor");
        checkGlError(__func__);
    }
};

static const int g_numShaders = 1;
static const char * const g_shaderFiles[g_numShaders][2] = {
    {"./shaders/asst1-gl3.vshader", "./shaders/asst1-gl3.fshader"}
};
static const char * const g_shaderFilesGl2[g_numShaders][2] = {
    {"./shaders/asst1-gl2.vshader", "./shaders/asst1-gl2.fshader"}
};
static vector<shared_ptr<ShaderState> > g_shaderStates; // our global shader states

static shared_ptr<GlTexture> g_tex0, g_tex1; // our global texture instance

struct SquareGeometry {
    GlBufferObject posVbo, texVbo, colVbo;
    
    SquareGeometry() {
        static GLfloat sqPos[12] = {
            -.5, -.5,
            .5,  .5,
            .5,  -.5,
            
            -.5, -.5,
            -.5, .5,
            .5,  .5
        };
        
        static GLfloat sqTex[12] = {
            0, 0,
            1, 1,
            1, 0,
            
            0, 0,
            0, 1,
            1, 1
        };
        
        static GLfloat sqCol[18] =  {
            1, 0, 0,
            0, 1, 1,
            0, 0, 1,
            
            1, 0, 0,
            0, 1, 0,
            0, 1, 1
        };
        
        glBindBuffer(GL_ARRAY_BUFFER, posVbo);
        glBufferData(
                     GL_ARRAY_BUFFER,
                     12*sizeof(GLfloat),
                     sqPos,
                     GL_STATIC_DRAW);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, texVbo);
        glBufferData(
                     GL_ARRAY_BUFFER,
                     12*sizeof(GLfloat),
                     sqTex,
                     GL_STATIC_DRAW);
        
        glBindBuffer(GL_ARRAY_BUFFER, colVbo);
        glBufferData(
                     GL_ARRAY_BUFFER,
                     18*sizeof(GLfloat),
                     sqCol,
                     GL_STATIC_DRAW);
    }
    
    //对于绘制调用，如果没有VAO对象，至少OpenGL4.x版本是无法进行绘制调用，返回0x520的错误。
    void draw(const ShaderState& curSS) {
        GLuint vao;
        glGenVertexArrays( 1, &vao );
        glBindVertexArray(vao);
        int numverts=6;
        safe_glEnableVertexAttribArray(curSS.h_aPosition);
        safe_glEnableVertexAttribArray(curSS.h_aTexCoord0);
        safe_glEnableVertexAttribArray(curSS.h_aTexCoord1);
        safe_glEnableVertexAttribArray(curSS.h_aColor);
        
        glBindBuffer(GL_ARRAY_BUFFER, posVbo);
        safe_glVertexAttribPointer(curSS.h_aPosition,
                                   2, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, texVbo);
        safe_glVertexAttribPointer(curSS.h_aTexCoord0,
                                   2, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, texVbo);
        safe_glVertexAttribPointer(curSS.h_aTexCoord1,
                                   2, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, colVbo);
        safe_glVertexAttribPointer(curSS.h_aColor,
                                   3, GL_FLOAT, GL_FALSE, 0, 0);
        
        glDrawArrays(GL_TRIANGLES,0,numverts);
        
        safe_glDisableVertexAttribArray(curSS.h_aPosition);
        safe_glDisableVertexAttribArray(curSS.h_aColor);
        safe_glDisableVertexAttribArray(curSS.h_aTexCoord0);
        safe_glDisableVertexAttribArray(curSS.h_aTexCoord1);
        glBindVertexArray(0);
    }
};


static shared_ptr<SquareGeometry> g_square; // our global geometries


// C A L L B A C K S ///////////////////////////////////////////////////


// _____________________________________________________
//|                                                     |
//|  display                                            |
//|_____________________________________________________|
///
///  Whenever OpenGL requires a screen refresh
///  it will call display() to draw the scene.
///  We specify that this is the correct function
///  to call with the glutDisplayFunc() function
///  during initialization

static void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    const ShaderState& curSS = *g_shaderStates[0];
    glUseProgram(curSS.program);
    
    safe_glUniform1i(curSS.h_uTexUnit0, 0);
    safe_glUniform1i(curSS.h_uTexUnit1, 1);
    safe_glUniform1f(curSS.h_uVertexScale, g_objScale);
    g_square->draw(curSS);
    
    glfwSwapBuffers(window);
    // check for errors
    checkGlError(__func__);
    
}


// _____________________________________________________
//|                                                     |
//|  reshape                                            |
//|_____________________________________________________|
///
///  Whenever a window is resized, a "resize" event is
///  generated and glut is told to call this reshape
///  callback function to handle it appropriately.
//static void reshape(int w, int h) {
//  g_width = w;
//  g_height = h;
//  glViewport(0, 0, w, h);
//  //glutPostRedisplay();
//}
static void reshape(GLFWwindow* window,const int w, const int h) {
    g_width = w;
    g_height = h;
    glViewport(0, 0, w, h);
    cerr << "Size of window is now " << w << "x" << h << endl;
    //glutPostRedisplay();
}


// _____________________________________________________
//|                                                     |
//|  mouse                                              |
//|_____________________________________________________|
///
///  Whenever a mouse button is clicked, a "mouse" event
///  is generated and this mouse callback function is
///  called to handle the user input.

//static void mouse(int button, int state, int x, int y) {
//  if (button == GLUT_LEFT_BUTTON) {
//    if (state == GLUT_DOWN) {
//      // right mouse button has been clicked
//      g_leftClicked = true;
//      g_leftClickX = x;
//      g_leftClickY = g_height - y - 1;
//    }
//    else {
//      // right mouse button has been released
//      g_leftClicked = false;
//    }
//  }
//  if (button == GLUT_RIGHT_BUTTON) {
//    if (state == GLUT_DOWN) {
//      // right mouse button has been clicked
//      g_rightClicked = true;
//      g_rightClickX = x;
//      g_rightClickY = g_height - y - 1;
//    }
//    else {
//      // right mouse button has been released
//      g_rightClicked = false;
//    }
//  }
//}

static void mouse(GLFWwindow* window, const int button, const int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // right mouse button has been clicked
            g_leftClicked = true;
            g_leftClickX = cursor_x;
            g_leftClickY = g_height - cursor_y - 1;
        }
        else {
            // right mouse button has been released
            g_leftClicked = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            // right mouse button has been clicked
            g_rightClicked = true;
            g_rightClickX = cursor_x;
            g_rightClickY = g_height - cursor_y - 1;
        }
        else {
            // right mouse button has been released
            g_rightClicked = false;
        }
    }
    
}

void cursor_position_callback( GLFWwindow* window, double x, double y )
{
    cursor_x = (float) x;
    cursor_y = (float) y;
}


// _____________________________________________________
//|                                                     |
//|  motion                                             |
//|_____________________________________________________|
///
///  Whenever the mouse is moved while a button is pressed,
///  a "mouse move" event is triggered and this callback is
///  called to handle the event.

static void motion(int x, int y) {
    const int newx = x;
    const int newy = g_height - y - 1;
    if (g_leftClicked) {
        g_leftClickX = newx;
        g_leftClickY = newy;
    }
    if (g_rightClicked) {
        float deltax = (newx - g_rightClickX) * 0.02;
        g_objScale += deltax;
        
        g_rightClickX = newx;
        g_rightClickY = newy;
    }
    //glutPostRedisplay();
}

//void keyboard(unsigned char key, int x, int y) {
//  switch (key) {
//  case 'h':
//    cout << " ============== H E L P ==============\n\n"
//    << "h\t\thelp menu\n"
//    << "s\t\tsave screenshot\n"
//    << "drag right mouse to change square size\n";
//    break;
//  case 'q':
//    exit(0);
//  case 's':
//    glFinish();
//    writePpmScreenshot(g_width, g_height, "out.ppm");
//    break;
//  }
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
                << "drag right mouse to change square size\n";
                break;
            case GLFW_KEY_S:
                glFinish();
                writePpmScreenshot(g_width, g_height, "out.ppm");
                break;
        }
    }
}

static void displayWindow(GLFWwindow* window){
    display();
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


// H E L P E R    F U N C T I O N S ////////////////////////////////////


//static void initGlutState(int argc, char **argv) {
//  glutInit(&argc,argv);
//  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
//  glutInitWindowSize(g_width, g_height);      // create a window
//  glutCreateWindow("CS 175: Hello World");    // title the window
//
//  glutDisplayFunc(display);                   // display rendering callback
//  glutReshapeFunc(reshape);                   // window reshape callback
//  glutMotionFunc(motion);                     // mouse movement callback
//  glutMouseFunc(mouse);                       // mouse click callback
//  glutKeyboardFunc(keyboard);
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
    window = glfwCreateWindow(g_width, g_height, "GLFW Based OpenGL Application - H2D", NULL, NULL);
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
    glClearColor(128./255,200./255,1,0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if (!g_Gl2Compatible)
        glEnable(GL_FRAMEBUFFER_SRGB);
}

static void initShaders() {
    g_shaderStates.resize(g_numShaders);
    for (int i = 0; i < g_numShaders; ++i) {
        if (g_Gl2Compatible)
            g_shaderStates[i].reset(new ShaderState(g_shaderFilesGl2[i][0], g_shaderFilesGl2[i][1]));
        else
            g_shaderStates[i].reset(new ShaderState(g_shaderFiles[i][0], g_shaderFiles[i][1]));
    }
}

static void initGeometry() {
    g_square.reset(new SquareGeometry());
}

static void loadTexture(GLuint texHandle, const char *ppmFilename) {
    int texWidth, texHeight;
    vector<PackedPixel> pixData;
    
    ppmRead(ppmFilename, texWidth, texHeight, pixData);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, g_Gl2Compatible ? GL_RGB : GL_SRGB, texWidth, texHeight,
                 0, GL_RGB, GL_UNSIGNED_BYTE, &pixData[0]);
    checkGlError(__func__);
}


static void initTextures() {
    g_tex0.reset(new GlTexture());
    g_tex1.reset(new GlTexture());
    
    loadTexture(*g_tex0, "./shaders/smiley.ppm");
    loadTexture(*g_tex1, "./shaders/reachup.ppm");
    
    glActiveTexture(GL_TEXTURE0);
    checkGlError("activeTexture");
    glBindTexture(GL_TEXTURE_2D, *g_tex0);
    checkGlError("bindTexture");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    //如此处参数设置的GL_Enum值超出
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    checkGlError("TextureParameter1");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    checkGlError("TextureParameter2");
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, *g_tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}


// M A I N /////////////////////////////////////////////////////////////

// _____________________________________________________
//|                                                     |
//|  main                                               |
//|_____________________________________________________|
///
///  The main entry-point for the HelloWorld example
///  application.

int main(int argc, char **argv) {
    try {
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
        
        initShaders();
        initGeometry();
        checkGlError("after initGeometry");
        initTextures();
        checkGlError("after initTextures");
        
        glfwSetWindowRefreshCallback(window, displayWindow);
        
        //OpenGL_Helper::PerfMonitor perfMonitor;
        //float fps;
        
        //mac os x的窗口调用会限制帧数
        while( !glfwWindowShouldClose(window) ){
            //display();
            //        perfMonitor.Update(fps);
            //        cout << "Current FPS at " << glfwGetTime() << " is " << fps << endl;
            
            motion(cursor_x, cursor_y);
            
            //交换缓存可放到绘制完成后
            // glfwSwapBuffers( window );
            //glfwPollEvents();
            glfwWaitEvents();
        }
        
        
        glfwTerminate();
        
        return 0;
    }catch (const runtime_error& e) {
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
    
}


