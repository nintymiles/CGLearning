//--------------------------------------------------------------------------------
// FrustumModel.cpp
// Render a camera Frustum
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "FrustumModel.h"

//--------------------------------------------------------------------------------
// Teapot model data
//--------------------------------------------------------------------------------
#include "matrix4.h"
#include "Geometry.h"

#include "FrustumGeometry.h"

#include <GLFW/glfw3.h>


//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
FrustumModel::FrustumModel() {}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
FrustumModel::~FrustumModel(){
    Unload();
}

void FrustumModel::Init() {
    // Settings
    glFrontFace(GL_CCW);
    
    // populate mat_project_
    
    // Load shader
    frustumShaderState_.reset(new FrustumShaderState());
    
    //  UpdateViewport();
    //    mat_model_ = Matrix4::makeTranslation(Cvec3(0, -10.f, -80.f));
    //
    //    mat_model_ =  mat_model_ * Matrix4::makeXRotation(-60);
    //
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,.5f));
    mat_view_ = inv(mat_view_);
    
    //    UpdateViewport();
}

void FrustumModel::UpdateViewport() {
    
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
//    glViewport(static_cast<float>(viewport[2])/2,static_cast<float>(viewport[3])/2,static_cast<float>(viewport[2])/2,static_cast<float>(viewport[3])/2);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10.f;
    
    
    float fov = 15;
    float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =Matrix4::makeProjection(fov, aspect, CAM_NEAR, CAM_FAR);
    
    
    
    FrustumGeometry fg=FrustumGeometry(mat_projection_);
    vector<VertexPN> vData = makeFrustVertexPNData(fg);
    
    
    
    
    // vertex number
    num_vertices_ = (int)vData.size();
    
    geometry_.reset(new Geometry(&vData[0], num_vertices_));
    
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPN);
    // Pass the vertex data
    glVertexAttribPointer(FRUSTUM_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(FRUSTUM_ATTRIB_VERTEX);
    
    glVertexAttribPointer(FRUSTUM_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(FRUSTUM_ATTRIB_COLOR);
    
    glBindVertexArray(0);
    
    
    mat_projection_ =Matrix4::makeProjection(90, aspect, CAM_NEAR, -1000);
}

void FrustumModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
}

void FrustumModel::Update(double time) {
    
    
}

void FrustumModel::Render() {
    mat_model_ = mat_model_ * Matrix4::makeYRotation(1);
    //    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_mvp_ = mat_projection_ * mat_view_ * mat_model_;
    
    
    
    glUseProgram(frustumShaderState_->program);
    
    GLfloat glmatrix[16];
    mat_mvp_.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(frustumShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES,0,num_vertices_);
    
    glBindVertexArray(0);
}

