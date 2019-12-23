# 控制球：轨迹和弧形（Balls: Track and Arc）
在一个交互式的计算机图形程序中，我们经常追踪鼠标的运动并且借助这种数据指定物体运动。平移处理起来相当直接。在小节6.5中，当鼠标左键被摁下，我们解读左/右鼠标运动为x轴方向平移，上/下鼠标运动为y轴方向平移（全部都关联于眼睛帧）。当鼠标右键被摁下，我们解读上/下运动为z轴方向平移。

旋转运动的指定就有一点儿不直观了；存在很多方式将鼠标运动链接为旋转变换，对于用户来说，这其中的每一种都让人有稍微不同的感觉。之前，在小节6.5中，我们描述了将鼠标位移解读为围绕x轴和y轴的某种特定旋转序列。本节中，我们会描述两种更加成熟的接口：弧形球（arcball）和轨迹球（trackball）。轨迹球的主要优势在于其让人感觉在空中移动一个真实的球体。弧形球的主要优点在于，如果用户在一个点开始移动鼠标，然后在另一个点结束，最终的选择变换不依赖于在这两点之间鼠标所采用的路径。

让我们假设我们正在关联于帧$\vec{\mathbf{a}}^t=\vec{\mathbf{w}}^t(O)_T(E)_R$移动物体，就如我们在小节5.2.1中所作。用户点击在屏幕上并且拖动鼠标。我们希望去解读这种用户运动为某种旋转Q，这种选择会被关联于$\vec{\mathbf{a}}^t$被应用。本章中，为了计算变换Q的值，我们将描述两种不同的方法，轨迹球和弧形球。

## 8.1 The Interfaces

We imagine a sphere of some chosen radius that is centered at ˜o, the origin of ⃗ o t . Often, it is useful to actually draw this sphere in wireframe surrounding the object so that the user can better feel what is happening. Suppose the user clicks on the screen at some pixel s 1 over the sphere in the image, we can interpret this as the user selecting some 3D point ˜p 1 on the sphere. Suppose the user then moves the mouse to some other pixel s 2 over the sphere, which we interpret as a second point ˜p 2 on the sphere.

Given these two points, deﬁne the two vectors ⃗v 1 , ⃗v 2 as the unit length vectors in the directions of the vectors ˜p 1 − ˜o and ˜p 2 − ˜o respectively. Deﬁne the angle φ = arccos(⃗v 1 · ⃗v 2 ) and the axis k = normalize(⃗v 1 × ⃗v 2 ). (See Figure 8.1.)

In the trackball interface, we deﬁne Q as the rotation of φ degrees about the axis k. In the arcball interface, we deﬁne Q as the rotation of 2φ degrees about the axis k.

## 8.2 Properties

The trackball interface is quite natural; it feels like the user is simply grabbing a physical point on a sphere and dragging it around. But there is an unexpected consequence, if the user ﬁrst moves her mouse on the screen from s 1 to s 2 , and then from s 2 to s 3 , the composed trackball rotations will be different from the result of moving the mouse directly from s 1 to s 3 ! In both cases, the point ˜p 1 will be rotated to ˜p 3 , but the two results can differ by some “twist” about the axis ˜o − ˜p 3 . This path dependence also exists in our simple rotation interface in Section 6.5.

The arcball interface has somewhat opposite properties. On the one hand, the object appears to spin twice as fast as expected. But, on the other hand, the arcball interface is indeed path independent. We can see this easily using quaternion operations. The rotation of 2φ degrees about the axis k can be represented by the quaternion ) * ) * ) *) * cos(φ) 0 0 vˆ 1 · vˆ 2 = = (8.1) sin(φ)k vˆ 1 × vˆ 2 vˆ 2 −ˆv 1

where k, vˆ 1 and vˆ 2 are the coordinate 3-vectors representing the vectors k, ⃗v 1 and ⃗v 2 with respect to the frame a t .

If we compose two arcball rotations, corresponding to motion from ˜p 1 to ˜p 2 followed by motion from ˜p 2 to ˜p 3 , we get ) *) * vˆ 2 · vˆ 3 vˆ 1 · vˆ 2 vˆ 2 × vˆ 3 vˆ 1 × vˆ 2

This is because both rotations are applied with respect to the frame ⃗ a t , which in fact is unchanged. Using the decomposition of Equation (8.1), we see that this is equal to:

) *) *) *) * ) *) * ) * 0 0 0 0 0 0 v ˆ · v ˆ 1 3 = = vˆ 3 −ˆv 2 vˆ 2 −ˆv 1 vˆ 3 −ˆv 1 vˆ 1 × vˆ 3

which is exactly what we would have gotten had we moved directly from ˜p 1 to ˜p 3 .

## 8.3 Implementation

Trackball and Arcball can be directly implemented using either 4 by 4 matrices or quaternions to represent the transformation Q.

Since all of the operations depend only on vectors and not on points, the origin of the coordinate system is unimportant and we can work in any coordinates system that shares the axes of a t , in particular, we can use eye coordinates.

One slightly tricky part is computing the coordinates of the point on the sphere corresponding to a selected pixel (this is essentially ray-tracing, which is covered in Chapter 20). One simple way to get approximately the correct behavior is to work in “window coordinates”. In this case, we think of a 3D space where the x-axis is the horizontal axis of the screen, the y-axis is the vertical axis of the screen, and the z-axis is coming out of the screen. We can think of the sphere’s center as simply sitting on the screen. Given the (x, y) window coordinates of the point where the user has clicked, we can easily ﬁnd the z coordinate on the sphere using the sphere-equation: (x−c x ) 2 +(y−c y ) 2 +(z−0) 2 −r 2 = 0, where [c x , c y , 0] t are the window coordinates of the center of the sphere.

Using this approach, we still need to compute the center of the sphere in window coordinates, as well as its projected radius on the screen. This will require an understanding of camera matrices, a topic we will cover in Chapter 10. For completeness, we give code for this procedure on the book’s website.

