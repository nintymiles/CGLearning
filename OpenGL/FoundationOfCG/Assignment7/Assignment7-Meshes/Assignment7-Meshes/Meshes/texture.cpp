#include <vector>
#include <string>

#include "ppm.h"
#include "asstcommon.h"
#include "texture.h"
#include "material.h"

using namespace std;

ImageTexture::ImageTexture(const string& ppmFileName, bool srgb) {
    int width, height;
    vector<PackedPixel> pixData;
    ppmRead(ppmFileName.c_str(), width, height, pixData);
        
    glBindTexture(GL_TEXTURE_2D, tex);

    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //如果srgb==false，则（！srgb）==true == 1，此时设置internal format为GL_RGB，否则设置为GL_SRGB
    //实际使用中unsized rgb格式效果更好，why？fragment shader以linear方式使用color？
    glTexImage2D(GL_TEXTURE_2D, 0, (!srgb) ? GL_RGB : GL_SRGB, width, height,0, GL_RGB, GL_UNSIGNED_BYTE, &pixData[0]);
    
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512,
    //             0, GL_RGB, GL_UNSIGNED_BYTE, &pixData[0]);

    //不使用渐进式纹理，但是使用了对应的过滤器，导致错误。
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    checkGlError("Error Loading Texture");
}
