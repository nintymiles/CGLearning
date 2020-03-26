//--------------------------------------------------------------------------------
// MoreTeapotsModel.cpp
// Render a utah teapot
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "MoreTeapotsModel.h"

//--------------------------------------------------------------------------------
// Teapot model data
//--------------------------------------------------------------------------------
#include "../teapot.inl"
#include "matrix4.h"
#include "Geometry.h"

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

#include <map>


//--------------------------------------------------------------------------------
// Ctor
//--------------------------------------------------------------------------------
MoreTeapotsModel::MoreTeapotsModel(){}

//--------------------------------------------------------------------------------
// Dtor
//--------------------------------------------------------------------------------
MoreTeapotsModel::~MoreTeapotsModel(){
    Unload();
}

void MoreTeapotsModel::Init(const int32_t numX, const int32_t numY,
                            const int32_t numZ) {
    // Settings
    //glFrontFace(GL_CCW);
    
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
    
    // Init Projection matrices
    teapot_x_ = numX;
    teapot_y_ = numY;
    teapot_z_ = numZ;
    vec_mat_models_.reserve(teapot_x_ * teapot_y_ * teapot_z_);
    
    //the specific between teapots
    const float total_width = 800.f;
    float gap_x = total_width / (teapot_x_ - 1);
    float gap_y = total_width / (teapot_y_ - 1);
    float gap_z = total_width / (teapot_z_ - 1);
    float offset_x = -total_width / 2.f;
    float offset_y = -total_width / 2.f;
    float offset_z = -total_width / 2.f;
    //the creation of transform data for instanced objects
    for (int32_t x = 0; x < teapot_x_; ++x)
        for (int32_t y = 0; y < teapot_y_; ++y)
            for (int32_t z = 0; z < teapot_z_; ++z) {
                vec_mat_models_.push_back(Matrix4::makeTranslation(Cvec3(x * gap_x + offset_x, y * gap_y + offset_y,z * gap_z + offset_z)));
                vec_colors_.push_back(Cvec3(random() / float(RAND_MAX * 1.1), random() / float(RAND_MAX * 1.1),random() / float(RAND_MAX * 1.1)));
                
                float rotation_x = random() / float(RAND_MAX) - 0.5f;
                float rotation_y = random() / float(RAND_MAX) - 0.5f;
                vec_rotations_.push_back(Cvec2(rotation_x * 0.05f, rotation_y * 0.05f));
                vec_current_rotations_.push_back(
                                                 Cvec2(rotation_x * M_PI, rotation_y * M_PI));
            }
    
    //create and upload ubo
    // Create parameter dictionary for shader patch
    std::map<std::string, std::string> param;
    //wildcard sign?
    param[std::string("%NUM_TEAPOT%")] =
    ToString(teapot_x_ * teapot_y_ * teapot_z_);
    param[std::string("%LOCATION_VERTEX%")] = ToString(ATTRIB_VERTEX);
    param[std::string("%LOCATION_NORMAL%")] = ToString(ATTRIB_NORMAL);
    
    // Load shader
    moreTeapotsShaderState_.reset(new MoreTeapotsShaderState(param));
    
    //
    // Create uniform buffer
    //
    GLuint bindingPoint = 1;
    GLuint blockIndex;
    GLuint program = moreTeapotsShaderState_->program;
    blockIndex = glGetUniformBlockIndex(program, "ParamBlock");
    glUniformBlockBinding(program, blockIndex, bindingPoint);
    
    // Retrieve array stride value
    int32_t num_indices;
    glGetActiveUniformBlockiv(program, blockIndex,
                              GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &num_indices);
    GLint i[num_indices];
    GLint strides[num_indices];
    glGetActiveUniformBlockiv(program, blockIndex,
                              GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, i);
    glGetActiveUniformsiv(program, num_indices, (GLuint*)i,
                          GL_UNIFORM_ARRAY_STRIDE, strides);
    
    ubo_matrix_stride_ = strides[0] / sizeof(float);
    ubo_vector_stride_ = strides[2] / sizeof(float);
    
    glGenBuffers(1, &ubo_);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo_);
    
    // Store color value which wouldn't be updated every frame
    int32_t size = teapot_x_ * teapot_y_ * teapot_z_ *
    (ubo_matrix_stride_ + ubo_matrix_stride_ +
     ubo_vector_stride_);  // Mat4 + Mat4 + Vec3 + 1 stride
    float* pBuffer = new float[size];
    float* pColor =
    pBuffer + teapot_x_ * teapot_y_ * teapot_z_ * ubo_matrix_stride_ * 2;
    for (int32_t i = 0; i < teapot_x_ * teapot_y_ * teapot_z_; ++i) {
        memcpy(pColor, &vec_colors_[i], 3 * sizeof(float));
        pColor += ubo_vector_stride_;  // Assuming std140 layout which is 4
                                       // DWORD stride for vectors
    }
    
    glBufferData(GL_UNIFORM_BUFFER, size * sizeof(float), pBuffer,
                 GL_DYNAMIC_DRAW);
    delete[] pBuffer;
    
    if(vao_==0)
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
    
    // Bind the IB
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
    
    glBindVertexArray(0);
    
    //  UpdateViewport();
    //    mat_model_ = Matrix4::makeTranslation(Cvec3(0, 0, -80.f));
    //
    //    mat_model_ =  mat_model_ * Matrix4::makeXRotation(-60);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,1200.0f));
    mat_view_ = inv(mat_view_);
    
    UpdateViewport();
}

void MoreTeapotsModel::UpdateViewport() {
    // Init Projection matrices
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10000.f;
    
    float aspect =
                static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ = Matrix4::makeProjection(60, aspect, CAM_NEAR, CAM_FAR);

}

void MoreTeapotsModel::Unload() {

}

void MoreTeapotsModel::Update(double time) {
    
    
}

void MoreTeapotsModel::Render(float r, float g, float b) {
    anglecounter++;
    if(anglecounter>360)
        anglecounter=0;
    
    
    glUseProgram(moreTeapotsShaderState_->program);
    
    TEAPOT_MATERIALS material = {{1.0f, 1.0f, 1.0f, 10.f}, {0.1f, 0.1f, 0.1f}, };
    
    // Update uniforms
    //
    // using glUniform3fv here was troublesome..
    //
    glUniform4f(moreTeapotsShaderState_->material_specular_, material.specular_color[0],
                material.specular_color[1], material.specular_color[2],
                material.specular_color[3]);
    glUniform3f(moreTeapotsShaderState_->material_ambient_, material.ambient_color[0],
                material.ambient_color[1], material.ambient_color[2]);
    
    glUniform3f(moreTeapotsShaderState_->light0_, -90.f, 90.f, -300.f);
    
    //
    // Geometry instancing, new feature in GLES3.0
    //
    
    // Update UBO
    glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
    float* p = (float*)glMapBufferRange(
                                        GL_UNIFORM_BUFFER, 0, teapot_x_ * teapot_y_ * teapot_z_ *
                                        (ubo_matrix_stride_ * 2) * sizeof(float),
                                        GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    float* mat_mvp = p;
    float* mat_mv = p + teapot_x_ * teapot_y_ * teapot_z_ * ubo_matrix_stride_;
    for (int32_t i = 0; i < teapot_x_ * teapot_y_ * teapot_z_; ++i) {
        // Rotation
        float x, y;
        vec_current_rotations_[i] += vec_rotations_[i];
        x = vec_current_rotations_[i][0];
        y = vec_current_rotations_[i][1];
        Matrix4 mat_rotation =
        Matrix4::makeXRotation(x) * Matrix4::makeXRotation(y);
        
        // Feed Projection and Model View matrices to the shaders
        Matrix4 mat_v = mat_view_ * vec_mat_models_[i] * mat_rotation * Matrix4::makeZRotation(anglecounter+i);
        Matrix4 mat_vp = mat_projection_ * mat_v;
        
        float glMarixVP[16];
        mat_vp.writeToColumnMajorMatrix(glMarixVP);
        memcpy(mat_mvp, &glMarixVP, sizeof(glMarixVP));
        mat_mvp += ubo_matrix_stride_;
        
        float glMarixV[16];
        mat_v.writeToColumnMajorMatrix(glMarixV);
        memcpy(mat_mv, &glMarixV, sizeof(glMarixV));
        mat_mv += ubo_matrix_stride_;
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindVertexArray(vao_);
    // Instanced rendering
    glDrawElementsInstanced(GL_TRIANGLES, num_indices_, GL_UNSIGNED_SHORT,
                            BUFFER_OFFSET(0),
                            teapot_x_ * teapot_y_ * teapot_z_);
    
    glBindVertexArray(0);
}

//--------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------
std::string MoreTeapotsModel::ToString(const int32_t i) {
    char str[64];
    snprintf(str, sizeof(str), "%d", i);
    return std::string(str);
}
