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

### 7.1.2 不变性（Invariance）
相关于方程式（7.1）的围绕单轴，常量角速率方式的运动，存在很多自然的事物。举一个例子，一个飞过空中的物体在没有外力施加于其上时会让其质心沿着直线运动，同时其方位围绕着一个固定轴旋转。另外，这种方位插值满足左侧和右侧不变性，这一点我们现在就来解释。

假设我们有一个可替换的世界帧$\vec{\mathbf{w}}'^t$使得$\vec{\mathbf{w}}^t=\vec{\mathbf{w}}'^tR_l$，其中$R_l$为某种固定的“左侧”旋转矩阵。借助这种帧，我们可以再表达我们最初的插值问题为在$\vec{\mathbf{o}}_0^t=\vec{\mathbf{w}}'^tR_lR_0$和$\vec{\mathbf{o}}_1^t=\vec{\mathbf{w}}'^tR_lR_1$之间进行插值的一种行为。如果这种再表达不改变插值结果，我们说插值满足“左侧”不变性：如果最初的插值给了我们$\vec{\mathbf{o}}^t_{\alpha}=\vec{\mathbf{w}}^tR_{\alpha}$，对于某种$R_{\alpha}$，那么再表达的插值给出了我们$\vec{\mathbf{w}}'^tR_lR_{\alpha}$，这就导致了完全相同的$\vec{\mathbf{o}}^t$。（参考图示$\text{Figure 7.2}$。）换句话说，只要一个插值方案仅仅依赖帧$\vec{\mathbf{o}}^t_0$和$\vec{\mathbf{o}}^t_1$的几何特征，而不是世界帧（world frame）和最终的$R_0$和$R_1$的选择，就可以说它是左侧不变的。左侧不变性是一种想要的非常自然的属性；存在非常少的情形，其中对于插值依赖世界帧的选择会有意义。

我们可以看到使用方程(7.1)的插值满足左侧不变性如后面所述。“过渡”仿射变换映射一个帧$\vec{\mathbf{o}}^t_0$为$\vec{\mathbf{o}}^t_1$，总是唯一的。在我们的情形中，因为我们的帧都是右手性的，正交标准性的，并且共享原点，这种变换一定是一种旋转。同时，我们刚刚定义的旋转的幂操作符（也就是说，保持轴不变但伸缩角度）是一种内生的几何操作，能够不参考任何坐标被描述。因而这种插值方案不依赖于世界帧的选择，那么是左侧不变的。

右侧不变性，在另一方面，意味着一个物体的插值确实改变了，即便是我们改变了被使用的物体帧。实际上，假如我们固定一个“右侧”旋转矩阵$R_r$，同时使用其定义新的物体帧，在时刻“time=0”和“time=1”分别为：$\vec{\mathbf{o}}'^t_0=\vec{\mathbf{o}}^t_0R_r$和$\vec{\mathbf{o}}'^t_1=\vec{\mathbf{o}}^t_1R_r$。因为我们不想让物体的姿态本身产生改变，我们用$\mathbf{c}'=R^{-1}_r\mathbf{c}$恰当地重新赋值其所有顶点的物体-坐标（object-coordinates）。我们说我们的插值满足右侧不变性，只要这种物体基（object basis）的改变不影响旋转物体本身的插值。换句话说，如果最初的插值给出了我们$\vec{\mathbf{o}}_{\alpha}^t=\vec{\mathbf{w}}^tR_{\alpha}$，对于某个$R_{\alpha}$，那么新的插值给了我们$\vec{\mathbf{w}}^tR_{\alpha}R_r$。因而物体的插值（借助新物体坐标$\mathbf{c}'$）是不变的。（参考图示$\text{Figure 7.3}$）。右侧不变性是一种合理的自然属性。但是我们下面会看到，当我们同时和旋转一起包含平移时，我们可能想要插值依赖物体帧原点的旋转。

我们能够直接看出使用方程（7.1）的插值满足右侧不变性如下
$$
((R_1R_r)(R_r^{-1}R_0^{-1}))^{\alpha}R_0R_r = (R_1R_0^{-1})^{\alpha}R_0R_r=R^{\alpha}R_r
$$

## 7.2 表达（The Representation）
一个四元数（quaternion）只是实数的四元组，我们会很快在其上定义合适的操作。

我们把一个四元数写作
$$ \large{
\begin{bmatrix} w \\ \hat{c} \end{bmatrix}
}$$
此处$w$是一个标量而$\hat{c}$是一个3部件坐标矢量（coordinate 3-vector）。我们已经把“帽子”标记添加到$\hat{c}$以和一个四部件坐标矢量进行区分。

要表达围绕一个单位长度（unit length）轴$\vec{k}$进行$\theta$角度的旋转，我们使用如下四元数
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

除以2的除法看起来有一点儿让人吃惊，但是正是它让四元数操作可以正确工作，后面会讲述。**注意围绕$-\vec{k}$轴$-\theta$角度的旋转为我们给出了相同的四元数。**一个围绕轴$\vec{k}$轴$\theta+4\pi$角度的旋转同样也给出了相同的四元数。目前为止很好。奇怪的是，一个围绕轴$\vec{k}$轴$\theta+2\pi$角度的旋转给出了负的四元数，而其实际上是完全相同的四元数

$$ \large{
\begin{bmatrix} -cos(\frac{\theta}{2}) \\ -sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$
在我们随后定义幂操作符时，这种怪异将会让事情更复杂一点。

四元数
$$ \large{
\begin{bmatrix} 1 \\ \hat{0} \end{bmatrix},\begin{bmatrix} -1 \\ \hat{0} \end{bmatrix}
}$$
表达了同一旋转矩阵（identity rotation matrix）。

四元数
$$ \large{
\begin{bmatrix} 0 \\ \hat{k} \end{bmatrix},\begin{bmatrix} 0 \\ -\hat{k} \end{bmatrix}
}$$
表达了围绕轴$\vec{k}$的180度的旋转。

任何这种形式的四元数
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$
都拥有值为1（四个条目的平方和的平方根）的态(norm)。反过来说，任何这样的单位态的四元数（**和其负值一起**）都可以被解读为一个唯一的旋转矩阵。

## 7.3 各种操作（Operations）
一个四元数（不必然是单位态(unit norm)）乘以一个标量的乘法被定义如下
$$ \large{
\alpha \begin{bmatrix} w \\ \hat{c} \end{bmatrix} = \begin{bmatrix} \alpha w \\ \alpha \hat{c} \end{bmatrix}
}$$

两个四元数（不必然是单位态）间的乘法借助下列有着奇怪外观的操作被定义 
$$ \large{
 \begin{bmatrix} w_1 \\ \hat{c}_1 \end{bmatrix} \begin{bmatrix} w_2 \\ \hat{c}_2 \end{bmatrix} = \begin{bmatrix} w_1 w_2 - \hat{c}_1.\hat{c}_2\\ w_1\hat{c}_2 + w_2\hat{c}_1 + \hat{c}_1 \times \hat{c}_2 \end{bmatrix} \tag{7.2}
}$$
此处$.$和$\times$为3维坐标矢量上的点积和向量积。这种外观奇怪的乘法拥有下列有用的属性：如果$[w_i,\hat{c}_i]^t$代表旋转矩阵$R_i$，那么乘积$[w_1,\hat{c}_1]^t[w_2,\hat{c}_2]^t$就代表旋转矩阵$R_1R_2$。这个表达可以通过一系列不是特别直观的计算被验证。

一个单位态四元数的乘法反转（倒数）为
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{-1} = \begin{bmatrix} cos(\frac{\theta}{2}) \\ -sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$
这个四元数只是围绕相同轴旋转了$-\theta$角度。（倒数也可以针对非单位态四元数定义，但是我们这里不需要）。

更重要的是，我们能够借助四元数操作针对一个坐标实例应用旋转操作。假设我们拥有4部件坐标矢量$\mathbf{c} = [\hat{c},1]^t$，同时我们使用一个$4\times4$旋转矩阵$R$左乘这个坐标矢量来获得
$$\mathbf{c}' = R \mathbf{c}$$
这里最终的4部件坐标矢量为这种形式$\mathbf{c}' = [\hat{c}',1]^t$。要使用四元数完成这个操作，我们让$R$借助单位态四元数被表达 
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}
}$$

让我们拿3部件坐标矢量$\hat{c}$并且使用它生成一个非单位态四元数
$$ \large{
\begin{bmatrix} 0 \\ \hat{c} \end{bmatrix}
}$$
接着我们执行下面的三重四元数乘法：
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix} 
\begin{bmatrix} 0 \\ \hat{c} \end{bmatrix}
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{-1} \tag{7.3}
}$$
这个公式可以通过一系列不是特别直观的计算验证出这种三重四元数的乘积事实上为下面形式的四元数
$$ \large{
\begin{bmatrix} 0 \\ \hat{c}' \end{bmatrix}
}$$
此处$\hat{c}'$为要求结果的3部件坐标矢量表达。

因此，四元数一方面明确地封装了旋转轴和角度，而另一方面又具备允许我们像旋转矩阵一样操纵他们的各种操作。

7.4 幂操作定义（Power）
给出一个表达一个旋转的单位态四元数，我们能够将其提升为幂$\alpha$操作如后所述。我们首先通过标准化四元数的后3个条目提取出单位轴$\vec{k}$。接着，我们使用$atan2$函数提取出$\theta$。这样就给出了我们一个唯一的值$\theta/2 \in [-\pi..\pi]$，从而确定了一个唯一的角度$\theta \in [-2\pi..2\pi]$。那么我们定义如下
$$ \large{
\begin{bmatrix} cos(\frac{\theta}{2}) \\ sin(\frac{\theta}{2})\hat{k} \end{bmatrix}^{\alpha} = \begin{bmatrix} cos(\frac{\alpha\theta}{2}) \\ sin(\frac{\alpha\theta}{2})\hat{k} \end{bmatrix}
}$$
当$\alpha$从0变换到1，我们获得一系列角度在0和$\theta$之间变化的旋转。

如果$\cos(\theta) > 0$，我们得到$\theta/2 \in [-\pi/2..\pi/2]$，从而$\theta \in [-\pi..\pi]$。在这种情形中，当我们使用$\alpha \in [0..1]$去在两个方位间插值，我们会在方位间的“短路径”上插值$\theta$。反过来说，如果
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

### 7.6.2 Operations

Going back to our drawing code of Section 6.2, we can now represent the eyeRBT and objRBT RBTs using the RigTform data type instead of Matrix4.

To create useful rigid body transforms, we need the following operations

RigTForm identity(); RigTForm makeXRotation(const double ang); RigTForm makeYRotation(const double ang); RigTForm makeZRotation(const double ang); RigTForm makeTranslation(const double tx, const double ty, const double tz);

We need to code the product of a RigTForm A and a Cvec4 c, which returns A.r * c + A.t.

Next, we need to code the product of two RigTForm. To understand how to do this, let us look at the product of two such rigid body transforms.

$$ \begin{array}{rl}
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & =  \\
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & r_1t_2 \\ 0 & 1 \end{bmatrix}  \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & t_1 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & r_1t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1 & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_2 & 0 \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & t_1+r_1t_2 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r_1r_2 & 0 \\ 0 & 1 \end{bmatrix}
\end{array}$$

From this we see that the result is a new rigid transform with translation t 1 + r 1 t 2 and rotation r 1 r 2 .

Next, we need to code the inverse operator for this data type. If we look at the inverse of a rigid body transform, we see that

$$ \begin{array}{rl}
\left(\begin{bmatrix} i & t \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r & 0 \\ 0 & 1 \end{bmatrix}\right)^{-1}  & =  \\
\begin{bmatrix} r & 0 \\ 0 & 1 \end{bmatrix}^{-1}\begin{bmatrix} i & t \\ 0 & 1 \end{bmatrix}^{-1}  & = \\
\begin{bmatrix} r^{-1} & 0 \\ 0 & 1 \end{bmatrix} \begin{bmatrix} i & -t \\ 0 & 1 \end{bmatrix}  & = \\
\begin{bmatrix} r^{-1} & -r^{-1}t \\ 0 & 1 \end{bmatrix} & = \\
\begin{bmatrix} i & -r^{-1}t \\ 0 & 1 \end{bmatrix} \begin{bmatrix} r^{-1} & 0 \\ 0 & 1 \end{bmatrix}
\end{array}$$

Thus, we see that the result is a new rigid body transform with translation −r −1 t and rotation r −1 .

Given this infrastructure, we can now recode the function doQtoOwrtA(RigTForm Q, RigTform 0, RigTForm A) using our new data type.

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

