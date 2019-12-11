# 图形中帧（坐标系）的应用（Frames in Graphics）
目前为止，已经讲解了点和矩阵变换的基础，我们将能够在计算机图形中描述它们通常如何被使用。随后我们会讨论多种建模运用（modeling manipulation）和成像操作（imaging operation）。

## 5.1 世界、物体和眼睛帧（World, Object and Eye Frames）
当描述一个场景的几何形状时，我们会开始于一个被称为world frame（世界帧）的右手性的正交标准帧（right handed orthonormal frame),$\vec{w}^t$。world frame绝对不会被我们改动。然后其它帧能被关联于这个world frame所描述。如果我们借助关联于world frame的坐标表达某个点的地址，那么这些坐标将被称作world coordinates（世界坐标）。

假设我们希望建模一个正在场景中运动的汽车。我们会想借助“特别”的顶点坐标来建模物体的几何形状，这样的顶点不需要我们时刻意识到场景中物体坐标的全局替换。同样地，我们会想在场景中来回移动汽车而不用改变这些坐标。这种行为可以借助物体帧（object frame）完成。

针对场景中的每个物体，我们为其关联一个右手性的正交标准物体帧（right handed orthonormal object frame）$\vec{o}^t$。现在我们可以借助关联于物体坐标系（object‘s coordinate system）的坐标表示物体各个部分的地址。这些坐标被称为object coordinates（物体坐标）并且会被存储在计算机程序中。要移动整个物体，我们只要更新$\vec{o}^t$即可，不需要更改物体上对应点的任何object coordinates（物体坐标）。

物体坐标系和世界帧（world frame）之间的关系通过并行的（affine）$4 \times 4$矩阵O表达。也即是

$$\Large{ \vec{o}^t = \vec{w}^tO }$$

在我们的计算机程序中，我们会存储矩阵O，借助上面的公式，这个矩阵可被理解为将world frame（世界帧）关联到object‘s coordinate system（物体坐标系）。要移动$\vec{o}^t$帧，我们改变矩阵O。

在真实世界中，当我们想要生成一张3D环境的2D图像，我们会在场景中某处放置一台相机。图片中每个对象的位置基于到相机的3D关系被确定，也就是说，其坐标关联于一个合适的坐标系（basis）。在计算机图形中我们借助一个右手性的正交标准帧(right handed orthonormal frame）$\vec{e}^t$，被称为eye frame（眼睛帧）。我们将眼睛解读为正看向这个帧的负z轴同时拍摄相片（参考图示$\text{Figure 5.1}$）。eye frame通过某种（坚固形体rigid body）$4 \times 4$矩阵E和world frame关联：

$$\Large{ \vec{\mathbf{e}}^t = \vec{\mathbf{w}}^tE }$$

在计算机程序中，我们会明确地存储矩阵E。

假定一个点

$$\Large{ \tilde{p} = \vec{O}^t\mathbf{c} = \vec{w}^tO\mathbf{c} =  \vec{w}^tE^{-1}O\mathbf{c} }$$

我们将$\mathbf{c}$称为object coordinates（物体坐标），将$O\mathbf{c}$称为world coordinates（世界坐标），将$E^{-1}O\mathbf{c}$称为eye coordinates（眼睛坐标）。我们借用下标“$_o$”用于object coordinates，“$_w$”下标用于world coordinates，“$_e$”下标用于eye coordinates。如此我们可以表达如下关系：

$$ \Large{ \begin{bmatrix} x_e \\ y_e \\ z_e \\ 1 \end{bmatrix}
= E^{-1}O
\begin{bmatrix} x_o \\ y_o \\ z_o \\ 1 \end{bmatrix} } $$

最终，就是这些eye coordinates（眼睛坐标）指定了其中每个顶点（vertex）在被渲染图像中所出现的位置。因此，正如在第6章中所描述的，我们的渲染处理将需要计算每个顶点的eye coordinates。

## 5.2 任意移动相关物体（Moving Things Around）
在一个交互式的3D程序中，我们经常想要借助某种坚固形体变换（rigid body transformation）在空中来回移动物体和眼睛（视野方位）。现在我们讨论这种方式如何被实现。

### 5.2.1 移动一个物体（Moving an Object）
我们通过恰当更新其帧（frame）的方式移动一个物体，这可以通过更新其矩阵O的方式被表示。

让我们讲，我们希望关联于某种帧（frame）$\vec{\mathbf{\alpha}}^t = \vec{\mathbf{w}}^tA$对一个object frame（物体帧）$\vec{\mathbf{o}}^t$应用变换M，就如在方程（4.1）中所示，我们有如下推导：

$$ \Large{
\quad\;\; \vec{\mathbf{o}}^t \qquad\qquad\qquad\qquad\;\;\;\;\; (5.1) \\
= \vec{\mathbf{w}}^tO \qquad\qquad\qquad\qquad(5.2) \\
= \vec{\mathbf{\alpha}}^tA^{-1}O \qquad\qquad\qquad (5.3) \\
\Rightarrow \vec{\mathbf{\alpha}}^tMA^{-1}O \qquad\qquad (5.4) \\
= \vec{\mathbf{w}}^tAMA^{-1}O \qquad\quad (5.5) 
}$$

所以在代码中我们将变换实现为$O \leftarrow AMA^{-1}O$。

什么是$\vec{\alpha}^t$的自然选择？最明显的选择会是关联于$\vec{\mathbf{o}}^t$自身应用变换到$\vec{\mathbf{o}}^t$上。不幸的是，这意味着被使用的轴会是相对于物体本身的那些轴。“向右”会在物体的右侧方向，这不会在被观察的图像中对应任何实际的方向。我们可能尝试通过关联于$\vec{\mathbf{e}}^t $变换$\vec{\mathbf{o}}^t$。这会修复轴相关问题但是确生成了另一个问题。当我们旋转物体，它会围绕eye frame（眼睛帧）的原点做环绕运动。但是我们通常发现围绕物体自己的中心旋转才更自然，这个中心我们认为是$\vec{\mathbf{o}}^t$的原点。参考图示$\text{Figure 5.2}$。

要修复这两种问题，我们可以生成一个新的帧（frame），其拥有物体（object）帧的原点，还有眼睛（eye）帧的轴。要获得这个帧，让我们将我们已有的矩阵分解如下：
$$\Large{
O = (O)_T(O)_R \\
E = (E)_T(E)_R
}$$

这里$(A)_T$代表矩阵A的平移因子，$(A)_R$代表矩阵A的旋转因子，就如在方程（3.3）中一样。我们随后可以看到想要的辅助帧（auxiliary frame）应该如下：
$$\Large{
 \vec{\mathbf{\alpha}}^t = \vec{\mathbf{w}}^t(O)_T(E)_R   \qquad (5.6)
}$$

这个帧（frame）通过始于世界坐标系（world coordinate system），然后平移其到物体帧（object‘s frame）的原点（从左到右读，也就是说，依次经历局部变换的解读），再然后围绕这个点旋转从而达到与眼睛帧（eye）的轴方向对齐。（参考图示$\text{Figure 5.3}$）。

因此，对于这种物体运动（object motion），方程式（5.1）中矩阵A应该为$A=(O)_T(E)_R$。

有一种完成相同效果的替代计算。举个例子，假设，我们希望借助一个旋转轴$\vec{k}$围绕其自身的中心旋转一个物体，这个旋转轴具有关联于$\vec{\mathbf{e}}^t$的坐标$\mathbf{k}$。（在上面计算中我们其实用$\mathbf{k}$获得了一个矩阵M，然后和一个合适的矩阵A一起，我们更新物体矩阵$O \leftarrow AMA^{-1}O$。）我们可以首先计算$\mathbf{k'}$,$\mathbf{k}$的关联于$\vec{\mathbf{o}}^t$的坐标。然后将$\mathbf{k'}$代入方程式（2.5）中，我们可以得到一个矩阵M'，这个矩阵直接表达了关联于$\vec{\mathbf{o}}^t$的我们所要求的旋转。在这种情形中，我们可以更新物体矩阵（object's matrix）为：$$\Large{ O \leftarrow OM' }$$

### 5.2.2 移动眼睛（视角）（Moving the Eye）
另一种我们希望进行的操作是移动眼睛到不同的视角。这会涉及改动$\vec{\mathbf{e}}^t$,在程序中具体通过更新矩阵E来达到目的。再一次，我们可以挑选一个合适的坐标系，关联于这个坐标系我们执行$\vec{\mathbf{e}}^t$的更新，就如之前我们在物体上所做的操作。

One choice is to use the same auxiliary coordinate system we just used above. In this case, the eye would orbit around the center of the object.

Another useful choice is to transform e t with respect to the eye’s own frame. This models ego-motion, like turning ones head. This is commonly used to control ﬁrst-person motion. In this case the matrix E would be updated as

E ← EM

### 5.2.3 Lookat（盯着看）

Sometimes (especially for static images) it can be convenient to describe the eye frame, ⃗ e t = w t E, directly by specifying the position of the eye ˜p, a point ˜q that the eye is ⃗ looking directly at, and an “up vector” ⃗u describing the vertical direction of the eye. ⃗ These points and vector are given by p, q and u, their coordinates with respect to w t . Given this input, let

z y x

= =

=

normalize(q − p) normalize(u)

y×z

where

normalize(c) =c ⁄

￿

2 1

c

+ c 2 2 + c 3 2

Then the matrix E is deﬁned as ⎡

⎢ ⎢ ⎣

## 5.3 Scales

x 1 x 2 x 3

0

y 1 y 2 y 3

0

z 1 z 2 z 3

0

p 1 p 2 p 3

1

⎤

⎥ ⎥ ⎦

Thus far, we have thought of our world as being made up of movable objects, each ⃗ with its own orthonormal frame represented by its own rigid body matrix ⃗ o t = w t O. We have restricted our attention to orthonormal frames so that translation and rotation matrices work as we expect them to.

Of course, for modeling objects, we certainly may want to apply scaling as well. For example, we may want to model an ellipsoid as a squashed sphere. One way to deal with this is to keep a separate scale matrix O ′ around for the object as well. Then t the scaled (non-orthonormal) object frame is o ′ = ⃗ o t O ′ . This way we can still move

the object by updating its O matrix as above. To draw the object, we use the matrix E −1 OO ′ to transform the “scaled object coordinates” into eye coordinates.

## 5.4 Hierarchy

Often it is convenient to think of an object as being assembled by some ﬁxed or movable sub-objects. Each sub-object may have its own orthonormal frame, say ⃗ a t (and scaled frame as well). Then we can store the vertex coordinates of the sub-object in its own coordinate system. Given this hierarchy we want the ability to easily model motions of the object as a whole as well as independent motions of the sub-objects.

For example, when modeling a robot with moving limbs, we may use an object and scaled object frame to represent the trunk, a sub-object frame to represent a rotatable shoulder, and a scaled sub-sub-object frame to to represent the upper arm (which moves along with the shoulder). (See Figure 5.4).

When we move the whole object, by updating its O matrix, we want all of the subjects more along in unison (see Figure 5.5). To get this behavior, we represent the sub-object’s frame using a rigid body matrix that relates it to the object’s frame, (instead of relating it to the world frame). Thus, we store a rigid body matrix A which we interpret as deﬁning the relationship: a t = o t A, as well as a scale matrix A ′ deﬁning its t scaled sub-object frame as a ′ = ⃗ a t A ′ . To reposition the sub-object within the object,

all we do is update the matrix A. To draw the sub-object, we use the matrix E −1 OAA ′ which transforms “scaled sub-object coordinates” into eye coordinates. Clearly this idea can be nested recursively, and we can represent a sub-sub-object as b t = ⃗ a t B, t and a scaled sub-sub-object as b ′ = b t B ′ .


