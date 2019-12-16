# 从顶点到像素的阶段（From Vertex to Pixel）
一旦用户指定了一个三角形的三个顶点，同时这些顶点已经经历了顶点着色器（vertex shader）阶段，这些数据现在会通过一系列固定组合（a fix set）的由OpenGL完成的处理步骤。这种处理决定了三角形应该被放置到屏幕上何处，那些像素应该位于三角形内，还有变异变量（varying variable）数据的恰当地被插值的数值是什么。这种数据随后被传递到碎片着色器（fragment shader）中作更多的处理以决定最终的色彩。本章中，我们探索三角形从顶点到像素的一些变换细节。我们不会详细叙述这些细节，但是尝试指出最有趣的部分。在图示$\text{Figure 12.1}$中，我们展示了在本章中讲述的这些步骤如何适应渲染管线（rendering pipeline）。在本章和下一章中，我们将依赖一些并行函数（affine functions）的基本事实（知识）。并行函数的材料在附录B中被讲解。

![Figure12.1](media/Figure12.1.png)
**Figure 12.1:** 此处我们展示了渲染管线（rendering pipline）的一些步骤，这些步骤在本章中会被讲述（比较图示$\text{Figure 1.3 }$）

## 12.1 裁切阶段（Clipping）
裁切阶段的工作是处理完全或局部位于查看平截锥体（viewing frustum）之外的三角形。一些处理被完成是由于一个直接的理由-那就是我们希望抛弃看不见的几何体以最小化计算。但是，更重要的理由是，除非裁切被完成，否则我们会在跨越$z_e=0$的平面的三角形上出现问题（也就是说，在眼睛前后都有延伸）。

考虑图示$\text{Figure 12.2}$中的几何体（这里我们压制x方向以简化绘制）。本场景中，我们拥有一条单一的几何线段（在3D中，这会是一个三角形），其中一个顶点在眼睛前方而另一个在眼睛后方。假设我们应用第10章中的投射相机变换。前方的顶点会投射到接近底部的图像点，而后方的顶点会投射到接近底部的图像点。

如果，在屏幕上，我们只是绘制位于这些被投射的顶点之间的像素，我们会绘制在完全错误的区域。正确的渲染应该绘制始于靠近底部的被投射顶点，并且向下绘制直到到达图像的底部。

在计算机图像学中，解决这个问题最容易的方式是首先改变场景的几何形状，替换这种眼睛跨越的三角形为更小的不会延伸到眼睛后方的三角形（参考图示$\text{Figure 12.4}$）。这种步骤被称之为裁切（clipping）。实际上，我们依据视图平截锥体（frustum）的六个面裁切每个三角形，这六个面被近平面（near plane），远平面（far plane），以及“左、右、上、下”图像边缘所隐含限定。

针对这个问题，存在避免做任何几何裁切的方式。感兴趣的读者可以进一步阅读参考数目[52]。

## 用于裁切的坐标类型（Which Coordinates To Use）
我们可以在眼睛坐标中裁切，但是这种处理方式也需要知晓相机投射的参数，这一点是不方便的。反过来讲，在标准化设备坐标中验证裁切条件会非常经典：这种环境中，如果我们的三角形不离开经典矩型区域，我们近很乐见。

$$ \begin{matrix} −1 < x_n < 1 \qquad\qquad\qquad\qquad(12.1) \\ −1 < y_n < 1 \qquad\qquad\qquad\qquad(12.2) \\ −1 < z_n < 1 \qquad\qquad\qquad\qquad(12.3)\end{matrix} $$

不幸的是，在我们得到标准化设备坐标（normalized device coordinates）之时，引起麻烦的翻转问题已经发生。

计算机图形学中常见的解决方案是在所谓的裁切坐标（clip coordinates）空间中进行裁切，正如上面所描述。在这些裁切坐标中，方程式（12.1）中所列出的经典验证条件被平移改变为：

$$\begin{matrix}
    −w_c < x_c < w_c  \\ −w_c < y_c < w_c \\ −w_c < z_c < w_c
\end{matrix}$$

因为我们还没做除法，就没有翻转问题发生。在3D眼睛空间（3D eye space）中的三角形只是被映射为4D裁切空间（4D clip space）中的三角形。

![Figure12.2](media/Figure12.2.png)
**Figure 12.2:** 在这个场景中，橙色地面线段在眼睛前后延伸。其顶点以虚线展示的样子投射到图像平面。如果像素填充在被投射顶点之间，我们将得到错误的图像。

## 裁切过程相关变量的更新（Updating The Variables）
这里裁切步骤的实际编码超越本书的内容（兴趣）。感兴趣的读者可以查阅参考书目[5]。但是这个处理通常在三角形穿过视图平截锥体（frustum）的地方生成“新”顶点。这里我们会对和这些新顶点关联的变量（的获得方式）进行解读。

新顶点以裁切坐标的方式进行计算，从而用于自己的4个新关联的裁切坐标$[x_c,y_c,z_c,w_c]^t$。每个新顶点和旧顶点一样，关联了针对每个变异变量类型的一个集合的值。

全部变异变量（varying variables）都表达了在对象坐标$[x_o,y_o,z_o]^t$上的并行函数（affine functions），因此，借助B.5节中的推理，这些变量在$(x_c,y_c,z_c,w_c)$上也是并行函数。从而，如果新顶点位于两个三角形顶点的路径的某个分数比例-$\alpha$处，我们只需以“路径的$\alpha$比例”方式插值变异变量，同时借助这些新值设置这个顶点的变异变量。

## 12.2 背向面剔除（Backface Culling）
假设我们正在借助三角形绘制一个闭合固态物体，比如一个立方体。让我们标记每个三角形的两面为“前”和“后”。不管我们如何转动立方体，我们将绝不会看到任何三角形的背面，因为它必然被某个我们正在观察到的三角形的前面所阻挡。（参考图示$\text{Figure 12.5}$）。如此，我们也可以尽可能地剔除这些背后面向的多边形。并且完全不绘制他们。对于“非水密”（可透水）几何模型，我们可能希望看到一个三角形的两边；在这种情形中，背面提出就会不合适。

在OpenGL中，背面剔除通过调用`glEnable(GL_CULL_FACE)`开启。对于每个面，我们需要以某种方式告知OpenGL哪个面是前面，哪个面是后面。要做到这样，我们借助排序3个顶点顺序的方式，这种方式保证当我们观看其前面时，顶点是反时针方向（counter clockwise CCW）。当回至每个三角形时，我们在顶点缓存对象（vertex buffer object）中使用这种顶点排序。

借助顶点的标准化设备坐标，现在OpenGL可以轻松地在当前图像中确定我们正在观察一个三角形的哪一面。这里展示了这是如何做到的（参考图示$\text{Figure 12.6}$）。让$\tilde{p_1},\tilde{p_2},\tilde{p_3}$为被投射到$(x_n,y_n,0)$平面上三角形的3个顶点。定义矢量$\vec{a}=\tilde{p_3}-\tilde{p_2}$和$\vec{b}=\tilde{p_1}-\tilde{p_2}$。接着结算向量积（cross product）$\vec{c}=\vec{a} \times \vec{b}$。如果3个顶点在平面上是反时针方向排序，那么$\vec{c}$将朝向$+z_n$方向。否则它将会朝向$-z_n$方向。总之，所以我们需要做的事情就是以标准化设备坐标（normalized device coordinates）计算向量积的z坐标。当所有尘埃落定，这个坐标为

$$\large{ (x_n^3 − x_n^2 )(y_n^1 − y_n^2 ) − (y_n^3 − y_ n^2 )(x_n^1 − x_n^2 ) \qquad\qquad (12.4) }$$

要验证是否3个顶点是反时针方向，我们只要计算方程（12.4）的值即可。如果值为正则当从相机视角观看时顶点为反时针方向。


## 12.3 Viewport
当前，我们希望定位我们的三角形在图像屏幕上并且决定哪些像素会位于三角形内。因此，我们希望从抽象设备坐标（normalized device coordinates）变换到所谓的窗口坐标（window coordinates），在其上每个像素中心拥有一个整型坐标（integer coordinate）。这会让随后的像素计算更加自然。

回忆一下，在标准化设备坐标（normalized deviec coordinates）中，我们图像的范围是经典正方形（canonical square），拥有左下角坐标$[-1,-1]^t$,右上角坐标$[1,1]^t$。现在让我们假设我们的窗口为W像素宽和H像素高。我们想让左下角像素中心拥有一个为$[0,0]^t$2D窗口坐标,右上角像素中心拥有$[W-1,H-1]^t$坐标。我们认为每个像素拥有范围为正负0.5像素单位长度的，在水平和垂直方向延伸的不动产建筑（real estate）。因此，在这个模型中，我们认为每个像素为一个1像素乘1像素的单位正方型（unit square），其中心为整型坐标（integer coordinates）。如此被所有像素组合在一起所覆盖的2D窗口矩型就是从从左下角$[-0.5,-0.5]^t$到右上角$[W-0.5,H-0.5]^t$的窗口坐标。参考图示${\text{Figure 12.7}}$。

All that we need now is a transformation that performs the appropriate vertical and horizontal scales and shifts, such that it maps the canonical square to the window’s rectangle. We can verify that the following matrix provides the (unique) solution:
现在所有我们需要做的就是执行合适的水平和垂直伸缩及平移合成变换，以便于它将经典正方形映射为窗口矩型。

⎡ ⎤ ⎡ ⎤⎡ ⎤ x w W/2 0 0 (W − 1)/2 x n ⎢ y w ⎥ ⎢ 0 H/2 0 (H − 1)/2 ⎥ ⎢ y n ⎥ ⎢ ⎥ ⎢ ⎥ ⎢ ⎥ (12.5) ⎣ z w ⎦ = ⎣ 0 0 1/2 1/2 ⎦ ⎣ z n ⎦ 1 0 0 0 1 1

This matrix is called the viewport matrix, and it implements the viewport transformation. In OpenGL, we set up this viewport matrix with the call glViewport(0,0,W,H).

(A functionally equivalent model, that we will not use, but is often used in OpenGL documentation, deﬁnes the window x, y coordinates as ranging from [0, 0] t to [W, H] t , while thinking of each pixel center as have half-integer coordinates. In this model, we use a different matrix and we also use different coordinates to address the pixels. )

The third row of this matrix is used to map the [−1..1] range of z n values to the more convenient [0..1] range. And thus in our conventions, z w = 0 is far and z w = 1 is near. Thus we must also tell OpenGL that when we clear the z-buffer, we should set it to 0; we do this with the call glClearDepth(0.0).

12.3.1 Texture Viewport

For some (probably not very good) reason, the abstract domain for textures is not the canonical square, but instead is the unit square in [x t , y t ] t texture coordinates. Its lower left corner is [0, 0] t and upper right corner is [1, 1] t . Again assuming that the texture image is W pixels wide by H pixels high, in this case the coordinate transformation matrix is ⎡ ⎤ ⎡ ⎤⎡ ⎤ x w W 0 0 −1/2 x t ⎢ y w ⎥ ⎢ 0 H 0 −1/2 ⎥ ⎢ y t ⎥ ⎢ ⎥ ⎢ ⎥ ⎢ ⎥ (12.6) ⎣ − ⎦ = ⎣ − − − − ⎦⎣ − ⎦ 1 0 0 0 1 1

These details with the 1/2s and all may seem a bit picky, but understanding them is essential if you want to know exactly where the pixels are in your images and textures.

12.4 Rasterization

Rasterization is the process of taking the vertices of a triangle and ﬁlling in the pixels. Starting from the window coordinates for the three vertices, the rasterizer needs to ﬁgure out which pixel-centers are inside of the triangle. (Later in Section 16.3, we will explore the possibility of using numerous spatial samples within a pixel’s square to determine its color).

There are many ways that rasterization can be done in either hardware of software. Here, for example, is a simple brute force approach. See Figure 12.8. Each triangle on the screen can be deﬁned as the intersection of three half-spaces. Each such halfspace is deﬁned by a line that coincides with one of the edges of the triangle, and can be tested using an “edge function” of the from

edge = ax w + by w + c

where the (a, b, c) are constants that depend on the geometry of the edge. A positive value of this function at a pixel with coordinates [x w , y w ] t means that the pixel is inside the speciﬁed halfspace. If all three tests pass, then the pixel is inside the triangle.

This brute force approach can be sped up in many ways. For example, we can use the minimum and maximum x w and y w values of the three vertices to determine a bounding box of the triangle. Only pixels within this bounding box need to be tested.

In addition, to reduce the number of “pixel-in-triangle” tests, there are ways to design a simple conservative test that determines if an entire block of pixels is entirely inside or outside a triangle. Individual pixels only need to be tested when this test is inconclusive. As another optimization, we note that once we have evaluated such a linear function at, say, one pixel, we can then evaluate it at a neighboring pixel incrementally. For example, as we step one pixel horizontally, we just need to increment the evaluated edge function by the value a.

As input to rasterization, each vertex also has some auxiliary data associated with it. This data includes a z w value, as well as other data that is related, but not identical to the varying variables (see Chapter 13). It is also the job of the rasterizer to linearly interpolate this data over the triangle.

Each such value v to be linearly interpolated can be represented as an afﬁne function over screen space with the form

v = ax w + by w + c

(12.7)

The (a, b, c) constants can be determined using Equation (B.2) in Appendix B. An afﬁne function can be easily evaluated at each pixel by the rasterizer. Indeed, this is no different from evaluating the edge test functions just described.

During rasterization, it is important to carefully treat the boundary cases. In particular, suppose two triangles in a mesh abut along one edge whose projection lands exactly on a pixel. If the pixel is not drawn, then a gap will appear in the projected mesh. If the pixel is drawn twice, this can cause problems if we are modeling transparency using alpha blending (see Section 16.4). Boundary rules must be implemented carefully to ensure that such a pixel is only drawn once by this pair of triangles.

A related topic to triangle rasterization is line/curve rasterization. In this case, we are not looking for pixels “inside” of a 2D shape such as a triangle, but instead pixels “near” a 1D shape. Thus even deﬁning the problem is a bit trickier and one which we will not delve into here. The interested reader can consult [5].



