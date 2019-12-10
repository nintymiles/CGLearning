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

What is a natural choice for ⃗ a t ? The most obvious choice would be to apply the transformation to ⃗ o t with respect to ⃗ o t itself. Unfortunately, this means that the axes used will be those that correspond to the object itself. “Rightward” will be in the object’s rightward direction, which will not correspond to any particular direction in the observed image. We might try to rectify this by instead transforming ⃗ o t with respect to ⃗ e t . This ﬁxes the axis problem but creates another problem. When we rotate the object, it will rotate about the origin of the eye’s frame; it will appear to orbit around the eye. But we typically ﬁnd it more natural to rotate the object about its center, which we think of as the origin of o t . (See Figure 5.2).

To ﬁx both of these problems, we can create a new frame that has the origin of the object, but the axes of the eye. To do this let us factor our matrices as follows

O E

= =

(O) T (O) R

(E) T (E) R

Where (A) T is the translational and (A) R is the rotational factor of A, as in Equation (3.3). We can then see that the desired auxiliary frame should be

⃗ a t = w t (O) T (E) R

⃗

(5.6)

This frame is obtained by starting with the world coordinate system and (reading left to right, i.e. successive local interpretation) translating it to the center of the object’s frame, and then rotating it about that point to align with the directions of the eye. (See Figure 5.3).

Thus for this kind of object motion, the matrix A of Equation (5.1) should be A = (O) T (E) R .

There is an alternative computation that accomplishes the same effect. Suppose, for example, that we wish to rotate an object around its own center using an axis of rotation k that has coordinates k with respect to ⃗ e t . (Above we used k to obtain a matrix M, and together with an appropriate A, we updated O ← AMA −1 O.) We can ﬁrst compute k ′ , the coordinates of k with respect to ⃗ o t . Then plugging k ′ into Equation (2.5), we can obtain a matrix M ′ that directly expresses our rotation with respect to ⃗o t . In this case we can update the object’s matrix as

O ← OM ′

