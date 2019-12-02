# 纹理映射（Texture Mapping）
In Chapter 14, we saw how a simple model of interaction between light and materials can be used to compute a pixel’s color. The second main tool typically used by a fragment shader to compute a pixel’s color is to fetch data from an auxiliary image called a texture. In this chapter we will explore basic texture mapping as well as a number of variants. Such techniques are one of the main tools used to make our renderings look detailed and rich.
在第14章中，我们看到了在光和材料之间的一个简单交互模型是如何被用来计算一个像素的色彩。通常被碎片着色器（fragment shader）用来计算一个像素色彩的另一种重要工具是从一个被称作纹理的辅助图像中提取数据。在本章中我们会探索基本纹理映射（basic texture mapping），同时还会包含它的多个变体。这样的技术正是让我们的渲染显得细致和内容丰富的主要工具。

Figure 15.1 shows a simple example of texture mapping. On the left, we see a simple model made up of a few dozen triangles. One triangle is highlighted in red. We associate x t , y t texture coordinates with each vertex of each triangle (top). These coordinates “map” each vertex to some location in the texture. The texture coordinates are then interpolated over the triangle (bottom), giving us texture coordinates for each pixel. The fragment shader then grabs the texture color that is pointed to by its texture coordinates, and sends it to the framebuffer. The resulting rendered image is shown on the right.
图示$\text{Figure 15.1}$展示了一个纹理映射的简单例子。在其左侧，我们看到了一个用几十个三角形构成的简单模型。其中一个三角形被高亮为红色。我们给每个三角形的每个顶点关联一个纹理坐标$x_t,y_t$（参考图示上部）。这些坐标“映射”每个顶点纹理中的某个位置。纹理坐标随后在三角形范围被插值（参考图示底部），如此就针对每个像素给出了纹理坐标。碎片着色器（fragment shader）随后抓取被纹理坐标所指向的纹理色彩，并且将其发送到framebuffer。最终被渲染的图像展示在图示的右侧。

## 15.1 基础纹理映射 （Basic Texturing）
In basic texturing, we simply “glue” part of an image onto a triangle by specifying texture coordinates at the three vertices. In this model, we assume that each of the variables x t and y t are functions over a triangle that are afﬁne in object coordinates. This allows us to translate and rotate (or even scale and shear) the texture image in this gluing process.
基础纹理映射中，我们只是“粘贴”一个图像的局部到一个三角形上，这个动作通过在三角形的三个顶点上指定纹理坐标的方式完成。在这个模型中，我们假设每个变量$x_t$和$y_t$都是一个三角形上的函数，而这些函数在对象坐标上具有并行的特点。这种情形允许我们在粘合过程中平移、选择（甚至缩放和切削）纹理图片。

The code for loading a texture is described in Section A.4. As described there, a uniform variable is used to point to the desired texture unit, which is the same for all pixels processed in this draw-call. Varying variables are used to store texture coordinates at vertices, which point into some speciﬁc 2D location in the texture.
用于加载一个纹理的代码在附录小节A.4中已经被描述。正如在那里所讲的，一个uniform变量被用于指向想要的纹理单位，这个纹理单位对于在这个绘制调用中的所有像素都是相同的。变异变量（varying variables）被用于在顶点上存储纹理坐标，这些纹理坐标指向纹理中某个特定2D地址。

Here, we describe the relevant shaders needed for basic texture mapping. In this basic texturing example, the vertex shader simply passes the texture coordinates out to the fragment shader as varying variables.
下面，我们描述相关的用于基础纹理映射所需着色器（shaders）。在这个基础的纹理映射例子里，顶点着色器（vertex shader）只是将纹理坐标作为变异变量传递到碎片着色器（fragment shader）中。

```
#version 330
uniform mat4 uProjMatrix; 
uniform mat4 uModelViewMatrix;

in vec2 aTexCoord; 
in vec4 aVertex;

out vec2

vTexCoord;
void main(){ 
	vTexCoord = aTexCoord; 
	gl_Position = uProjMatrix * uModelViewMatrix * aVertex; 
}
```

The following fragment shader then uses these interpolated texture coordinates to lookup the desired color data from the texture and set the color in the framebuffer.
下面的碎片着色器（fragment shader）随后使用这些被插值的纹理坐标来从纹理上寻找想要的色彩数据，并且将其设置到帧缓存（framebuffer）中。

```
#version 330
uniform sampler2D uTexUnit0;

in vec2 vTexCoord; 

out fragColor

void main(){ 
	vec4 texColor0 = texture2D(uTexUnit0, vTexCoord); 
	fragColor = texColor0; 
}
```

The data type sampler2D is a special GLSL data type that refers to an OpenGL texture unit. The call to texture2D fetches a value from the texture unit.
数据类型sampler2D是一种特殊的GLSL数据类型，用于指向一个OpenGL纹理单位（texture unit）。对texture2D的调用会获取来自纹理单位的一个色彩值。

注：此处实际上是texture()函数调用。

In this simplest incarnation, we just fetch r,g,b values from the texture and send them directly to the framebuffer. Alternatively, the texture data could be interpreted as, say, the diffuse material color of the surface point, which would then be followed by the diffuse material computation described in Section 14.2.
在这种最简单的理想化情形中，我们只是从纹理中获取r、g、b的数值并且直接发送它们到帧缓存中。可选地，纹理数据可以被解释为，比方说，不同的表面点的材料色，这然后会跟着被在14.2节中所描述的漫射表面计算所使用。

![Figure15.1](media/Figure15.1.png)
**Figure 15.1:** Top: each vertex of a triangle is given x, y texture coordinates. These point into the texture image. Bottom: these coordinates are interpolated as varying variables at the pixel resolution. In the fragment shader, we can grab the color pointed  to by the texture coordinates, and use it in our rendering (right). From [65],. 顶部：一个三角形的每个顶点都被给出x，y纹理坐标。这些坐标指向纹理图像中。底部：这些坐标被在像素解析时插值为变异变量（varying variables）。在帧缓存（framebuffer）中，我们通过这些纹理坐标抓取其所指向的色彩，同时在渲染中使用这个色彩（右侧）。参考[65],ACM。

## 15.2 法线映射（Normal Mapping）
The data from a texture can also be interpreted in more interesting ways. In normal mapping, the r,g,b values from a texture are interpreted as the three coordinates of the normal at the point. This normal data can then be used as part of some material simulation , as described in Chapter 14. See Figure 15.2 for such an example.
来自一个纹理的发现也可以更有趣的方式被解读。在法线映射，来自一个纹理的r、g、b数值被解读为当前点法线的3个坐标。这种法线数据随后会被用作某种材料模拟计算的一部分，就如在14章所描述的。参看这种情形的实例图示$\text{Figure 15.2}$

Normal data has three coordinate values, each in the range [−1..1], while RGB textures store three values, each in the range [0..1]. Thus the normal data needs to be transformed into this format before being stored in the texture, as in . Conversely, your fragment shader needs to undo this transformation, as in normal x = 2r-1;.
法线数据有3中坐标值，每个都位于范围$[-1..1]$,而RGB纹理存储的3个数值，每个都处于范围$[0..1]$。因此，法线数据在被存储为纹理之前需要被转换为这种格式，就如在代码`r = normal_x/2. +.5;`中的转换。反之，你的碎片着色器（fragment shader）需要恢复这种转换，就如在代码`normal_x = 2r-1;`中的转换。

![Figure15.2](media/Figure15.2.png)
**Figure 15.2:** On the left we show a rendering of a teapot model. The teapot is shaded c with smoothly interpolated normals and the triangle edges are shown. (⃝Hugues Hoppe). On the right, we show the same surface but now it is rendered using a normal ﬁeld fetched from a high resolution texture. These normals are used in the lighting calc culations, and give the effect of highly resolved geometric detail. From [54], ⃝ACM. 在左侧，我们展示了一个茶壶模型的渲染。茶壶使用平滑插值的法线被着色，同时三角形边缘被展示。(©️Hugues Hoppe)。在右侧，我们展示了相同的表面，但是现在茶壶却是被来自高解析度纹理中所提取的法线域所渲染。这些法线被使用在光照计算中。并且给出了高解析度几何细节效果。参考[54],©️ACM。

## 15.3 Environment Cube Maps

Textures can also be used to model the environment in the distance around the object being rendered. In this case, we typically use 6 square textures representing the faces of a large cube surrounding the scene. Each texture pixel represents the color as seen along one direction in the environment. This is called a cube map. GLSL provides a cube-texture data type, samplerCube speciﬁcally for this purpose. During the shading of a point, we can treat the material at that point as a perfect mirror and fetch the environment data from the appropriate incoming direction. An example is shown in Figure 15.3.

To implement this idea, we use Equation (14.1) to calculate the bounced view vector, B(⃗v ). This bounced vector will point points towards the environment direction which would be observed in a mirrored surface. By looking up the cube map using this direction, we give the surface the appearance of a mirror.


```#version 330 uniform samplerCube texUnit0;

in vec3 vNormal; in vec4 vPosition;

out fragColor;
vec3 reflect(vec3 w, vec3 n){ return - w + n * (dot(w, n) * 2.0); }

void main(void){

vec3 normal = normalize(vNormal); vec3 reflected = reflect(normalize(vec3(-vPosition)), normal); vec4 texColor0 = textureCube(texUnit0,reflected);

fragColor = vec4(texColor0.r, texColor0.g, texColor0.b, 1.0);

}
```

In eye coordinates, the eye’s position coincides with the origin, and thus -vPosition represents the view vector ⃗v . textureCube is a special GLSL function that takes a direction vector and returns the color stored at this direction in the cube texture map.

In this code, all of our vectors are expressed in eye coordinates, and so we are also assuming that our cube texture represents the environment data in eye coordinates. If our cube texture were representing directions using, say, world coordinates, then appropriate coordinates for the rendered point would need to be passed to the fragment shader.

This same idea, but modeling refraction instead of mirror reﬂection, has been used to generate the fountain image of Figure 22.4.

![Figure15.3](media/Figure15.3.png)
**Figure 15.3:** On the left the environment is stored as a cube texture. This is used to c render a mirrored lizard. From [25], ⃝IEEE. 在左侧，环境被存储为一个立方体纹理（cube texture）。其被用来渲染一个镜面化的蜥蜴。参考[25],©️IEEE。

## 15.4 投影仪纹理映射（Projector Texture Mapping）
There are times when we wish to glue our texture onto our triangles using a projector model, instead of the afﬁne gluing model assumed in Section 15.1. For example, we may wish to simulate a slide projector illuminating some triangles in space. (See Figure 15.4). This is not as unusual as it may ﬁrst appear. For example, suppose we have taken a photograph of the facade of a building with a camera, and then wish to paste it appropriately on a digital 3D model of the building. To do this pasting, we should invert the geometry of the photography process by replacing the camera with a virtual slide projector in the same position relative to the building. (See Figure 15.5).
要完成这种粘合，我们应反转相机成像处理的几何过程-通过在相对于建筑的相同位置将相机替换为虚拟滑动投影仪的方式。

In projector texture mapping, the slide projector is modeled using 4 by 4, modelview and projection matrices, M s and P s . These deﬁne the relation 
$$\large{ 
	\begin{bmatrix} x_tw_t \\ y_tw_t \\ z_tw_t \\ w_t \end{bmatrix} 
	=
	P_sM_s \begin{bmatrix} x_o \\ y_o \\ z_o \\ 1 \end{bmatrix} 
	\qquad (15.1) 
}$$
with the texture coordinates deﬁned as x t = x w t w t t and y t = y w t w t t . To color a point on a triangle with object coordinates [x o , y o , z o , 1] t , we fetch the texture data stored at location [x t , y t ] t . See Figure 15.6.

Due to the division by w t , the values x t and y t are not afﬁne functions of (x o , y o , z o ), and thus would not be appropriately interpolated if directly implemented using varying variables. But, from Section B.5, we see that, indeed, the three quantities x t w t , y t w t and w t are all afﬁne functions of (x o , y o , z o ). Thus these quantities will be properly interpolated over a triangle when implemented as varying variables. In the fragment shader, we need to divide by w t to obtain the actual texture coordinates.

When doing projector texture mapping, we do not need to pass any texture coordinates as attribute variables to our vertex shader. We simply use the object coordinates already available to us. We do need to pass in, using uniform variables, the necessary projector matrices. Here is the relevant part of our vertex shader.

```
#version 330 
uniform mat4 uModelViewMatrix; 
uniform mat4 uProjMatrix;
uniform mat4 uSProjMatrix; 
uniform mat4 uSModelViewMatrix;

in vec4 aVertex; out vec4 avTexCoord;

void main(){ 
	vTexCoord= uSProjMatrix * uSModelViewMatrix * aVertex; 
	gl_Position = uProjMatrix * uModelViewMatrix * aVertex; 
}
```

And our fragment shader is

```
#version 330
uniform sampler2D vTexUnit0;

in vec4 vTexCoord; 
out fragColor

void main(void){ 
	vec2 tex2; 
	tex2.x = vTexCoord.x/vTexCoord.w; 
	tex2.y = vTexCoord.y/vTexCoord.w; 
	vec4 texColor0 = texture2D(vTexUnit0, tex2); 
	fragColor= texColor0; 
}
```

Full disclosure: to produce the image of Figure 15.4, in our vertex shader we also computed the normal at each vertex in the “eye” coordinates of the projector. A diffuse lighting equation was then added to our fragment shader to modulate the texture color. As a result, the top face in the image is a bit dimmer than the rightmost face.

Conveniently, OpenGL even gives us a special call texture2DProj(vTexUnit0, pTexCoord), that actually does the divide for us. Inconveniently, when designing our slide projector matrix uSProjMatrix, we have to deal with the fact that (as described in Section 12.3.1), the canonical texture image domain in OpenGL is the unit square whose lower left and upper right corners have coordinates [0, 0] t and [1, 1] t respectively, instead of the canonical square from [−1, −1] t to [1, 1] t used for the display window. Thus the appropriate projection matrix passed to the vertex shader would be of the form makeTranslation(Cvec3(0.5,0.5,0.5)) * makeScale(Cvec3(0.5,0.5,1)) * makeProjection(...).

![Figure15.4](media/Figure15.4.png)
**Figure 15.4:** The checkerboard image is slide projected on to the front and top faces of the cube. 棋盘图像被滑动投射到立方体的前脸和顶部面

![Figure15.5](media/Figure15.5.png)
**Figure 15.5:** Left: Actual photograph. Middle: Rendered geometric model. Right: c Rendered with projector texture mapping. From [15], ⃝ACM. 左侧：实际相片。中间：渲染的几何模型。右侧：用投影仪纹理映射渲染的图像。来自参考书目[15],©️ACM.

![Figure15.6](media/Figure15.6.png)
**Figure 15.6:** In projector texture mapping, to determine the color of a point observed in the eye’s camera, we map this to the a point in the “projectors image”. The color from this texture is used to color the point.

## 15.5 多通道（Multipass）
More interesting rendering effects can be obtained using multiple rendering passes over the geometry in the scene. In this approach, the results of all but the ﬁnal pass are stored ofﬂine and not drawn to the screen. To do this, the data is rendered into something called a frameBufferObject, or FBO. After rendering, the FBO data is then loaded as a texture, and thus can be used as input data in the next rendering pass. For coding details on FBOs you might start with [53].

### 15.5.1 反射映射（Reﬂection Mapping）
Reﬂection mapping is a simple example of multipass rendering. In this case, we want one of the rendered objects to be mirrored and reﬂect the rest of the scene. To accomplish this, we ﬁrst render the rest of the scene as seen from, say, the center of the mirrored object. Since the scene is 360 degrees, we need to render six images from the chosen viewpoint, looking right, left, back, forth, up and down. Each of these six images has a 90 degree vertical and horizontal ﬁeld of view.

This data is then transferred over to a cube map. The mirrored object can now be rendered using the cube map shaders from Section 15.3. See Figures 15.7 and 15.8 for an example. This calculation is not completely correct, since the environment map stores the view of the scene as seen from one point only. Meanwhile, true reﬂection needs to look at the appropriate bounced ray from each point of the object (see Figure 15.9). Nonetheless, for casual use, reﬂection mapping can be quite effective.

![Figure15.7](media/Figure15.7.png)
Figure 15.7: In reﬂection mapping, the scene (ﬂoor and cubes) is ﬁrst rendered into a cube map. This is used as environment map to render the mirrored sphere.

![FIgure15.8](media/FIgure15.8.png)
Figure 15.8: The blue cube is moved. The cube map is recreated and the image redrawn.

![Figure15.9](media/Figure15.9.png)
Figure 15.9: To correctly render a mirrored sphere, we need to follow a set of bounce rays that do not all intersect in a single point.

### 15.5.2 阴影映射（Shadow Mapping）
In our simple material calculations of Chapter 14, the color computation at a point does not depend on any of the rest of the geometry in the scene. Of course, in the real world, if some occluding object is situated between a surface point and the light, the point will be in shadow and thus be darker. This effect can be simulated using a multipass technique called shadow mapping. The idea is to ﬁrst create and store a z-buffered image from the point of view of the light, and then compare what we see in our view to what the light saw in its view. See Figure 15.10. If a point observed by the eye is not observed by the light, then there must be some occluding object in between, and we should draw that point as if it were in shadow.

In a ﬁrst pass, we render into an FBO the scene as observed from some camera whose origin coincides with the position of the point light source. Let us model this camera transform as:

⎡ ⎤ ⎡ ⎤ x t w t x o ⎢ y t w t ⎥ ⎢ y o ⎥ ⎢ ⎥ ⎢ ⎥ ⎣ z t w t ⎦ = P s M s ⎣ z o ⎦ w t 1

for appropriate matrices, P s and M s . During this ﬁrst pass, we render the scene to an FBO using M s as the modelview matrix and P s as the projection matrix. In the FBO, we store, not the color of the point, but rather its z t value. Due to z-buffering, the data stored at a pixel in the FBO represents the z t value of the geometry closest to the light along the relevant line of sight. This FBO is then transferred to a texture.

During the second rendering pass, we render our desired image from the eye’s point of view, but for each pixel, we check and see if the point we are observing was also observed by the light, or if it was blocked by something closer in the light’s view. To do this, we use the same computation that was done with projector texture mapping in Section 15.4. Doing so, in the fragment shader, we can obtain the varying variables x t y t and z t associated with the point [x o , y o , z o , 1] t . We then compare this z t value with the z t value stored at [x t , y t ] t in the texture. If these values agree (to within a small precision tolerance), then we are looking at a point that was also seen by the light; such a point is not in shadow and should be shaded accordingly. Conversely, if these values disagree, then the point we are looking at was occluded in the light’s image, is in shadow, and should be shaded as such. See Figure 15.11 for an example.
![Figure15.10](media/Figure15.10.png)
Figure 15.10: In shadow mapping, we ﬁrst render an (FBO) image from the point of view of the light. Only depth values are stored in this image. In a second pass, we render the scene from the point of view of our eye. Using the same mathematics from projector texturing, for every point observed by the eye, we can compare its light-depth to that stored in the light’s image. If these agree (orange and green points), the point was seen by the light, and we color it appropriately. If these disagree (purple), then the point was occluded from the eye and is in shadow, and we color it differently.

![Figure15.11](media/Figure15.11.png)
Figure 15.11: In shadow mapping, the scene is ﬁrst rendered from the point of view of the light (visualized on the left). The depth values are stored in a texture. In the second pass the light texture data is used to determine if the surface point is directly observed by the light. This gives us the ﬁnal rendered image on the right. ⃝Hamilton c Chong.


