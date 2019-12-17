# 变异变量（Varying Variables）- 较难理解

为了表示跨越一个三角形的函数，在图形管线中从一个三角形顶点进行到这个三角形内的每个像素的阶段中，我们需要插值变异变量（varying variables）。本章中，我们探索这种插值如何被正确完成。令人吃惊地，这种看似寻常的步骤实际上是有点儿复杂的。阅读本章之前，你应该熟悉附录B中的材料。

## 13.1 解决问题的动机（Motivating The Problem）

在之前的图示$\text{Figure 11.4}$中，我们已经看到我们不能借助线性插值确定出一个点的正确的$z_e$值。这里有另一个激发解决问题的例子。让我们观察映射一个棋盘图案图片到一个立方体+z面的例子（参考图示$\text{Figure 13.1}$。这个面由两个三角形组成。我们希望给每个三角形粘贴上合适的棋盘图片的三角形状的一半。借助为每个顶点关联纹理坐标$[x_t,y_t]^t$来实现。在三角形内不，我们希望$[x_t,y_t]^t$被确定为三角形上的唯一线性插值体函数（unique interpolant functions），其在$[x_o,y_o,z_o]^t$坐标中并行。这些被插值的纹理坐标随后被用来获取色彩数据，这些数据位于纹理图像中的对应点。纹理映射和其变体在第15章中会被更详细地讲述。

如果我们以正交的方式观察这个立方体面，那么我们窗口的合适部分应该看起来就像是原始的棋盘图像。如果这个面以某种角度远离我们，正如图示$\text{Figure 13.1}$,那么我们期望看到某种“透视”效果；棋盘上越远的正方形应该显得越小，同时物体空间中的平行线应该显示要交汇于图像中正要消失的点。

OpenGL是如何获得这种正确图像呢？假设我们尝试在屏幕上通过简单线性插值以确定出像素上的$[x_t,y_t]^t$坐标。随后，当我们以某种固定的2D矢量位移（vector displacement）移动时，纹理坐标会被以某种固定的2D矢量位移在纹理坐标中被更新。这种情形中，所有纹理中的正方形会被映射为等尺寸的平行四边形。这会毫无疑问地给出我们错误图像（参考图示$\text{Figure 13.2}$）。甚至，因为不正确的事情发生在所观察面上两个三角形中的每一个之上，纹理的两半将会以不可预期的方式在这个面的对角线上相遇。

## 13.2 合理线性插值（Rational Linear Interpolation）

The problems just described arise because our desired functions (texture coordinates in this case) are simply not afﬁne functions over the screen variables (x w , y w ). If we use linear interpolation to blend the values on the screen, we are treating them as if they were, and get the wrong answer.

To ﬁgure out the correct way to interpolate this data, let us reason as follows:

Recall that given our modelview matrix M, and our projection matrix P, for each point on our triangle, the normalized device coordinate are related to object coordinates through the relation

⎡

⎢ ⎢ ⎣

x n w n y n w n z n w n

w

n

⎤

⎡

⎥ ⎢ ⎥ ⎢ ⎦ = PM ⎣

x o y o z o

1

⎤

⎥ ⎥ ⎦

Inverting our matrices, this implies that at each point on the triangle, we have the relation:

⎡

⎢ ⎢ ⎣

x o y o z o

1

⎤

⎥ ⎥ ⎦ =M

−1 −1 P

⎡

⎢ ⎢ ⎣

x n w y n w z n w

n n n

⎤

w n

⎥ ⎥ ⎦

Now suppose that v is an afﬁne functions of (x o , y o , z o ) (such as the x t texture coordinate). We also make use of the obvious fact that the constant function 1 is also afﬁne in (x o , y o , z o ). Thus, for some (a, b, c, d), we have ⎡ ⎤ ) ) * x o * v b c d ⎢ ⎢ y o ⎥ ⎥ a = ⎣ ⎦ 1 0 0 0 1 z o 1

and therefore:

)

v 1

*

=

=

) a

0

) e

i

b 0

c 0

d 1

* M

⎡

*

−1

P

−1

⎡

⎢ ⎢ ⎣

⎤

⎥ ⎥ ⎦

x n w n y n w n z n w n

w

n

⎤

⎥ ⎥ ⎦

f

j

g k

h l

121

⎢ ⎢ ⎣

x n w n y n w n z n w n

w

n

for some appropriate values e..l.

Now divide both sides by w n and we get

)

v

w n 1 w n

*

=

) e

i

f j

g k

h l

*

⎡

⎢ ⎢ ⎣

x n y n z n

1

⎤

⎥ ⎥ ⎦

This tell us that w v n and

1 w

are afﬁne functions of normalized device coordinates.

n

Using the fact that normalized device coordinates are related to window coordinates by a matrix multiplication (and no divisions) we can use the reasoning of Section B.5 v 1 to deduce that w and w are afﬁne functions of window coordinates (x w , y w , z w ). n n

Finally, since we started with a triangle, which is planar in object coordinates, we are also dealing with a planar object in window coordinates. Thus, assuming that the triangle has non-zero area in the window, we can apply the reasoning of Section B.4 to remove the dependence on z w .

1 w

Doing this, we can conclude: w v n and n are both afﬁne functions of (x w , y w ). This is great news. It means that we can calculate their values at each pixel, just given their values at the vertices. In fact, to compute them we do not even need to ﬁgure out the speciﬁc constant values of our derivation above. All we need are the values of v and w n at each vertex.

Now we can see how OpenGL can perform the correct interpolation to calculate v at each pixel. This process is called rational linear interpolation.

• The vertex shader is run on each vertex, calculating clip coordinates and varying variables for each vertex.

• Clipping is run on each triangle; this may create new vertices. Linear interpolation in clip coordinate space is run to determine the clip coordinates and varying variable values for each such new vertex.

• For each vertex, and for each varying variable v, OpenGL creates an internal variable w v n . Additionally, for each vertex OpenGL creates one internal variable 1 . w n

• For each vertex, division is done to obtain the normalized device coordinates.

x n = w x c c , y n = w y c c , z n = w z c c ,

• For each vertex, the normalized device coordinates are transformed to window coordinates.

• The [x w , y w ] t coordinates are used to position the triangle on the screen.

• For every interior pixel of the triangle, linear interpolation is used to obtain the 1 interpolated values of z w , w v n (for all v) and w n .

• At each pixel, the interpolated z w value is used for z-buffering.

• At each pixel, and for all varying variables, division is done on the interpolated 1 internal variables to obtain the correct answer v v = ( w )/( w ). n n

• The varying variable v is passed into the fragment shader.

In ﬁgure 13.3, we show how these steps ﬁt into the rendering pipeline.

