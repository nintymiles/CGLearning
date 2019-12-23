# 重采样（Resampling）
现在，图像采样和图像重建的基础我们都已然讨论过了，我们可以回顾纹理映射的处理过程。我们会看到，在这种情形中，我们始于一个具体图像然后终于一个具体图像。总之，这种映射技术性地涉及了重建和采样两个阶段。以此为上下文，我们将解释用于反锯齿纹理映射中的渐进式纹理映射（mip mapping）。

## 18.1 Ideal Resampling
Suppose we start with a discrete image or texture T[k][l] and apply some 2D warp to this image to obtain an output image I[i][j]. How should we set each of the output pixels? In particular, during texture mapping, the triangle and camera geometry effectively warp a texture image onto some portion of the rendered image in the window.

Ideally we should follow the following set of steps

• Reconstruct a continuous texture T (x t , y t ) using a set of basis functions B k,l (x t , y t ), as in Chapter 17.

• Apply the geometric warp to the continuous image.

• Integrate against a set of ﬁlters F i,j (x w , y w ) to obtain the discrete output image, as in Chapter 16.

Let the geometric transform be described by a mapping M(x w , y w ) which maps from continuous window to texture coordinates. Then, putting these three steps together, we obtain < < &

$$
I(i,j) \leftarrow \iint_{\Omega} dx_wdy_w F_{i,j}(x_w,y_w) \sum_{k,l} B_{k,l}(M(x_w,y_w)) T[k][l] \\
\sum_{k,l} T[k][l] \iint_{\Omega} dx_wdy_w F_{i,j}(x_w,y_w) B_{k,l}(M(x_w,y_w))
$$

