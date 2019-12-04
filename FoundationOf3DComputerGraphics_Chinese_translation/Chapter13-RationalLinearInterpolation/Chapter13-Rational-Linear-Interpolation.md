# 变异变量（Varying Variables） - 较难
In order to represent functions that vary across a triangle, we need to interpolate the varying variable data from the three vertices of a triangle to each pixel inside of the triangle. In this chapter we explore how this is properly done. Surprisingly, this innocuous looking step is actually a bit complicated. Before reading this chapter, you should be familiar with the material in Appendix B.

## 13.1 Motivating The Problem

We already saw above in Figure 11.4, that we cannot determine the correct z e value at a point using linear interpolation. Here is another motivating example. Let us look at the simple case of mapping an image of a checkerboard pattern onto the +z face of a cube (See Figure 13.1). This face is made up of two triangles. We wish to glue to each triangle the appropriate triangular half of the checkerboard image. We do this by associating [x t , y t ] t texture coordinates for each vertex. In the interior of a triangle, we wish [x t , y t ] t to be determined as the unique interpolant functions over the triangle that are afﬁne in (x o , y o , z o ). These interpolated texture coordinates can then be used to fetch color data from that point in the texture image. Texture mapping and its variants are discussed more fully in Chapter 15.

If we view the cube face perpendicularly, then the appropriate part of our window should look just like the original checkerboard image. If the face is angled away from us, as in Figure 13.1, then we expect to see some “foreshortening”; farther squares on the board should appear smaller, and parallel lines in object space should appear to converge at a vanishing point in the image.

How does OpenGL obtain this correct image? Suppose we attempt to determine [x t , y t ] t at the pixels by simply linearly interpolating them over the screen. Then, as we move by some ﬁxed 2D vector displacement on the screen, the texture coordinates will be updated by some ﬁxed 2D vector displacement in texture coordinates. In this case, all of the squares of the texture will map to equal sized parallelograms. This will clearly give us the wrong image (see Figure 13.2). Moreover, because incorrect things happen on each of the two triangles of our face, the two halves of the texture will meet up in an unexpected way over the face’s diagonal.

## 13.2 Rational Linear Interpolation

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

