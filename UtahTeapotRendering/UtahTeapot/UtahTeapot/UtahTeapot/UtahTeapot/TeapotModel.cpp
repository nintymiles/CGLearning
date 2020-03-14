//--------------------------------------------------------------------------------
// TeapotModel.cpp
// Render a utah teapot
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TeapotModel.h"

//--------------------------------------------------------------------------------
// Teapot model data
//--------------------------------------------------------------------------------
#include "teapot.inl"
#include "matrix4.h"
#include "Geometry.h"

#include <GLFW/glfw3.h>


//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
TeapotModel::TeapotModel() {}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
TeapotModel::~TeapotModel(){
    Unload();
}

void TeapotModel::Init() {
    // Settings
    glFrontFace(GL_CCW);
    
    // Load shader
    teapotShaderState_.reset(new TeapotShaderState());
    
    // Create Index buffer
    num_indices_ = sizeof(teapotIndices) / sizeof(teapotIndices[0]);
    // Create VBO
    num_vertices_ = sizeof(teapotPositions) / sizeof(teapotPositions[0]) / 3;
    
    int32_t stride = sizeof(VertexPNX);
    int32_t index = 0;
    int32_t tIndex = 0;
    VertexPNX* p = new VertexPNX[num_vertices_];
    for (int32_t i = 0; i < num_vertices_; ++i) {
        p[i].p[0] = teapotPositions[index];
        p[i].p[1] = teapotPositions[index + 1];
        p[i].p[2] = teapotPositions[index + 2];
        
        p[i].n[0] = teapotNormals[index];
        p[i].n[1] = teapotNormals[index + 1];
        p[i].n[2] = teapotNormals[index + 2];
        index += 3;
        
        p[i].x[0] = teapotTexCoords[tIndex];
        p[i].x[1] = teapotTexCoords[tIndex + 1];
        
        tIndex += 2;
    }
    
    geometry_.reset(new Geometry(p, teapotIndices, num_vertices_, num_indices_));
    
    delete[] p;
    
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
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
    
    // Bind the IB
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
    glBindVertexArray(0);
    
    //  UpdateViewport();
    mat_model_ = Matrix4::makeTranslation(Cvec3(0, -10.f, -80.f));
    
    mat_model_ =  mat_model_ * Matrix4::makeXRotation(-60);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);
    
    UpdateViewport();
}

void TeapotModel::UpdateViewport() {
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10000.f;
    
    
    float fov = 60;
    float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =Matrix4::makeProjection(fov, aspect, CAM_NEAR, CAM_FAR);
}

void TeapotModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
    //  if (shader_param_.program_) {
    //    glDeleteProgram(shader_param_.program_);
    //    shader_param_.program_ = 0;
    //  }
}

void TeapotModel::Update(double time) {
    
    
}

void TeapotModel::Render(float r, float g, float b) {
    
    mat_model_ = mat_model_ * Matrix4::makeZRotation(1);
    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    
    
    glUseProgram(teapotShaderState_->program);
    
    TEAPOT_MATERIALS material = {
        {r, g, b}, {1.0f, 1.0f, 1.0f, 90.f}, {0.1f, 0.1f, 0.1f}, };
    
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
    mat_view_.writeToColumnMajorMatrix(glmatrix2);
    glUniformMatrix4fv(teapotShaderState_->matrix_view_, 1, GL_FALSE, glmatrix2);
    
    glUniform3f(teapotShaderState_->light0_, -100.f, -200.f, 100.f);
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_SHORT,
                   BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

