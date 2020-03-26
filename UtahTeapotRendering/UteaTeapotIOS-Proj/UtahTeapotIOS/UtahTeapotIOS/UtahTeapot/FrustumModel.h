#ifndef FRUSTUMMODEL_H
#define FRUSTUMMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "shadersasst.h"

#include "Geometry.h"


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

enum FRUSTUM_SHADER_ATTRIBUTES {
    FRUSTUM_ATTRIB_VERTEX,
    FRUSTUM_ATTRIB_COLOR,
};

class FrustumModel{
    int32_t num_vertices_;
    GLuint vbo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<FrustumShaderState> frustumShaderState_;
    
    Matrix4 mat_projection_;
    Matrix4 mat_view_;
    Matrix4 mat_model_;
    
public:
    FrustumModel();
    virtual ~FrustumModel();
    void Init();
    void Render();
    void Update(double time);
    void Unload();
    void UpdateViewport();
    
};

#endif
