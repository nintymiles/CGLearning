//--------------------------------------------------------------------------------
// TexturedTeapotModel.cpp
// Render a textured utah teapot
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TexturedTeapotModel.h"

//--------------------------------------------------------------------------------
// Teapot model data
//--------------------------------------------------------------------------------
#include "../teapot.inl"
#include "matrix4.h"
#include "Geometry.h"
#include "Texture.h"
#include "geometrymaker.h"

//Note TARGET_OS_MAC includes any apple OS,TARGET_OS_IPHONE includes any device/simulator run on iOS
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_OSX   //定位OSX系统的macro
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#elif TARGET_OS_IPHONE //用于定位IPHONE系统的macro
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif
#endif



//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
TexturedTeapotModel::TexturedTeapotModel() {}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
TexturedTeapotModel::~TexturedTeapotModel(){
    Unload();
}

/**
 * Report type of teapot we are rendering. This is the only place
 * to decide what kind of teapot to render.
 *
 * @return
 *   GL_TEXTURE_CUBE_MAP if you want to render cubemaped teapot
 *   GL_TEXTURE_2D if just to render a 2D textured teapot
 *   GL_INVALID_VALUE no texture for teapot
 */
GLint TexturedTeapotModel::GetTextureType(void) {
    return
        GL_TEXTURE_CUBE_MAP;
//    GL_TEXTURE_2D;
    //            GL_INVALID_VALUE;
}



void TexturedTeapotModel::Init() {
    // Settings
    glFrontFace(GL_CCW);
    

    
    // Load shader
    teapotShaderState_.reset(new TexturedTeapotShaderState());

    // Create Index buffer
    num_indices_ = sizeof(teapotIndices) / sizeof(teapotIndices[0]);
    // Create VBO
    num_vertices_ = sizeof(teapotPositions) / sizeof(teapotPositions[0]) / 3;

//    int ibLen, vbLen;
//    getCubeVbIbLen(vbLen, ibLen);
//
//    // Temporary storage for cube Geometry
//    vector<VertexPNX> vtx(vbLen);
//    vector<unsigned short> idx(ibLen);
//
//    makeCube(1, vtx.begin(), idx.begin());
//
//    num_indices_ = ibLen;
//    // Create VBO
//    num_vertices_ = vbLen;
//    geometry_.reset(new Geometry(&vtx[0], &idx[0], num_vertices_, num_indices_));

    int32_t index = 0;
    //int32_t tIndex = 0;
    VertexPNX* p = new VertexPNX[num_vertices_];
    for (int32_t i = 0; i < num_vertices_; ++i) {
        p[i].p[0] = teapotPositions[index];
        p[i].p[1] = teapotPositions[index + 1];
        p[i].p[2] = teapotPositions[index + 2];

        p[i].n[0] = teapotNormals[index];
        p[i].n[1] = teapotNormals[index + 1];
        p[i].n[2] = teapotNormals[index + 2];

        //tiled texture coord
        p[i].x[0] = teapotTexCoords[index];
        p[i].x[1] = teapotTexCoords[index + 1];

        //        //non tiled texture coord
        //        p[i].x[0] = teapotTexCoords[3*index]/2.0;
        //        p[i].x[1] = teapotTexCoords[3*index + 1]/2.0;

        index += 3;

        //
        //        p[i].x[0] = teapotTexCoords[3*i]/2.0;
        //        p[i].x[1] = teapotTexCoords[3*i + 1]/2.0;
    }

    geometry_.reset(new Geometry(p, teapotIndices, num_vertices_, num_indices_));

    delete[] p;
    
    
    GLint type = GetTextureType();
    // Need flip Y, so as top/bottom image
//    std::vector<std::string> textures {
//        std::string("./textures/left.tga"),   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
//        std::string("./textures/right.tga"),  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
//        std::string("./textures/bottom.tga"), // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
//        std::string("./textures/top.tga"),    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
//        std::string("./textures/front.tga"),  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
//        std::string("./textures/back.tga")    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
//    };
    
    std::vector<std::string> textures {
        std::string("./textures/posx.jpg"),  // GL_TEXTURE_CUBE_MAP_POSITIVE_X
        std::string("./textures/negx.jpg"),   // GL_TEXTURE_CUBE_MAP_NEGATIVE_X
        std::string("./textures/posy.jpg"),    // GL_TEXTURE_CUBE_MAP_POSITIVE_Y
        std::string("./textures/negy.jpg"), // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
        std::string("./textures/posz.jpg"),  // GL_TEXTURE_CUBE_MAP_POSITIVE_Z
        std::string("./textures/negz.jpg")    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };
    
    if(type == GL_TEXTURE_2D) {
        textures[0] = std::string("./textures/greenleaves.png");
    }
    
    texObj_ = Texture::Create(type, textures);
    assert(texObj_);
    
    std::vector<std::string> samplers;
    std::vector<GLint> units;
    texObj_->GetActiveSamplerInfo(samplers, units);
    for(size_t idx = 0; idx < samplers.size(); idx++) {
        GLint sampler = glGetUniformLocation(teapotShaderState_->program,
                                             samplers[idx].c_str());
        glUniform1i(sampler, units[idx]);
    }
    
    texObj_->Activate();
    
    if(!vao_)
    glGenVertexArrays( 1, &vao_ );
    
    glBindVertexArray(vao_);
    
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    
    glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(ATTRIB_NORMAL);
    
    glVertexAttribPointer(ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)+ 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(ATTRIB_UV);
    
    glBindVertexArray(0);
    
    //  UpdateViewport();
    mat_local_model_ = Matrix4::makeTranslation(Cvec3(0, 0, -80.f));
    
    mat_local_model_ =  mat_local_model_ * Matrix4::makeXRotation(-70);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,10.0f,4.0f));
    mat_view_ = inv(mat_view_);
    
    UpdateViewport();
}

void TexturedTeapotModel::UpdateViewport() {
    // Init Projection matrices
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10000.f;
    
    
    float fov = 60;
    float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =Matrix4::makeProjection(fov, aspect, CAM_NEAR, CAM_FAR);
}

void TexturedTeapotModel::Unload() {
    
}

void TexturedTeapotModel::Update(double time) {
    if (motionControl_) {
        motionControl_->Update();
        //mat_view_ = appCamera_->GetTransformMatrix() * mat_view_ *
        //appCamera_->GetRotationMatrix();
        //mat_view_ =  mat_view_ * appCamera_->GetRotationMatrix();
        
        mat_motion_matrix_ =  motionControl_->GetRotationMatrix();
    }
}

void TexturedTeapotModel::SetMotionControl(std::shared_ptr<MotionControl> mControl){
    motionControl_ = mControl;
}

void TexturedTeapotModel::Render(float r, float g, float b) {
    
    //mat_model_ = mat_model_ * Matrix4::makeZRotation(0.5);
    mat_model_ = mat_local_model_ * mat_motion_matrix_;
    
    Matrix4 mat_mv = mat_view_ * mat_model_;
    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_vp = mat_projection_ * mat_mv;
    
    
    glUseProgram(teapotShaderState_->program);
    
    TEAPOT_MATERIALS material = {
        {1.0, 0.105, 0.095}, {1.0f, 1.0f, 1.0f, 40.f}, {0.1f, 0.1f, 0.1f}, };
    
    // Update uniforms
    glUniform4f(teapotShaderState_->material_diffuse_, material.diffuse_color[0],
                material.diffuse_color[1], material.diffuse_color[2], 1.f);
    
    glUniform4f(teapotShaderState_->material_specular_, material.specular_color[0],
                material.specular_color[1], material.specular_color[2],
                material.specular_color[3]);
    
    // using glUniform3fv here was troublesome
    
    glUniform3f(teapotShaderState_->material_ambient_, material.ambient_color[0],
                material.ambient_color[1], material.ambient_color[2]);
    
    GLfloat glmatrix[16];
    mat_vp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(teapotShaderState_->matrix_projection_, 1, GL_FALSE,
                       glmatrix);
    
    GLfloat glmatrix2[16];
    mat_mv.writeToColumnMajorMatrix(glmatrix2);
    glUniformMatrix4fv(teapotShaderState_->matrix_view_, 1, GL_FALSE, glmatrix2);
    
    GLfloat glmatrix3[16];
    mat_model_.writeToColumnMajorMatrix(glmatrix3);
    glUniformMatrix4fv(teapotShaderState_->matrix_model_, 1, GL_FALSE, glmatrix3);
    
    glUniform3f(teapotShaderState_->light0_, 100.f, 0.f, -300.f);
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_SHORT,
                   BUFFER_OFFSET(0));

    glBindVertexArray(0);
}
