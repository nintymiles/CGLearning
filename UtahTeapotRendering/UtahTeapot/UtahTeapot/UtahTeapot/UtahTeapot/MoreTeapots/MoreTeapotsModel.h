#ifndef MORETEAPOTSMODEL_H
#define MORETEAPOTSMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "cvec.h"
#include "shadersasst.h"

#include "Geometry.h"


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct TEAPOT_VERTEX {
    float pos[3];
    float normal[3];
};

enum SHADER_ATTRIBUTES {
    ATTRIB_VERTEX,
    ATTRIB_NORMAL,
    ATTRIB_UV,
};

struct TEAPOT_MATERIALS {
    //float diffuse_color[3];
    float specular_color[4];
    float ambient_color[3];
};

class MoreTeapotsModel{
    int32_t num_indices_;
    int32_t num_vertices_;
    //  GLuint ibo_;
    //  GLuint vbo_;
    GLuint anglecounter;
    GLuint ubo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<MoreTeapotsShaderState> moreTeapotsShaderState_;
    
    Matrix4 mat_projection_;
    Matrix4 mat_view_;
    Matrix4 mat_model_;
    
    std::vector<Matrix4> vec_mat_models_;
    std::vector<Cvec3> vec_colors_;
    std::vector<Cvec2> vec_rotations_;
    std::vector<Cvec2> vec_current_rotations_;
    
    int32_t teapot_x_;
    int32_t teapot_y_;
    int32_t teapot_z_;
    int32_t ubo_matrix_stride_;
    int32_t ubo_vector_stride_;
    
    std::string ToString(const int32_t i);
    
public:
    MoreTeapotsModel();
    virtual ~MoreTeapotsModel();
    void Init(const int32_t numX, const int32_t numY,
              const int32_t numZ);
    void Render(float r, float g, float b);
    void Update(double time);
    void Unload();
    void UpdateViewport();
    
};

#endif
