# Note
这是对**MIT Foundation of 3D Computer Graphics**第7章的翻译，本章讲解了方位插值、四元数表达(quaternion representation)、slerp（球体线性插值）/lerp操作等基础知识，重点是如何使用四元数替换旋转矩阵。本书内容仍在不断的学习中，因此本文内容会不断的改进。若有任何建议，请不吝赐教ninetymiles@icloud.com 

> 注：文章中相关内容归原作者所有，翻译内容仅供学习参考。
> 另：Github项目[CGLearning](https://github.com/nintymiles/CGLearning)中拥有相关翻译的完整资料、内容整理、课程项目实现。

# 四元数（Quaternions）专业性多一点(a bit technical)
本章中，我们会探讨将旋转的四元数表达作为对旋转矩阵的代替
$$
R = \begin{bmatrix} r & 0 \\ 0 & 1\end{bmatrix}
$$
针对四元数，我们的主要用处是协助我们以自然的方式在方位间插值。对于驱动（动画）从空中飞过的物体十分有用。如果你不打算针对实际应用插值出旋转，那么这种表达不是必须的。

## 7.1 插值（Interpolation）
让我们假设我们拥有一个想要的物体帧（object frame），在时刻“time=0”：$\vec{\mathbf{o}}_0^t=\vec{\mathbf{w}}^tR_0$，同时还有一个想要的物体帧，在时刻“time=1”：$\vec{\mathbf{o}}_1^t=\vec{\mathbf{w}}^tR_1$，此处$R_0$和$R_1$都是$4 \times 4$的旋转矩阵。假设针对$\alpha \in [0..1]$，我们希望找到一系列帧$\vec{\mathbf{o}}_{\alpha}^t$，其自然地从$\vec{\mathbf{o}}_{0}^t$旋转到$\vec{\mathbf{o}}_{1}^t$。

一种思路是定义$R_{\alpha}=(1-\alpha)R_0 + (\alpha)R_1$，然后设置$\vec{\mathbf{o}}_{\alpha}^t=\vec{\mathbf{w}}^tR_{\alpha}$。问题是，根据矩阵线性插值，每个矢量只是沿着一条直线移动。参考图示$\text{Figure 7.1}$。这种情形中，中间的$R_{\alpha}$值甚至都不是旋转矩阵；这很明显是不可接受的。更进一步，（特别在3D空间中），非常不容易以某种方式找出一种真正的旋转矩阵近似这种被插值的矩阵，从而去除这种挤压。

另一种思路，不值得过于深入探究，是以某种方式将$R_0$和$R_1$都分解为3个所谓的欧拉角（Euler angles，参考小节2.5）。这3个标量值每个都可以借助$\alpha$被线性插值。被插值的欧拉角随后被用于生成中间的旋转。可以证实（turn out）当插值欧拉角时，被插值的旋转序列不管从物理方式还是几何方式都不是特别自然。例如，对于基的变化不是不变性（invariant）的（后面会定义）。

我们真正想做的是首先生成一个过渡矩阵$R_1R_0$。这种矩阵，可以为任何旋转矩阵，能够当作围绕某个轴$[k_x,k_y,k_z]^t$旋转某个角度$\theta$，就如方程式(2.5)所示。让我们想象我们已经拥有一种操作$(R_1R_0)^{\alpha}$，而这种操作给出我们一个围绕$[k_x,k_y,k_z]^t$进行$\alpha.\theta$角度的旋转。那么定义下面的操作就变得自然


$$
R_{\alpha} := (R_1R_0^{-1})^{\alpha}R_0 \tag{7.1}
$$

同时设置

$$\vec{\mathbf{o}}_{\alpha}^t = \vec{\mathbf{w}}^tR_{\alpha}$$

如此，我们会得到一系列帧，当我们增加$\alpha$值，这些帧就会围绕一个单轴旋转的程度越来越大，直到其到达想要的静止姿态。很清晰地，在开始处我们有$\vec{\mathbf{w}}^t(R_1R_0^{-1})^{0}R_0 = \vec{\mathbf{w}}^t R_0 =\vec{\mathbf{o}}_0^t$，而在结尾处我们有$\vec{\mathbf{w}}^t(R_1R_0^{-1})^{1}R_0 = \vec{\mathbf{w}}^t R_1 =\vec{\mathbf{o}}_1^t$。

这其中最难的部分为分解一个过渡矩阵，比如$R_1R_0$，为轴/角度形式。四元数表达背后的思路就是时时刻刻跟踪轴和角度以致于不再需要这种分解。在另一方面，借助四元数表达，我们还可以执行所有必要的可以用旋转矩阵来完成的操作（manipulation）。


### 7.1.1 循环（Cycles）
此处我们需要澄清一个细节。事实上，矩阵$R_1R_0^{-1}$可以被当作某一个$\theta+n2\pi$角度的旋转，$n$为任意整数。当观察这种旋转充当施加在一个矢量上的线性变换的效果时，这些额外的$2\pi$因子是不相关的。但是当定义一个幂操作符，这个操作符给了我们“一系列围绕一个单一轴旋转程度越来越大的帧，直到其到达要求的终止姿态”，我们就需要确定怎样选择一个所给定矩阵$R_1R_0^{-1}$的$n$值。对于插值，自然的选择为选择n使得$|\theta+n2\pi|$是最小的。实际上这意味着，对于n的选择，我们让$\theta + n2\pi \in [-\pi..\pi]$。另外，这种选择是没有歧义的（除了在$\theta = \pi + n2\pi $的情形，这种情形中我们会需要在$-\pi$和$\pi$之间任意选择一种）。确实，这种$n$值的选择在后面的小节7.4中会被提出。

### 7.1.2 Invariance

There are many natural things about the single-axis, constant angular velocity motion of Equation (7.1). For one thing, an object ﬂying through space with no forces acting on it has its center of mass follow a straight line and its orientation spins along a ﬁxed axis. Additionally, this kind of orientation interpolation satisﬁes both left and right invariance, which we will now explain.

⃗ ⃗ ⃗ Suppose we have an alternate world frame w ′t such that w t = w ′t R l with R l being some ﬁxed “left” rotation matrix. Using this frame, we can re-express our ⃗ original interpolation problem as one of interpolating between ⃗ o 0 := w ′t R l R 0 and ⃗ o 1 := w ′t R l R 1 . We say that our interpolation satisﬁes left invariance if this re⃗ expression does not change the interpolated answer: if the original interpolation gave ⃗ us ⃗ o α = w t R α , with some R α , then the re-expressed interpolation problem gives us w ⃗ ′t R l R α , which thus results in the exact same ⃗ o t . (See Figure 7.2.) In other words, an α interpolation scheme is left invariant if it depends only on the geometry of the frames ⃗ o 0 and ⃗ o 1 , not on the choice of world frame and the resulting R 0 and R 1 . Left invariance is a very natural property to desire; there are very few cases where it makes any sense for the interpolation to depend on the choice of world frame.

We can see that the interpolation using Equation (7.1) satisﬁes left invariance as follows. The “transition” afﬁne transform that that maps one frame, ⃗ o t , into another, 0 ⃗ o t , is always unique. In our case, since our frames are both right handed, orthonormal 1 and share an origin, this transform must be a rotation. Meanwhile, the power operator on rotations that we just deﬁned (namely, keep the axis but scale the angle) is an intrinsic geometric operation, and can be described without reference to any coordinates. Thus this interpolation scheme does not depend on the choice of world frame and is left invariant.

Right invariance, on the other hand, means that the interpolation of an object does change, even if we change the object frame being used. In particular, suppose we ﬁx a “right” rotation matrix R r , and use this to deﬁne new object frames for time=0 and t t time=1: o ′ 0 = o 0 t R r and o ′ 1 = o t R r . Since we don’t want our object’s pose itself 1

to change, we appropriately reassign the object-coordinates of all of its vertices using −1 c ′ = R r c. We say that our interpolation satisﬁes right invariance if this change of object basis has no impact on the interpolation of the rotating object itself. In other ⃗ words, if the original interpolation gave us ⃗ o α = w t R α , with some R α , then the new ⃗ interpolation gives us w t R α R r . Thus the object’s interpolation (using the new object coordinates c ′ ) are unchanged. (See Figure 7.3). Right invariance is a reasonably natural property. But we will see below, that when we also include translations along with our rotations, we may want to the interpolation to depend on the choice of the object frame’s origin.

We can directly see that the interpolation using Equation (7.1) satisﬁes right invariance as

$$
((R_1R_r)(R_r^{-1}R_0^{-1}))^{\alpha}R_0R_r = (R_1R_0^{-1})^{\alpha}R_0R_r=R^{\alpha}R_r
$$

## 7.2 The Representation

A quaternion is simply a four-tuple of real numbers, on which we will soon deﬁne suitable operations.

We write a quaternion as

$$ \large{
\begin{bmatrix} w \\ \hat{c} \end{bmatrix}
}$$

where w is a scalar and ˆc is a coordinate 3-vector. We have added the “hat” notation to ˆc to distinguish it from a coordinate 4-vector.

To represent a rotation of θ degrees about a unit length axis k, we use the quaternion 

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

The division by 2 looks a bit surprising but it makes the quaternion operations, described later, work out properly. Note that a rotation of −θ degrees about the axis −k gives us the same quaternion. A rotation of θ + 4π degrees about an axis k also gives us the same quaternion. So far so good. Oddly, a rotation of θ + 2π degrees about an axis k, which in fact is the same rotation, gives us the negated quaternion 

$$ \large{
\begin{bmatrix} -cos(\frac{\theta}{2}) \\ -sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

This oddity will complicate matters a bit later on when we deﬁne the power operator.

The quaternions

$$ \large{
\begin{bmatrix} 1 \\ \hat{0} \end{bmatrix},\begin{bmatrix} -1 \\ \hat{0} \end{bmatrix}
}$$

represent the identity rotation matrix.

The quaternions

$$ \large{
\begin{bmatrix} 0 \\ \hat{k} \end{bmatrix},\begin{bmatrix} 0 \\ -\hat{k} \end{bmatrix}
}$$

represent a 180 ◦ rotation about k.

Any quaternion of the form

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

has a norm (square root of sum of squares of the four entries) of 1. Conversely, any such unit norm quaternion can be interpreted (along with its negation) as a unique rotation matrix.

## 7.3 Operations

Multiplication of a (not necessarily unit norm) quaternion by a scalar is deﬁned as 

$$ \large{
\alpha \begin{bmatrix} w \\ \hat{c} \end{bmatrix} = \begin{bmatrix} \alpha w \\ \alpha \hat{c} \end{bmatrix}
}$$

Multiplication between two (not necessarily unit norm) quaternions is deﬁned using the following strange looking operation

$$ \large{
 \begin{bmatrix} w_1 \\ \hat{c}_1 \end{bmatrix} \begin{bmatrix} w_2 \\ \hat{c}_2 \end{bmatrix} = \begin{bmatrix} w_1 w_2 - \hat{c}_1.\hat{c}_2\\ w_1\hat{c}_2 + w_2\hat{c}_1 + \hat{c}_1 \times \hat{c}_2 \end{bmatrix} \tag{7.2}
}$$

where · and × are the dot and cross product on 3 dimensional coordinate vectors. This strange multiplication possesses the following useful property: if [w i ,ˆc i ] t represents the rotation matrix R i , then the product [w 1 ,ˆc 1 ] t [w 2 ,ˆc 2 ] t represents the rotation matrix R 1 R 2 . This can be veriﬁed through a series of not particularly intuitive calculations.

The multiplicative inverse of a unit norm quaternion is

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{-1} = \begin{bmatrix} cos(\frac{\theta}{2}) \\ -sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

This quaternion simply rotates by −θ around the same axis. (Inverses can also be deﬁned for non-unit norm quaternions, but we will not need this).

Importantly, we can use quaternion multiplication in order to apply a rotation to a coordinate vector. Suppose we have the 4-coordinate vector c = [ˆc, 1] t , and we left multiply it by a 4 by 4 rotation matrix R, to get

$$\mathbf{c}' = R \mathbf{c}$$

where the resulting 4-coordinate vector is of the form c ′ = [ˆc ′ , 1] t . To do this with quaternions, let R be represented with the unit norm quaternion 

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

Let us take the 3-coordinate vector ˆc and use it to create the non unit norm quaternion 

$$ \large{
\begin{bmatrix} 0 \\ \hat{c} \end{bmatrix}
}$$

Next we perform the following triple quaternion multiplication:

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix} 
\begin{bmatrix} 0 \\ \hat{c} \end{bmatrix}
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{-1} \tag{7.3}
}$$

It can again be veriﬁed through a series of not particularly intuitive calculations that the result of this triple quaternion product is in fact a quaternion of the form ) * 0 ˆc ′

where ˆc ′ is the 3-coordinate vector of the desired answer.

Thus quaternions on the one hand explicitly encode the rotation axis and angle, while on the other hand possess operations that allow us to easily manipulate them like rotations.

7.4 Power

Given a unit norm quaternion representing a rotation, we can raise it to the power α as follows. We ﬁrst extract the unit axis k by normalizing the three last entries of the quaternion. Next, we extract θ using the atan2 function. This gives us a unique value θ/2 ∈ [−π..π], and thus a unique θ ∈ [−2π..2π]. Then we deﬁne

$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{\alpha} = \begin{bmatrix} cos(\frac{\alpha\theta}{2}) \\ sin(\frac{\alpha\theta}{2})\hat{k} \end{bmatrix}
}$$

As α goes from from 0 to 1, we get a series of rotations with angles going between 0 and θ.

If cos( θ ) > 0, we get θ/2 ∈ [−π/2..π/2], and thus θ ∈ [−π..π]. In this case, when 2 we use α ∈ [0..1] to interpolate between two orientations, we will be interpolating the θ “short way” between the orientations. Conversely, if cos( 2 ) < 0, then |θ| ∈ [π..2π], and we will be interpolating the “long way” (greater than 180 degrees). In general, it is more natural to interpolate the short way between two orientations, and so when given a quaternion with negative ﬁrst coordinate, we always negate the quaternion before applying the power operation.

### 7.4.1 Slerp and Lerp

Putting all this together, if we want to interpolate between two frames that are related to the world frame through the rotation matrices R 0 and R 1 , and if these two matrices correspond to the two quaternions 

$$ \large{
\begin{bmatrix} cos(\frac{\theta_0}{2}) \\ sin(\frac{\theta_0}{2})\hat{k}_0 \end{bmatrix} , \begin{bmatrix} cos(\frac{\theta_1}{2}) \\ sin(\frac{\theta_1}{2})\hat{k}_1 \end{bmatrix}
}$$

then we simply need to compute the quaternion: 

$$ \large{
\left(\begin{bmatrix} cos(\frac{\theta_1}{2}) \\ sin(\frac{\theta_1}{2})\hat{k}_1 \end{bmatrix} \begin{bmatrix} cos(\frac{\theta_0}{2}) \\ sin(\frac{\theta_0}{2})\hat{k}_0 \end{bmatrix}^{-1}  \right)^{\alpha}
\begin{bmatrix} cos(\frac{\theta_0}{2}) \\ sin(\frac{\theta_0}{2})\hat{k}_0 \end{bmatrix} \tag{7.4}
}$$

This interpolation operation is often called spherical linear interpolation or just slerping for the following reason. Unit norm quaternions are simply 4-tuples of real numbers with sum-of-squares equal to one, thus we can think of these geometrically as points on the unit sphere in R 4 . It can be shown (see below), that if you start with two unit norm quaternions and then interpolate them using Equation (7.4), the resulting path in R 4 , in fact, corresponds exactly to a great arc connecting these two points on the unit sphere. Moreover, the interpolation proceeds along this path with arc length proportional to α.

In any dimension n, a trigonometric argument can be used to show that spherical linear interpolation between any two unit vectors, ⃗v 0 and ⃗v 1 , can be calculated as

$$ \large{
\frac{sin((1-\alpha)\Omega)}{sin\Omega}\vec{v}_0 + \frac{sin(\alpha\Omega)}{sin\Omega}\vec{v}_1 \tag{7.5}
}$$

where Ω is the angle between the vectors in R n . Thus, applying this to our two unit quaternions in R 4 , we see that we can replace Equation (7.4) with the equivalent interpolant

$$ \large{
\frac{sin((1-\alpha)\Omega)}{sin\Omega}\begin{bmatrix} cos(\frac{\theta_0}{2}) \\ sin(\frac{\theta_0}{2})\hat{k}_0 \end{bmatrix}  + \frac{sin(\alpha\Omega)}{sin\Omega} \begin{bmatrix} cos(\frac{\theta_1}{2}) \\ sin(\frac{\theta_1}{2})\hat{k}_1 \end{bmatrix} \tag{7.6}
}$$


where Ω is the angle between the initial and ﬁnal quaternions in R 4 . Below we sketch the steps needed to verify the equivalence of Equations (7.4) and (7.6). Note that as with Equation (7.4), in order to select “the short interpolation” of less than 180 degrees, we must negate (any) one of the two quaternions if the 4-dimensional dot product between the two quaternions is negative.

From this point of view, we see that we can approximate Equation (7.4), and thus Equation (7.6), with the simpler linear interpolation in R 4 . That is, we can simply compute

$$ \large{
(1-\alpha)\begin{bmatrix} cos(\frac{\theta_0}{2}) \\ sin(\frac{\theta_0}{2})\hat{k}_0 \end{bmatrix}  + (\alpha)\begin{bmatrix} cos(\frac{\theta_1}{2}) \\ sin(\frac{\theta_1}{2})\hat{k}_1 \end{bmatrix} 
}$$

Since this interpolant is no longer be a unit norm quaternion, we must then normalize the result, but this is easy to do. Importantly, this interpolation process, which is called lerping traces out the same path of quaternions that the more complicated slerp does (rotating through a single ﬁxed axis), though its rotation angle no longer moves evenly with α. (See Figure 7.4).

The lerp operation is both left and right invariant. For example, left invariance follows from 

$$ \large{
(1-\alpha)\begin{bmatrix} w_l \\ \hat{c}_l \end{bmatrix}\begin{bmatrix} w_0 \\ \hat{c}_0 \end{bmatrix} + (\alpha)\begin{bmatrix} w_l \\ \hat{c}_l \end{bmatrix}\begin{bmatrix} w_1 \\ \hat{c}_1 \end{bmatrix} = \begin{bmatrix} w_l \\ \hat{c}_l \end{bmatrix}\left( (1-\alpha)\begin{bmatrix} w_0 \\ \hat{c}_0 \end{bmatrix} + (\alpha)\begin{bmatrix} w_1 \\ \hat{c}_1 \end{bmatrix} \right)
}$$

as scalar multiply commutes across quaternion multiplication, and quaternion multiplication distributes over sums. Similarly, we can directly see from its form, that Equation (7.6) is also both left and right invariant. The only tricky part is going through the calculations that show that the angle, Ω, is left and right invariant.

Lerp can be implemented more efﬁciently than slerp. More importantly, it generalizes quite easily to the case of blending between n different rotations. We can simply blend the quaternions in R 4 , and then normalize the result. Such n-way blends can be useful when constructing splines of rotations (see Section 9.3), and when performing skinning for animation (see Section 23.1.2). There are also methods that perform n-way blending intrinsically on the sphere, though such methods do not come with a constant time evaluation method [10].

Equivalence of Power and Sphere Based Slerps (optional)

Here we sketch the steps needed to establish the equivalence between the power-based and sphere-based interpolations on rotations.

• It can be argued (as we just did above) that Equation (7.6) is left invariant. Note that we already established left invariance earlier for Equation (7.4). Due to the left invariance of both interpolation methods, we can, without loss of generality, just consider the case where R 0 is the identity.

• Assuming R 0 is the identity, the power based interpolant of Equation (7.4) gives us (R 1 ) α , which is

$$ \large{
\begin{bmatrix} cos(\frac{\alpha\theta_1}{2}) \\ sin(\frac{\alpha\theta_1}{2})\hat{k} \end{bmatrix}
}$$

• Since R 0 is the identity, the initial quaternion is [1, 0] t . Plugging this into Equation (7.6), we can verify that this also agrees with Equation (7.7).

• A trigonometric argument can be used to show that Equation (7.6) corresponds geometrically to interpolation along the surface of a sphere.

## 7.5 Code

A quaternion class can be encode very simply.

We deﬁne Quat as a four-tuple of reals. We then deﬁne the multiplication (q1 * q2) as in Equation (7.2). Given a unit norm quaternion, Quat q, we deﬁne inv(q). Given a unit norm quaternion, Quat q, and a Cvec4 c, we deﬁne (q * c), which applies the rotation to the coordinate vector c, as in Equation (7.3), and returns the coordinate vector c ′ .

We also write code to implement the MakeRotation functions as we had for matrices.

Given a unit norm quaternion q and a real number alpha, we deﬁne the power operator: pow(q,alpha). Given two quaternions q0 and q1, we can deﬁne the interpolating quaternion: slerp(q0,q1,alpha). Remember that, when implementing slerp, we need to negate the quaternion (q1 * inv(q0)) before calling pow if its ﬁrst coordinate is negative in order to interpolate the “short way”.

## 7.6 Putting Back the Translations

So far, we have discussed how quaternions are useful for representing rotations, but have ignored translations. Now we will discuss how to use quaternions in concert with translation vectors to represent rigid body transformations.

A rigid body transformation, or RBT, can be described by composing a translation and a rotation.

$$ \begin{array}{cl}
A & = & TR \\
\begin{bmatrix} r & t \\ 0 & 1 \end{bmatrix} & = & \begin{bmatrix} i & t \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r & 0 \\ 0 & 1 \end{bmatrix}
\end{array}$$

Thus, we can represent this as an object: 

```cpp
class RigTform{ 
	Cvec4 t; 
	Quat r; 
};
```

Remember that, since t represents a translation vector, its fourth coordinate is 0.

### 7.6.1 Interpolation

Given two RBTs O 0 = (O 0 ) T (O 0 ) R and O 1 = (O 1 ) T (O 1 ) R , we can interpolate between them by: ﬁrst linearly interpolating the two translation vectors to obtain the translation T α , then slerping between the rotation quaternions to obtain the rotation ⃗ R α , and ﬁnally setting the interpolated RBT O α to be T α R α . If ⃗ o 0 t = w t O 0 and ⃗ o 1 t = w ⃗ t O 1 , we can then set ⃗ o α t = w ⃗ t O α . Under this interpolation, the origin of ⃗ t o travels in a straight line with constant velocity, and the vector basis of ⃗ o t rotates with constant angular velocity about a ﬁxed axis. As we already said, this is very natural, since an object ﬂying through space with no forces acting on it has its center of mass follow a straight line, and its orientation spins along a ﬁxed axis. Additionally, this unique geometric interpolant between ⃗ t and ⃗ t can be expressed with reference to o 0 o 1 any speciﬁc coordinates. Therefore, it does not depend on the choice of world frame and must be left invariant.

It is important to note that this RBT interpolant is not right invariant. If you change the object frames and interpolate between them using this method, the new origin will travel in a straight line, while the old origin will trace out a curved path. (See Fig-ure 7.5.) Thus, this method makes the most sense where there is a meaningful notion of “center” for the object being interpolated. In cases where there is no such center, the most natural answer is less obvious (but see for example [35]).

### 7.6.2 操作（Operations）
返回到小节6.2中我们的绘制代码中，我们现在可以借助`RigTform`数据类型而不是`Matrix4`类型来表达`eyeRBT`和`objRBT`。

要生成有效的坚固形体（刚体）变换，我们需要下列的操作

```
RigTForm identity(); 
RigTForm makeXRotation(const double ang); 
RigTForm makeYRotation(const double ang); 
RigTForm makeZRotation(const double ang); 
RigTForm makeTranslation(const double tx, const double ty, const double tz);
```

我们需要编码`RigTForm A`和`Cvec4 c`的乘积，它会返回结果`A.r * c + A.t`。

接着，我们需要编码两个`RigTForm`的乘积。要理解如何完成这个计算，让我们观察两个这样的刚体变换的乘积。

$$ \begin{array}{rl}
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & =  \\
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & r_1t_2 \\ 0 & 1 \end{bmatrix}  \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & r_1t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & t_1+r_1t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1r_2 & 0 \\ 0 & 1 \end{bmatrix}
\end{array}$$

从这种表达我们看到结果为一个新的刚体变换拥有平移$t_1 + r_1t_2$和旋转$r_1r_2$。

接着，我们需要为这种数据类型编码倒数（inverse）操作符。如果我们观察一个刚体变换的倒数，我们会看到

$$ \begin{array}{rl}
\left(\begin{bmatrix} i & t \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r & 0 \\ 0 & 1 \end{bmatrix}\right)^{-1}  & =  \\
\begin{bmatrix} r & 0 \\ 0 & 1 \end{bmatrix}^{-1}\begin{bmatrix} i & t \\ 0 & 1 \end{bmatrix}^{-1}  & = \\
\begin{bmatrix} r^{-1} & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & -t \\ 0 & 1 \end{bmatrix}  & = \\
\begin{bmatrix} r^{-1} & -r^{-1}t \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & -r^{-1}t \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r^{-1} & 0 \\ 0 & 1 \end{bmatrix}
\end{array}$$

因此，我们看到结果为一个新的刚体变换，拥有平移$-r^{-1}t$和旋转$r^{-1}$。

给出这种基础结构，我们能够借助我们的新数据类型重新编码函数`doQtoOwrtA(RigTForm Q, RigTform 0, RigTForm A)`。

Finally, in order to communicate with the vertex shader using 4 by 4 matrices, we need a procedure Matrix4 makeRotation(quat q) which implements Equation (2.5). Then, the matrix for a rigid body transform can be computed as


```cpp
matrix4 makeTRmatrix(const RigTform& rbt){ 
	matrix4 T = makeTranslation(rbt.t); 
	matrix4 R = makeRotation(rbt.r); 
	return T * R; 
}
```

Thus, our drawing code starts with

```cpp
Matrix4 MVM = makeTRmatrix(inv(eyeRbt) * objRbt); 
\\ can right multiply scales here 
Matrix4 NMVM = normalMatrix(MVM); 
sendModelViewNormalMatrix(MVM,NMVM);
```

Note that, the way we have structured our computation, we will not need any code that takes a Matrix4 and convertes it to a Quat.

Other than switching from the Matrix4 data type to the RigTForm type, the rest of our code, which keeps track of the various rigid body frames, does not need to be altered. In this new implementation, a scale is still represented by a Matrix4.

