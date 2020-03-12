//
//  Texture.hpp
//  TexturedUtahTeapotIOS
//
//  Created by SeanRen on 2020/3/11.
//  Copyright © 2020 SeanRen. All rights reserved.
//

#ifndef Texture_H
#define Texture_H

#include <string>
#include <vector>

#include <GLFW/glfw3.h>

/**
 *  class Texture
 *    adding texture into teapot
 *     - oad image in assets/Textures
 *     - enable texture units
 *     - report samplers needed inside shader
 *  Functionality wise:
 *     - one texture
 *     - one sampler
 *     - texture unit 0, sampler unit 0
 */
class Texture {
protected:
    Texture();
    virtual ~Texture();
    
public:
    /**
     *   Create a texture object
     * @param type  should be GL_TEXTURE_2D / GL_TEXTURE_CUBE_MAP
     * @param texFiles holds image file names under APK/assets.
     *     2d texture uses the very first image texFiles[0]
     *     cube map needs 6 (direction of +x, -x, +y, -y, +z, -z)
     * @param assetManager Java side assetManager object
     * @return newly created texture object, or nullptr in case of errors
     */
    static Texture* Create( GLuint type, std::vector<std::string>& texFiles);
    static void Delete(Texture *obj);
    
    virtual bool GetActiveSamplerInfo(std::vector<std::string> &names,
                                      std::vector<GLint> &units) = 0;
    virtual bool Activate(void) = 0;
    virtual GLuint GetTexType() = 0;
    virtual GLuint GetTexId() = 0;
    
};



#endif /* Texture_hpp */
