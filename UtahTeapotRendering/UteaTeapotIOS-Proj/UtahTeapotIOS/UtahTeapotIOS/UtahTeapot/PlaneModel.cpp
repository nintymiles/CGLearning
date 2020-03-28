//--------------------------------------------------------------------------------
// PlaneModel.cpp
// Render a plane
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "PlaneModel.h"

#include "matrix4.h"
#include "Geometry.h"

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>


//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
PlaneModel::PlaneModel() {
    vao_ = 0;
    vbo_ = 0;
    ibo_ = 0;
}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
PlaneModel::~PlaneModel(){
    Unload();
}

void PlaneModel::Init() {
    // Settings
    glFrontFace(GL_CCW);
    
    // Load shader
    textureShaderState_.reset(new SimpleTextureShaderState());
    
    getPlaneVbIbLen(num_vertices_, num_indices_);
    
    // Temporary storage for cube Geometry
    vector<VertexPNX> vtx(num_vertices_);
    vector<unsigned short> idx(num_indices_);
    
    //生成的平面位于y=0处;
    makePlane(160*2, vtx.begin(), idx.begin());
    geometry_.reset(new Geometry(&vtx[0], &idx[0], num_vertices_, num_indices_));
        
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(TEXTURE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_VERTEX);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_NORMAL);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_UV);
    
    // Bind the IB
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
    glBindVertexArray(0);
    
    mat_model_ = Matrix4::makeTranslation(Cvec3(0.f, 0.f, -100.f));
    
    mat_model_ =  mat_model_ * Matrix4::makeXRotation(90);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);
    
    
    vector<std::string> textures = {std::string(GetBundleFileName("greenleaves.png"))};
    
    
    texObj_ = Texture::Create(GL_TEXTURE_2D, textures);
    assert(texObj_);
    
    std::vector<std::string> samplers;
    std::vector<GLint> units;
    texObj_->GetActiveSamplerInfo(samplers, units);
    for(size_t idx = 0; idx < samplers.size(); idx++) {
        GLint sampler = glGetUniformLocation(textureShaderState_->program,
                                             samplers[idx].c_str());
        glUniform1i(sampler, units[idx]);
    }
    
    texObj_->Activate();
}

void PlaneModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
}

void PlaneModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void PlaneModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void PlaneModel::Render(){
    
    //mat_model_ = mat_model_ * Matrix4::makeZRotation(1);
    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    glUseProgram(textureShaderState_->program);
    
    
    GLfloat glmatrix[16];
    mat_vp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(textureShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, num_indices_, GL_UNSIGNED_SHORT,
                   BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

