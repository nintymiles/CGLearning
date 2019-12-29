# 

# 深度（Depth）

## 11.1 Visibility

In the physical world, if object A is in front of object B, then the light from object B will be blocked by object A before reaching the camera, and it will not appear in the image (for example, the blue hexagon of ﬁgure 10.2). In computer graphics, we need to model this computationally.

There are a variety of approaches that can be used to ensure that only surfaces that are visible to the camera appear in the image. One idea is to sort the triangles by their depths, and then draw them in back to front order. The idea is, that the frontmost triangles will redraw over the occluded triangles and produce the correct image. There are a number of difﬁculties with this so-called painter’s approach. For example, a scene may include interpenetrating triangles. It may also include visibility cycles of non interpenetrating triangles such as shown in Figure 11.1.

Another very general approach, which we will discuss in Chapter 20, is ray casting. In this approach, for each pixel, we explicitly calculate every scene point observed along the pixel’s line of sight. The closest intersected point is then used to color the pixel.

In a rasterization-based renderer such as OpenGL, we use something called a zbuffer to perform our visibility calculations. In this approach, the triangles can be drawn in any order. At each pixel of our framebuffer, we store not only a color value but also a “current depth” value. This represents the depth of the geometry that was used to set the current value of the pixel. When a new triangle tries to set the color of a pixel, we ﬁrst compare its depth to the value stored in the z-buffer. Only if the observed point in this triangle is closer do we overwrite the color and depth values of this pixel. Since this is done on a per-pixel basis, it can properly handle interpenetrating triangles.

### Other Uses of Visibility Calculations

Visibility calculations can also be important when calculating the color of an observed point. In particular, it may be important to determine if the observed point can directly see some “light source” or whether it is in shadow. In OpenGL this can be done using an algorithm called shadow mapping, described in Section 15.5.2. In the context of ray tracing, we can simply use our ray intersection code to see what geometry is intersected by the ray going from the observed point towards the light.

Visibility computation can also be used to speed up the rendering process. If we know that some object is occluded from the camera, then we don’t have to render the object in the ﬁrst place. This might be used for example in an indoor scene where we can typically not see too far past the surrounding room. In this context, we can use a conservative visibility test; such a test quickly tells us whether some object may be, or is deﬁnitely not visible. If the object may be visible, then we continue and render the object with our z-buffer. But if the object is deﬁnitely not visible, then we can skip its drawing entirely.

## 11.2 Basic Mathematical Model

In OpenGL, we use a z-buffer to compute visibility. To use a z-buffer, in addition to the [x n , y n ] coordinates of a point, we also need a depth value. To accomplish this, for every point described in eye-coordinates, we deﬁne its [x n , y n , z n ] t coordinates using the following matrix expression.

$$ \begin{array}{c}
\begin{bmatrix} x_nw_n \\ y_nw_n \\ z_nw_n \\ w_n \end{bmatrix}  = \begin{bmatrix} x_c \\ y_c \\ z_c \\ w_c \end{bmatrix}  =  \begin{bmatrix} s_x & 0 & -c_x & 0 \\ 0 & s_y & -c_y & 0 \\ 0 & 0 & 0 & 1 \\ 0 & 0 & -1 & 0 \end{bmatrix}\begin{bmatrix} x_e \\ y_e \\ z_e \\ 1 \end{bmatrix}  \tag{11.1}
\end{array} $$

Again, the raw output are called the clip coordinates, and as before, to obtain the values of x n and y n , we need to divide by the w n value. But now we also have the value z

n

= −1 z . Our plan is to use this z n value to do depth comparisons in our z-buffer. e

First let us verify that the z n values resulting from Equation (11.1) can indeed be used for depth comparison. Given two points ˜p 1 and ˜p 2 with eye coordinates [x e 1 , y e 1 , z e 1 , 1] t and [x e 2 , y e 2 , z e 2 , 1] t . Suppose that they both are in front of the eye, i.e., 2 1 z e 1 < 0 and z e 2 < 0. And suppose that ˜p 1 is closer to the eye than ˜p 2 , that is z e < z e . 2 1 Then − z 1 2 < − z 1 1 , meaning z n < z n .

e

e

All together, we can now think of the process of taking points given by eye coordinates to points given by normalized device coordinates as an honest to goodness 3D geometric transformation. This kind of transformation is generally neither linear nor afﬁne, but is something called a 3D projective transformation.

Projective transformations are a bit funny; for example, in the above case, any point with z e = 0 will cause a “divide by zero”. At this juncture, the most important thing we need to know about projective transformations (the argument is given below) is that they preserve co-linearity and co-planarity of points (See Figures 11.2 and 11.3). Colinearity means that, if three or more points are on single line, the transformed points will also be on some single line.

As a result of this preservation of co-planarity, we know that for points on a ﬁxed triangle, we will have z n = ax n + by n + c, for some ﬁxed a, b and c. Thus, the correct z n value for a point can be computed using linear interpolation over the 2D image domain as long as we know its value at the three vertices of the triangle (see Appendix B for more on linear interpolation.)

Note, though, that distances are not preserved by a projective transform. Looking again at Figures 11.2 and 11.3, we see that evenly spaced pixels on the ﬁlm do not correspond to evenly spaced points on the geometry in eye-space. Meanwhile, such evenly spaced pixels do correspond to evenly spaced points in normalized device coordinates.

As a result of this distance-distortion, linear interpolation of z e values over the screen would give the wrong answer. Consider the 2D drawing of Figure 11.4, with interpenetrating segments, one orange and one blue. The z e values of the vertices are shown. The two segments cross right at the midpoint of the image plane, so in the correct image, the top half should be blue and the bottom half should be orange. Suppose we linearly interpolated the z e values in each segment in image space. All of the blue pixels would have interpolated z e values of −1. On the orange triangle, going from bottom (with z e = −0.1) to top (with z e = −1000000 or so), the interpolated z e value would become less than −1 almost immediately. Thus in the z-buffered image, almost the entire image would be blue! In Chapter 13 we will see how we can properly interpolate the z e value over a triangle on the screen.

### Lines are preserved by projective transforms (optional)

Here we sketch the steps needed to establish that lines are preserved by projective transforms.

First, let us deﬁne a projective transform acting on a 3D afﬁne space. Fixing our frame, each point is represented by c, a 4-coordinate vector with 1 as its last entry. We then multiply this by a chosen 4-by-4 matrix P, to obtain d = Pc. There are no restrictions on the fourth row of P, but we will assume that the matrix is invertible. Finally, we divide each of the four entries in d by the fourth entry of d. This gives us our result e, which we interpret as the coordinates of another point in our afﬁne space. (Let us ignore the case where the fourth entry happens to be 0.) Such a transformation c ⇒ e. is called a projective transformation.

To see that this kind of transformation preserves co-linearity, let us think of c as a coordinate vector in the 4D linear space R 4 . If we started with points that all lie on some line in our 3D afﬁne space, then all the associated c coordinates must also represent vectors in R 4 that lie on some corresponding 2D vector subspace of R 4 . Let us now think of the multiplication by P as simply a linear transform acting on vectors in R 4 . This, as any linear transform, must map 2D subspaces to 2D subspaces. So all of the resulting d coordinates must lie on some ﬁxed 2D subspace in R 4 . Finally, when we divide by the fourth coordinate, we are simply scaling each of the resulting vectors so that they also lie on the 3D hyperplane in R 4 with a last coordinate of 1. Any such e must lie in a 1D line within the 3D hyperplane. This hyperplane is isomorphic to our 3D afﬁne space, and thus the resulting points of the projective transform must be co-linear.

## 11.3 Near And Far

When using the projection matrix of Equation (11.1), there can be numerical difﬁculties when computing z n . As z e goes towards zero, the z n value diverges off towards inﬁnity. Conversely, points very far from the eye have z n values very close to zero. The z n of two such far away points may be indistinguishable in a ﬁnite precision representation, and thus the z-buffer will be ineffective in distinguishing which is closer to the eye.

In computer graphics, this issue is typically dealt with by replacing the third row of the matrix in Equation (11.1) with the more general row [0, 0, α, β]. Once again, it is easy to verify that if the values α and β are both positive, then the z-ordering of points (assuming they all have negative z e values) is preserved under the projective transform.

To set α and β, we ﬁrst select depth values n and f called the near and far values (both negative), such that our main region of interest in the scene is sandwiched be2fn tween z e = n and z e = f. Given these selections, we set α = f−n f+n and β = − f−n . We can verify now that any point with z e = f maps to a point with z n = −1 and that a point with z e = n maps to a point with z n = 1 (See Figures 11.5 and 11.6). Any geometry not in this [near..far] range is clipped away by OpenGL and ignored (clipping is covered in Section 12.1).

Putting this together with the projection matrix of Equation (10.6), we obtain the matrix
$$ \begin{bmatrix}
 \frac{1}{\alpha\tan(\frac{\theta}{2})} & 0 & 0 & 0 \\
 0 & \frac{1}{\tan(\frac{\theta}{2})} & 0 & 0 \\
 0 & 0 & \frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
 0 & 0 & -1 & 0 
\end{bmatrix}$$

Alternatively, putting this together with the projection matrix of Equation (10.7), and using the near plane to deﬁne our bounding rectangle using the scalars l, r, b, t, we obtain the frustum projection matrix 

$$ \begin{bmatrix}
 \frac{1}{\alpha\tan(\frac{\theta}{2})} & 0 & \frac{r+l}{r-l} & 0 \\
 0 & \frac{1}{\tan(\frac{\theta}{2})} & \frac{t+b}{t-b} & 0 \\
 0 & 0 & \frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
 0 & 0 & -1 & 0 
\end{bmatrix} \tag{11.2}$$

The effect of this projection matrix is to map the frustum to the canonical cube with corners [−1, −1, −1] t , [1, 1, 1] t .

For various historical reasons, there are a few variants of this matrix that appear in the literature and in OpenGL documentation. For example, some conventions assume that n is given as the distance to the near plane, and is thus a positive number. As another example, some conventions ﬂip the signs of the entries in the third row of the matrix. In the resulting (left handed) normalized device coordinates, nearer means smaller z n . Since we do not do this ﬂipping, we must explicitly tell OpenGL which sign to use for depth comparison; we do this with the call to glDepthFunc(GL GREATER).

## 11.4 Code

In OpenGL, use of the z-buffer is turned on with a call to glEnable(GL DEPTH TEST). We also need a call to glDepthFunc(GL GREATER), since we are using a right handed coordinate system where “more-negative” is “farther from the eye”.

Putting this all together, all we really need to do is to write procedures Matrix4 makeProjection(double minfov, double aspectratio, double zNear, double zFar) and Matrix4 makeProjection(double top, double bottom, double left, double right, double zNear, double ZFar) that return the appropriate projection matrices. We also need the procedure sendProjectionMatrix(projmat) to send the projection matrix to the appropriately named matrix variable in the vertex shader. The code for these procedures is found on our website. The actual multiplication with the projection matrix is done in the vertex shader as we did in Section 6.3.


