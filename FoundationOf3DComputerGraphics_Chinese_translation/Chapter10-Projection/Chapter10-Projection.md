# 投射（Projection）

Until now, we have described our objects and eye in 3 dimensions. Our next step is to understand how to turn this into a 2D image as would be seen from the eye. To do this we will need to model a simple camera. Throughout, we will assume that the camera is placed at the origin of the eye frame ⃗ e t , and that it is looking down the eye’s negative z-axis. We will use the notation [x e , y e , z e , 1] t to refer to the eye coordinates of a point.

10.1 Pinhole Camera

The simplest possible camera is a pinhole camera (see Figure 10.1). As light travels towards the ﬁlm plane, most is blocked by an opaque surface placed at the z e = 0 plane. But we place a very small hole in the center of the surface, at the point with eye coordinates [0, 0, 0, 1] t . Only rays of light that pass through this point reach the ﬁlm plane and have their intensity recorded on ﬁlm. The image is recorded at a ﬁlm plane placed at, say, z e = 1.

In the physical world, any real camera needs a ﬁnite sized aperture so that a measurable amount of light can pass to the ﬁlm plane. And once the aperture is ﬁnite sized, a lens is needed to better “organize” and focus the incoming light. But these issues need not concern us now, as we are not building physical cameras. See Chapter 21 for more on this.

In our pinhole camera, the data on the ﬁlm plane needs to later be ﬂipped in order to obtain the desired photo. Mathematically, we can avoid this step if we instead model the pinhole camera with the ﬁlm plane in front of the pinhole, say at the z e = −1 plane (see Figure 10.2). This would not make any sense in the physical world, but it works just ﬁne as a mathematical model of a pinhole camera.

Once the picture has been created, if we hold up the photograph at the z e = −1 plane, and observe it with our own eye placed at the origin (see Figure 10.3), it will look to us just like the original scene would have. We are exactly reproducing the data that would have reached our eye if we had stood there and looked at the scene. If we move the picture around in space, say closer or farther from our eye, we will no longer be exactly reproducing the original scene-stimulus, but it will still appear like a reasonably valid visual representation of the original scene.

10.2 Basic Mathematical Model

This pinhole camera is easy to model mathematically. Let us use coordinates [x n , y n ] t to specify points on our ﬁlm plane. For the present, we choose a 2D coordinate system on our ﬁlm plane such that these coordinates happen to match the eye coordinates, i.e., x n = x e and y n = y e , but soon we shall relax this speciﬁcation.

Given a point ˜p in the scene with eye coordinates [x e , y e , z e , 1] t , it is easy enough to see (say using a similar-triangles argument) that the ray from ˜p to the origin hits the ﬁlm plane at

x

n

n

=

=

x e −

z

e

y e −

(10.1)

y

(10.2)

z

e

(10.3)

We can model this expression as a matrix operation as follows.

⎡

⎢ ⎢ ⎣

1

0

−

0

0

1

−

0

0

0

−

−1

0

0

−

0

⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

x e y e z e

1

⎤

⎡

⎥ ⎢ ⎥ ⎢ ⎦ = ⎣

x c y c

w c

⎤

⎡

⎥ ⎢ ⎥ ⎢ ⎦ = ⎣

x n w y n w

n n

⎤

⎥ ⎥ ⎦

(10.4)

w n

where a dash (−) means “don’t care (yet)”. We call this matrix the projection matrix. The raw output of the matrix multiply, [x c , y c , −, w c ] t , are called the clip coordinates of ˜p. (They are so named because this raw data is then used in the clipping phase, described in Section 12.1.) w n = w c is a new variable called the w-coordinate. In such clip coordinates, the fourth entry of the coordinate 4-vector is not necessarily a zero or a one.

We say that x n w n = x c and y n w n = y c . If we want to extract x n alone, we must perform the division x n = x n w w n n (and likewise for y n ). When we do this, we exactly recover the computation of Equation 10.1, our simple camera model.

Our output coordinates, with subscripts “n”, are called normalized device coordinates because they address points on the image in abstract units without speciﬁc reference to numbers of pixels. In computer graphics, we keep all of the image data in the canonical square −1 ≤ x n ≤ +1, −1 ≤ y n ≤ +1, and ultimately map this onto a window on the screen. Data outside of this square is not be recorded or displayed. This is exactly the model we used to describe 2D OpenGL drawing in Appendix A.

10.3 Variations

By changing the entries in the projection matrix we can slightly alter the geometry of the camera transformation.

10.3.1 Scales

If we were to move the ﬁlm plane to z e = n, where n is some negative number (see Figure 10.4), we could model this camera as

x

n

y

n

= =

x e n z e

y e n z e

This corresponds to changing the zoom on a lens. In matrix form, this becomes ⎡ ⎤ ⎡ ⎤⎡ ⎤ x n w n −n 0 0 0 x e ⎢ y n w n ⎥ ⎢ 0 −n 0 0 ⎥ ⎢ y e ⎥ ⎢ ⎥ ⎢ ⎥⎢ ⎥

⎦ = ⎣

⎦ ⎣ z e − − − 0 0 −1 0 1 This is in fact the same, as starting with our original camera with the ﬁlm plane at z e = −1, then scaling the recorded image by a factor of −n, and ﬁnally keeping only the part that is inside of the canonical square (see Figure 10.5). As such, it is best to no longer think of the normalized device coordinates as agreeing with any eye-coordinates on the ﬁlm plane, but simply as some intrinsic coordinates in the image plane.

−

⎣

w n

⎦

One useful way of controlling the scale factor is to determine −n by specifying instead the vertical angular ﬁeld of view of the desired camera. In particular suppose we want our camera to have a ﬁeld of view of θ degrees. Then we can set −n =

(See Figure 10.6), giving us the projection matrix ⎡ 1 0 0 tan( θ ) 2

⎤

0

1 tan(

0

0

θ 2

)

⎥ 0 ⎥ ⎥ − ⎦

⎢ ⎢ ⎢ ⎣

0

0

−1

0

1 tan(

θ 2

)

(10.5)

We can directly verify that any point who’s ray from the origin forms a vertical angle of θ with the negative z axis maps to the boundary of the canonical square in the image 2 plane, and therefore that the camera’s ﬁeld of view is θ. For example the point with eye coordinates: [0, tan( θ ), −1, 1] t maps to normalized device coordinates [0, 1] t . 2

More generally, we can scale our original image by differing horizontal and vertical factors of s x and s y , respectively, to obtain the following camera model ⎡ ⎤ ⎡ ⎤⎡ ⎤⎡ ⎤ x n w n s x 0 0 0 1 0 0 0 x e ⎢ y n w n ⎥ ⎢ 0 s y 0 0 ⎥ ⎢ 0 1 0 0 ⎥ ⎢ y e ⎥ ⎢ ⎥ = ⎢ ⎥⎢ ⎥⎢ ⎥ ⎣ − ⎦ ⎣ 0 0 1 0 ⎦ ⎣ − − − − ⎦ ⎣ z e ⎦

w

n

0

0

0

0 0 −1

1

=

⎡

⎢ ⎢ ⎣

s x 0 −

0

0 s y −

0

0 0 0

0 ⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

0

−1

0

1

x e y e z e

1

⎤

⎥ ⎥ ⎦

In computer graphics, this non-uniformity is useful when dealing with non-square windows on the screen. Suppose the window is wider than it is high. In our camera transform, we need to squish things horizontally so a wider horizontal ﬁeld of view ﬁts into our retained canonical square. When the data is later mapped to the window, it will be stretched out correspondingly and will not appear distorted.

Deﬁne a, the aspect ratio of a window, to be its width divided by its height (measured say in pixels). We can then set our projection matrix to be ⎡ 1 ⎤ 0 0 0 a tan( θ ) 2 ⎢ 1 ⎥ ⎢ 0 0 0 ⎥ ⎢ tan( θ ) ⎥ (10.6) 2 ⎣ − − − − ⎦ 0 0 −1 0

The vertical behavior of this camera is unchanged from that of Equation (10.5), but an appropriately wider horizontal ﬁeld is used to generate the image.

When the window is taller than it is wide, and thus a < 1, we can still use the matrix of Equation (10.6), but we may not be happy with the resulting narrow horizontal ﬁeld of view. If we want θ to be the minimal vertical/horizontal ﬁeld of view, then, whenever a < 1, we need to use the following projection matrix ⎡ 1 ⎤ 0 0 0 tan( θ ) 2 ⎢ a ⎥ ⎢ 0 0 0 ⎥ ⎢ tan( θ ) ⎥ 2 ⎣ − − − − ⎦ 0 0 −1 0

These are exactly the matrices that are produced by our proceduremakeProjection that we called in Section (6.2).
There is often a trade-off when choosing a ﬁeld of view to use in computer graphics. On the one hand, a wider ﬁeld of view, in say a game environment, lets the viewer see more of what is around them. On the other hand, in a typical viewing environment (unless one has their head right up against the screen), the screen occupies only a small angular extent of the viewer’s surroundings. In this case, the viewing geometry will not match the imaging environment (as it did in Figure 10.3), and the image will have a distorted appearance (for example, spheres may not appear as circles).

10.3.2 Shifts

Less commonly, we may wish to translate the 2D normalized device coordinates by [c x , c y ] t . This can be modeled in the projection matrix as ⎡ ⎤ ⎡ ⎤⎡ ⎤⎡ ⎤ x n w n 1 0 0 c x 1 0 0 0 x e ⎢ y n w n ⎥ ⎢ 0 1 0 c y ⎥ ⎢ 0 1 0 0 ⎥ ⎢ y e ⎥ ⎢ ⎥ ⎢ ⎥⎢ ⎥⎢ ⎥

=

⎣

−

w n

⎦

⎣

⎡

⎢ ⎢ ⎣

0 0

1 0 0

0 0

1 0

0 1

−c x −c y

−1

=

0 1 0

⎦⎣

0 0 0

0 ⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

0

x e y e

z e 1

−1 ⎤

⎥ ⎥ ⎦

0

⎦⎣

z e 1

⎦

This corresponds to a camera with a shifted ﬁlm plane (see Figure 10.7). This may seem uncommon, but in fact due to manufacturing as well as optical issues, most real cameras do have some small shifts in them.

In computer graphics, the main use for shifted cameras is for tiled displays (see Figure 10.8) where we place multiple displays next to each other to create a larger one. In this case, each of these sub-images is correctly modeled as an appropriately shifted camera. Another application is for creating pairs of images for stereo viewing on a single screen.

Often in computer graphics, shifted (and scaled) cameras are speciﬁed by ﬁrst specifying a near plane z e = n. On this plane, a rectangle is speciﬁed with the eye coordinates of an axis aligned rectangle. (For non-distorted output, the aspect ratio of this rectangle should match that of the ﬁnal window.) The values l, r specify the left and right sides of this rectangle in x e coordinates, while the values t, b specify the top and bottom of this rectangle in y e coordinates. In all, this speciﬁes the shape of a 3D frustum in space. Rays through this rectangle and incident to the origin are then mapped to the canonical image square using the projection matrix ⎡ 2n r+l ⎤ − r−l 0 r−l 0 2n ⎢ ⎢ 0 − t−b t−b t+b 0 ⎥ ⎥ (10.7) ⎣ − − − − ⎦ 0 0 −1 0

(See Figure 10.9).

10.3.3 ...And The Rest

The remaining two zeros in the upper left 2 by 2 block of the matrix in Equation (10.7) are typically not touched in our camera models. Together, they represent the rotation and shearing of the pixel grid. Shearing does not typically arise in real cameras nor is it often useful in computer graphics. Of course we can rotate the entire camera about its optical axis, but this can be done as an appropriate camera rotation in the original deﬁnition of the ⃗ e t frame!

10.4 Context

The projection operation we have described is a mapping that can be applied to any point given in eye coordinates, to obtain its normalized device coordinates. In OpenGL, though, this mapping is only applied to the vertices of a triangle. Once the normalized device coordinates of the three vertices of a triangle are obtained, the interior points are simply ﬁlled in by computing all of the pixels that fall in the interior of the triangle on the image plane.



