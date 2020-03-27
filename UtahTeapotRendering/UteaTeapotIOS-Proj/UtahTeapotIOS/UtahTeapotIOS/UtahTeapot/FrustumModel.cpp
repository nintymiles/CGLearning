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



#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>


//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
FrustumModel::FrustumModel():fg_(FrustumGeometry(Matrix4())),visible(true){}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
FrustumModel::~FrustumModel(){
    Unload();
}

void FrustumModel::Init() {
    // populate mat_project_
    vector<VertexPN> vData = makeFrustVertexPNData(fg_);
    
    // Load shader
    frustumShaderState_.reset(new FrustumShaderState());
    
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
    

    mat_model_ =  Matrix4::makeTranslation(Cvec3(0.2, -0.5f, 3.6f));
    mat_model_ =  mat_model_ * Matrix4::makeXRotation(45);
    mat_model_ =  mat_model_ * Matrix4::makeYRotation(15);
    mat_model_ =  mat_model_ * Matrix4::makeZRotation(75);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);
    
//    UpdateViewport();
}

void FrustumModel::UpdateViewport() {
    
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    glViewport(static_cast<float>(viewport[2])/2,static_cast<float>(viewport[3])/2,static_cast<float>(viewport[2])/2,static_cast<float>(viewport[3])/2);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10.f;
    
    
    float fov = 15;
    float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =Matrix4::makeProjection(fov, aspect, CAM_NEAR, CAM_FAR);
    
}

void FrustumModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
}

void FrustumModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void FrustumModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void FrustumModel::setFrustumCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->frustumCamera_ = camera;
    
    fg_.updateWithProjMat(frustumCamera_->projMat);
    vector<VertexPN> vData = makeFrustVertexPNData(fg_);
    geometry_->updateVBO(&vData[0], (int)vData.size());
    
    //Update(0);
}

void FrustumModel::Render() {
    if(!visible)
        return;
    
//    mat_model_ = mat_model_ * Matrix4::makeYRotation(1);
//    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_mvp_ = mat_projection_ * mat_view_ * mat_model_;
    
    glUseProgram(frustumShaderState_->program);
    
    checkGlError("before binding matrix mvp");
    GLfloat glmatrix[16];
    mat_mvp_.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(frustumShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    checkGlError("before binding vao_");
    glBindVertexArray(vao_);
    checkGlError("before drawing lines");
    glDrawArrays(GL_LINES,0,num_vertices_);
    
    checkGlError("after drawing lines");
    
    glBindVertexArray(0);
}

