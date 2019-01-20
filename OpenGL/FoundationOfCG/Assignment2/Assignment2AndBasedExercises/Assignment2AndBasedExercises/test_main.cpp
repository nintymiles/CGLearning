#define GLFW_INCLUDE_GLCOREARB // 使得GLFW包含 glfw3.h 头文件，否则为 glfw.h
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
using namespace std;

//--------------------------------------------------------------------------------
//  Simple Triangle for OpenGL 4 Code is under below
//--------------------------------------------------------------------------------



void CheckStatus( GLuint obj )
{
    GLint status = GL_FALSE;
    if( glIsShader(obj) ) glGetShaderiv( obj, GL_COMPILE_STATUS, &status );
    if( glIsProgram(obj) ) glGetProgramiv( obj, GL_LINK_STATUS, &status );
    if( status == GL_TRUE ) return;
    GLchar log[ 1 << 16 ] = { 0 };
    if( glIsShader(obj) ) glGetShaderInfoLog( obj, sizeof(log), NULL, log );
    if( glIsProgram(obj) ) glGetProgramInfoLog( obj, sizeof(log), NULL, log );
    std::cerr << log << std::endl;
    exit( -1 );
}

void AttachShader( GLuint program, GLenum type, const char* src )
{
    GLuint shader = glCreateShader( type );
    glShaderSource( shader, 1, &src, NULL );
    glCompileShader( shader );
    CheckStatus( shader );
    glAttachShader( program, shader );
    glDeleteShader( shader );
}

GLuint LoadProgram( const char* vert, const char* geom, const char* frag )
{
    GLuint prog = glCreateProgram();
    if( vert ) AttachShader( prog, GL_VERTEX_SHADER, vert );
    if( geom ) AttachShader( prog, GL_GEOMETRY_SHADER, geom );
    if( frag ) AttachShader( prog, GL_FRAGMENT_SHADER, frag );
    glLinkProgram( prog );
    CheckStatus( prog );
    return prog;
}

#define GLSL(version, shader) "#version " #version "\n" #shader


GLuint program;
GLuint VAO;
void initData()
{
    const char* vert = GLSL
    (
     410 core,
     layout( location = 0 ) in vec2 position;
     void main()
     {
     gl_Position = vec4( position, 0.0, 1.0 );
     }
     );
    
    const char* frag = GLSL
    (
     410 core,
     out vec4 FragColor;
     void main()
     {
     FragColor = vec4( 0.6, 0.6, 1.0, 0.5 );
     }
     );
    
    program = LoadProgram( vert, NULL, frag );
    
    glGenVertexArrays( 1, &VAO );
    glBindVertexArray( VAO );
    
    GLuint vertex_buffer = 0;
    glGenBuffers( 1, &vertex_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
    float data[] =
    {
    0.0f,0.8f,
    -0.8f, 0.0f,
    0.8f,0.0f
    };
    glBufferData( GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW );
    
    GLuint index_buffer = 0;
    glGenBuffers( 1, &index_buffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer );
    unsigned int indexes[] =
    {
    0,1,2
    };
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW );
    
    glEnableVertexAttribArray( 0 );
    
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );
    
    glBindVertexArray( 0 );
}

//int main( int argc, char** argv )
//{
//    if( !glfwInit() )
//        return -1;
    
//    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
//    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
//    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
//    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
//
//    GLFWwindow* window = glfwCreateWindow( 640, 480, "Hello window", NULL, NULL );
//    if( !window )
//        {
//        cerr << "Error on window creating" << endl;
//        glfwTerminate();
//        return -1;
//        }
//
//    glfwMakeContextCurrent( window );
//
//    cout << "GLFW version                : " << glfwGetVersionString() << endl;
//    cout << "GL_VERSION                  : " << glGetString( GL_VERSION ) << endl;
//    cout << "GL_VENDOR                   : " << glGetString( GL_VENDOR ) << endl;
//    cout << "GL_RENDERER                 : " << glGetString( GL_RENDERER ) << endl;
//    cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << endl;
//
//
//    initData();
//    while( !glfwWindowShouldClose(window) )
//        {
//        glClearColor( 0, 0, 0, 0 );
//        glClear( GL_COLOR_BUFFER_BIT );
//
//        glUseProgram( program );
//        glBindVertexArray( VAO );
//        glDrawElements( GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0 );
//        glBindVertexArray( 0 );
//
//        glfwSwapBuffers( window );
//
//        glfwPollEvents();
//        }
//
//    glfwTerminate();
//    return 0;
//}
