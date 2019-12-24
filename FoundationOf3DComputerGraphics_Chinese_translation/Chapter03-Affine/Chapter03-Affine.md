# 仿射（并行）（Afﬁne）

## 3.1 点和帧（Points and Frames）
将点和矢量看作是两种不同的概念是有用的。点表示在几何世界中的某种固定位置，而矢量表示世界中两个点之间的运动。我们会使用两种不同的标记区分点和矢量。矢量$\vec{v}$会有一个箭头在顶部，而点$\tilde{p}$会有波浪线在顶部。

如果我们认为矢量表达两点之间的运动，那么矢量操作（加法和标量乘法）就有明确的意义。如果我们把两个矢量加起来，我们在表达两个运动的串接（concatenation）。如果我们用一个标量乘以矢量，我们在通过某个因子增加或减少运动。零矢量（zero vector）为一个特别矢量，其代表没有运动。

这些操作对于点不会真正产生任何意义。把两个点加起来应该表示什么含义，比如说，哈佛广场加上Kendall广场（这里是两个地点名称）是什么？一个点被一个标量相乘又指得什么？什么是北极点的7倍？是否存在一个零点（zero point）和其它点的行为不一样？

存在一种在两个点之间确实有意义的操作：减法。当我们从另一个点减去一个点，我们应该会得到从第二个点到第一个点路径之间的运动，
$$
\tilde{p} - \tilde{q} = \vec{v}
$$

反过来说，如果我们从一个点开始，然后移动一个矢量（位移），我们应该会到达另一个点。
$$
\tilde{q} + \vec{v} = \tilde{p}
$$

对一个点应用线性变换同样有意义。例如我们可以想象一个点围绕某个固定原点的旋转。而且平移点也是有意义的（但是这个概念对于矢量没有任何意义）。要表达平移，我们需要开发仿射变换（或并行变换 affine transformation）的概念。要完成这个任务，我们借助$4 \times 4$矩阵。这些矩阵不仅对于处理本章的仿射（并行）变换很方便，而且对于描述（随后在第十章会看到的）相机投射变换也是很有帮助。

### 3.1.1 帧（Frames）

In an afﬁne space, we describe any point ˜p by ﬁrst starting from some origin point ˜o, and then adding to it a linear combination of vectors. These vectors are expressed using coordinates c i and a basis of vectors.

˜p = ˜o +

&

i

⃗

c i b i

=

'

⃗

b 1

⃗

b 3

⃗

b 2

˜o

⃗

b 3

˜o

(

⎡ ⎢

⎢ ⎣

c 1

c 2 c 3

1

⎤

⎥ ⎥ ⎦ =

⃗ f

t c

where 1˜o is deﬁned to be ˜o.

The row

'

⃗

b 1

⃗

b 2

(

=f

t

is called an afﬁne frame; it is like a basis, but it is made up of three vectors and a single point.

In order to specify a point using a frame, we use a coordinate 4-vector with four entries, with the last entry always being a one. To express a vector using an afﬁne frame, we use a coordinate vector with a 0 as the fourth coordinate (i.e., it is simply a sum of the basis vectors). The use of coordinate 4-vectors to represent our geometry (as well as 4-by-4 matrices) will also come in handy in Chapter 10 when we model the behavior of a pinhole camera.

3.2 Afﬁne transformations and Four by Four Matrices

Similar to the case of linear transformations, we would like to deﬁne a notion of afﬁne transformations on points by placing an appropriate matrix between a coordinate 4vector and a frame.

Let us deﬁne an afﬁne matrix to be a 4 by 4 matrix of the form ⎡ ⎤ a b c d ⎢ e f g h ⎥
We apply an afﬁne transform to a point ˜p = f t c as follows

'

'

⃗

b 1

⃗

b 1

⃗

b 2

⃗

b 2

⃗

b 3

⃗

b 3

˜o

˜o

(

(

⎡ ⎢

⎢ ⎣

⎡ ⎢

⎢ ⎣

c 1

c 2 c 3

1

a e i

0

⎤

⎥ ⎥ ⎦

⇒

b f j

c g k

0

d h l

1

⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

c 1 c 2 c 3

1

⎤

0

⎥ ⎥ ⎦

or for short

f t c ⇒ f t Ac

We can verify that the second line of the above describes a valid point, since the multiplication of ⎡ ′ ⎤ ⎡ ⎤⎡ ⎤ x a b c d x ⎢ ⎢ y ′ ⎥ ⎥ ⎢ ⎢ e f g h ⎥ ⎥ ⎢ ⎢ y ⎥ ⎥ = ⎣ z ′ ⎦ ⎣ i j k l ⎦ ⎣ z ⎦

1

0

0

0

1

1

gives us a coordinate 4-vector with a 1 as the fourth entry. Alternatively, we can see that the multiplication of ⎡ ⎤ a b c d ' ( ' ( ⎢ e f g h ⎥ b 1 ′ b 2 ′ b 3 ′ ˜o ′ = b 1 b 2 b 3 ˜o ⎣ ⎢ ⎦ ⎥ i j k l 0 0 0 1

where 0˜o is deﬁned to be 0, gives a valid frame made up of three vectors and a point.

Also note that if the last row of the matrix were not [0, 0, 0, 1] it would generally give us an invalid result.

Similar to the case of linear transform, we can apply an afﬁne transformation to a frame as ⎡ ⎤ a b c d ' ( ' ( e f g h ⎥ ⎢ ⎢ ⎥ b 1 b 2 b 3 ˜o ⇒ b 1 b 2 b 3 ˜o ⎣ ⎦

or for short

f t ⇒ f t A

3.3 Applying Linear Transformations to Points

Suppose we have a 3 by 3 matrix representing a linear transformation. we can embed it into the upper left hand corner of a 4 by 4 matrix, and use this larger matrix to apply the transformation to a point (or frame).

'

'

⃗

b 1

b 1

⃗

b 2

b 2

⃗

b 3

b 3

˜o

˜o

(

(

⎡ ⎢

⎢ ⎣

⎡ ⎢

⎢ ⎣

c 1 c 2

c 3 1

a e i 0

⎤

⎥ ⎥ ⎦

⇒

b f j 0

c g k 0

0 0 0 1

⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

c 1 c 2 c 3

1

⎤

⎥ ⎥ ⎦

This has the same effect on the c i as it did with linear transformations. If we think of the point ˜p as being offset from the origin ˜o by a vector ⃗v , we see that this has the same effect as applying the linear transform to the offset vector. So, for example, if the 3 by 3 matrix is a rotation matrix, this transformation will rotate the point about the origin (see Figure 3.1). As we will see below in Chapter 4, when applying a linear transformation to a point, the position of the frame’s origin plays an important role.

We use the following shorthand for describing a 4 by 4 matrix that just applies a linear transform.

L=

) l

0

0 1

*

where L is a 4 by 4 matrix, l is a 3 by 3 matrix, the upper right 0 is a 3 by 1 matrix of zeros, the lower left 0 is a 1 by 3 matrix of zeros, and the lower right 1 is a scalar.

3.4 Translations

It is very useful to be able to apply a translation transformation to points. Such transformations are not linear (See Exercise 6). The main new power of the afﬁne transformation over the linear transformations is its ability to express translations. In particular, if we apply the transformation

'

'

⃗

b 1

⃗

b 1

⃗

b 2

⃗

b 2

⃗

b 3

⃗

b 3

˜o

˜o

(

(

⎡ ⎢

⎢ ⎣

⎡ ⎢

⎢ ⎣

c 1 c 2

c 3 1

1 0 0 0

⎤

⎥ ⎥ ⎦

⇒

0 1 0 0

0 0 1 0

t x t y

t z 1

⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

c 1 c 2 c 3

1

⎤

⎥ ⎥ ⎦

we see that its effect on the coordinates is

c

c

c

1

2

3

⇒

⇒ ⇒

c 1 + t x

c 2 + t y

c 3 + t z

For a translation we use the shorthand ) i T = 0

t 1

*

where T is a 4 by 4 matrix, i is a 3 by 3 identity matrix, the upper right t is a 3 by 1 matrix representing the translation, the lower left 0 is a 1 by 3 matrix of zeros, and the lower right 1 is a scalar.

Note that if c has a zero in its fourth coordinate, and thus represents a vector instead of a point, then it is unaffected by translations.

3.5 Putting Them Together

Any afﬁne matrix can be factored into a linear part and a translational part.

⎡

⎢ ⎢ ⎣

a e i 0

b f j 0

c g k 0

d h l 1

⎤

⎡

⎥ ⎢ ⎥ ⎢ ⎦ = ⎣

1 0 0 0

0 1 0 0

0 0 1 0

d h l 1

⎤⎡

⎥⎢ ⎥⎢ ⎦⎣

a e h 0

b f i 0

c g j 0

0 0 0 1

⎤

⎥

or in shorthand

)

l 0

t 1

*

=

) i

0

t 1

*)

l 0

0 1

*

(3.1)

A = T L (3.2) Note that since matrix multiplication is not commutative, the order of the multiplication T L matters. An afﬁne matrix can also be factored as A = LT ′ with a different translation matrix T ′ , but we will not make use of this form.

If L, the linear part of A, is a rotation, we write this as

A = T R (3.3) In this case we call the A matrix a rigid body matrix, and its transform, a rigid body transform, or RBT. A rigid body transform preserves dot products between vectors, handedness of a basis, and distance between points.

3.6 Normals

In computer graphics, we often use the normals of surfaces to determine how a surface point should be shaded. So we need to understand how the normals of a surface transform when the surface points undergo an afﬁne transformation described by a matrix A.

One might guess that we could just multiply the normal’s coordinates by A. For example, if we rotated our geometry, the normals would rotate in exactly the same way. But using A is, in fact not always correct. For example in Figure 3.2, we squash a sphere along the y axis. In this case, the actual normals transform by stretching along the y axis instead of squashing. Here we derive the correct transformation that applies in all cases.

Let us deﬁne the normal to a smooth surface at a point to be a vector that is orthogonal to the tangent plane of the surface at that point. The tangent plane is a plane of vectors that are deﬁned by subtracting (inﬁnitesimally) nearby surface points, and so we have

⃗n · (˜p 1 − ˜p 2 ) = 0

for the normal ⃗n and two very close points ˜p 1 and ˜p 2 on a surface. In some ﬁxed orthonormal coordinate system, this can be expressed as ⎛⎡ ⎤ ⎡ ⎤ ⎞ x1 x0 - ⎜⎢ y1 ⎥ ⎢ y0 ⎥⎟ . nx ny nz ∗ ⎝ ⎜ ⎣ ⎢ ⎦ ⎥ − ⎣ ⎢ ⎦ ⎥ ⎠ ⎟ = 0 (3.4) z1 z0

1

1

We use a ’∗’ in the forth slot since it is multiplied by 0 and thus does not matter.

Suppose we transform all of our points by applying an afﬁne transformation using an afﬁne matrix A. What vector remains orthogonal to any tangent vector? Let us rewrite Equation (3.4) as

nx

ny

nz

⎛⎡

⎜⎢ ⎜⎢ ⎝⎣

x1 y1 z1 1

⎤

⎡

⎥ ⎢ ⎥ ⎢ ⎦ − ⎣

x0 y0 z0 1

⎤⎞

5-

. 6 ∗ A −1 (A

⎥⎟ ⎥⎟ ⎦⎠ ) = 0

If we deﬁne [x ′ , y ′ , z ′ , 1] t = A[x, y, z, 1] t to be the coordinates of a transformed point, and let [nx ′ , ny ′ , nz ′ , ∗] = [nx, ny, nz, 1] t A −1 , then we have ⎛⎡ ′ ⎤ ⎡ ′ ⎤⎞ x1 x0 - . ⎜⎢ ′ ⎥ ⎢ ′ ⎥⎟ ′ ′ ′ ⎜⎢ y1 ⎥ ⎢ y0 ⎥⎟

nx ny nz ∗ ⎝⎣ ′ ⎦ − ⎣ ′ ⎦⎠ = 0 z1 z0

1

1

and we see that [nx ′ , ny ′ , nz ′ ] t are the coordinates (up to scale) of the normal of the transformed geometry.

Note that since we don’t care about the ’∗’ value, we don’t care about the fourth column of A −1 . Meanwhile, because A is an afﬁne matrix, so is A −1 and thus the fourth row of the remaining three columns are all zeros and can safely be ignored. Thus, using the shorthand ) * l t A= 0 1

we see that

-

′

nx

ny

′

nz

′

.

=

-

nx

ny

nz

. −1 l

and transposing the whole expression, we get ⎡ ⎣

′ ′ ′

nx ny nz

⎤ ⎦

=l

−t

⎡ ⎣

nx ny nz

⎤ ⎦

where l −t is the inverse transpose (equiv. transposed inverse) 3 by 3 matrix. Note that if l is a rotation matrix, the matrix is orthonormal and thus its inverse transpose is in fact the same as l. In this case a normal’s coordinates behave just like a point’s coordinates. For other linear transforms though, the normals behave differently. (See Figure 3.2.) Also note that the translational part of A has no effect on the normals.

