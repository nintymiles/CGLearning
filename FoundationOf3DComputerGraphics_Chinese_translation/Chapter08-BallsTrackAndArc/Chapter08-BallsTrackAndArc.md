# 控制球：轨迹和弧形（Balls: Track and Arc）
在一个交互式的计算机图形程序中，我们经常追踪鼠标的运动并且借助这种数据指定物体运动。平移处理起来相当直接。在小节6.5中，当鼠标左键被摁下，我们解读左/右鼠标运动为x轴方向平移，上/下鼠标运动为y轴方向平移（全部都关联于眼睛帧）。当鼠标右键被摁下，我们解读上/下运动为z轴方向平移。

旋转运动的指定就有一点儿不直观了；存在很多方式将鼠标运动链接为旋转变换，对于用户来说，这其中的每一种都让人有稍微不同的感觉。之前，在小节6.5中，我们描述了将鼠标位移解读为围绕x轴和y轴的某种特定旋转序列。本节中，我们会描述两种更加成熟的接口：弧形球（arcball）和轨迹球（trackball）。轨迹球的主要优势在于其让人感觉在空中移动一个真实的球体。弧形球的主要优点在于，如果用户在一个点开始移动鼠标，然后在另一个点结束，最终的选择变换不依赖于在这两点之间鼠标所采用的路径。

让我们假设我们正在关联于帧$\vec{\mathbf{a}}^t=\vec{\mathbf{w}}^t(O)_T(E)_R$移动物体，就如我们在小节5.2.1中所作。用户点击在屏幕上并且拖动鼠标。我们希望去解读这种用户运动为某种旋转Q，这种选择会被关联于$\vec{\mathbf{a}}^t$被应用。本章中，为了计算变换Q的值，我们将描述两种不同的方法，轨迹球和弧形球。

## 8.1 接口定义（The Interfaces）
我们假设拥有某种选定半径的球体，其中心在$\tilde{o}$，$\vec{\mathbf{o}}^t$的原点。通常，在实际中围绕对象用线框图绘制球体是很用的，用户可以更好的感知发生了什么。假设用户在屏幕上点击图像中球体上的像素$s_1$，我们可以解读这个动作为用户在球体上选定了某个3D点$\tilde{p}_1$。假设用户随后又移动到在球体上的另一个像素$s_2$，这个点我们解读为球体上的第二个对应点$\tilde{p}_2$。

给出这两个点，定义出两个矢量$\vec{v}_1,\vec{v}_2$为单位矢量（unit vectors），它们分别位于矢量$\tilde{p}_1 - \tilde{o}$和$\tilde{p}_2-\tilde{o}$的方向上。定义角度$\phi = \arccos(\vec{v}_1.\vec{v}_2)$和轴$\vec{k}=normalize(\vec{v}_1 \times \vec{v}_2)$。（参考图示$\text{Figure 8.1}$。）

在轨迹球接口中，我们定义Q为围绕轴$\vec{k}$旋转$\phi$角度的变换。而在弧形球中，我们定义Q为围绕轴$\vec{k}$旋转$2\phi$角度的变换。

## 8.2 属性（Properties）
轨迹球接口是十分自然的；感觉就像用户只是抓住了一个球体的真实的某个点然后来回拖动。但是也存在一个无法预料的后果，如果用户在屏幕上从$s_1$移动到$s_2$，然后又从$s_2$移动到$s_3$，合成的轨迹球旋转将不同于直接从$s_1$移动到$s_3$的旋转！在两种情形中，点$\tilde{p}_1$都会被旋转到$\tilde{p}_3$，但是两种结果可通过某种围绕轴$\tilde{o}-\tilde{p}_3$的“扭转”区分。这种路径依赖也存在于小节6.5中的简单旋转接口中。

弧形球接口拥有某种程度上互相对立的属性。一方面，物体看起来以预期两倍快的速度旋转，另一方面，弧形球接口确实是路径独立的。我们可以轻易地借助四元数（quaternion）操作来明白这点。围绕轴$\vec{k}$进行$2\phi$角度的旋转可以用四元数表达为
$$
\begin{bmatrix}cos(\phi) \\ sin(\phi)\vec{k}\end{bmatrix} = 
\begin{bmatrix}\hat{V}_1.\hat{V}_2 \\ \hat{V}_1 \times \hat{V}_2\end{bmatrix} = \begin{bmatrix} 0 \\ \hat{V}_2 \end{bmatrix} \begin{bmatrix} 0 \\ -\hat{V}_1 \end{bmatrix}  \qquad \qquad (8.1)
$$
其中$\vec{k},\hat{V}_1,\hat{V}_2$为3部件坐标矢量（coordinate 3-vectors）表达关联于帧$\vec{\mathbf{a}}^t$的矢量$\vec{k},\vec{v}_1,\vec{k}_2$。

If we compose two arcball rotations, corresponding to motion from ˜p 1 to ˜p 2 followed by motion from ˜p 2 to ˜p 3 , we get ) *) * vˆ 2 · vˆ 3 vˆ 1 · vˆ 2 vˆ 2 × vˆ 3 vˆ 1 × vˆ 2

This is because both rotations are applied with respect to the frame ⃗ a t , which in fact is unchanged. Using the decomposition of Equation (8.1), we see that this is equal to:

) *) *) *) * ) *) * ) * 0 0 0 0 0 0 v ˆ · v ˆ 1 3 = = vˆ 3 −ˆv 2 vˆ 2 −ˆv 1 vˆ 3 −ˆv 1 vˆ 1 × vˆ 3

which is exactly what we would have gotten had we moved directly from ˜p 1 to ˜p 3 .

## 8.3 Implementation

Trackball and Arcball can be directly implemented using either 4 by 4 matrices or quaternions to represent the transformation Q.

Since all of the operations depend only on vectors and not on points, the origin of the coordinate system is unimportant and we can work in any coordinates system that shares the axes of a t , in particular, we can use eye coordinates.

One slightly tricky part is computing the coordinates of the point on the sphere corresponding to a selected pixel (this is essentially ray-tracing, which is covered in Chapter 20). One simple way to get approximately the correct behavior is to work in “window coordinates”. In this case, we think of a 3D space where the x-axis is the horizontal axis of the screen, the y-axis is the vertical axis of the screen, and the z-axis is coming out of the screen. We can think of the sphere’s center as simply sitting on the screen. Given the (x, y) window coordinates of the point where the user has clicked, we can easily ﬁnd the z coordinate on the sphere using the sphere-equation: (x−c x ) 2 +(y−c y ) 2 +(z−0) 2 −r 2 = 0, where [c x , c y , 0] t are the window coordinates of the center of the sphere.

Using this approach, we still need to compute the center of the sphere in window coordinates, as well as its projected radius on the screen. This will require an understanding of camera matrices, a topic we will cover in Chapter 10. For completeness, we give code for this procedure on the book’s website.

