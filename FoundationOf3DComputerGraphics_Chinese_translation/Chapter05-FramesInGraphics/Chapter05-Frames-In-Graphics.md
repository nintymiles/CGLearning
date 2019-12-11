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

一种选择是使用和上面相同的辅助坐标系。在这种情形中，眼睛会围着物体的中心环绕运动。

另一种有效的选择是关联于眼睛自身的帧（frame）变换$\vec{\mathbf{e}}^t$。这会建模自我运动（ego-motion），就如转动你的头部。这通常被用于控制第一人称（first-person）运动。在这种情形中矩阵E会被更新为$E \leftarrow EM$

### 5.2.3 Lookat（盯着看）
> 本节有一些错误，已经根据本书的errata改正。

有时，特别是针对静态图像(static images)时，直接描绘出眼睛帧$\vec{\mathbf{e}}^t = \vec{\mathbf{w}}^tE$会很便利，通过指定眼睛的位置$\tilde{p}$,和一个眼睛正死盯着的点$\tilde{q}$，还有一个“上方矢量(up vector)"$\vec{u}$用于描述眼睛上方的方向。这些点和矢量通过变量p,q和u被给出，它们的坐标关联于$\vec{\mathbf{w}}^t$。假定这种输入，进行下面的计算：

$$
z = normalize(p-q) \\
x = normalize(u \times z) \\
y = normalize(z \times x)
$$

$$ normalize(c) = \frac{c}{\sqrt{c_1^2+c_2^2+c_3^2}}$$

然后矩阵E可以被定义为：
$$ \begin{bmatrix}
x_1 & y_1 & z_1 & p_1 \\
x_2 & y_2 & z_2 & p_2 \\
x_3 & y_3 & z_3 & p_3 \\
0 & 0 & 0 & 1
\end{bmatrix}$$


## 5.3 缩放变换（Scales）
目前为止，我们已经将我们的世界认为是由运动的物体组成，每个物体都有一个由其自身的坚固形体矩阵所表达的正交标准帧（orthonormal frame)$\vec{\mathbf{o}}^t = \vec{\mathbf{w}}^tO$。我们限制关注点到正交标准帧以便平移和旋转矩阵可以像我们预期它们的行为那样工作。

当然，要建模物体，我们当然想同时应用缩放。例如，我们想要建模一个椭球体为一个被挤压的球体。一种处理方式是让这个物体同时拥有一个缩放矩阵（scaled matrix）$O'$。然后被缩放的物体帧（非正交标准化的）被确定为$\vec{\mathbf{o}}'^t = \vec{\mathbf{o}}^tO'$。这种方式中我们仍然像上面一样通过更新矩阵O移动物体。要绘制这个物体，我们使用矩阵$E^{-1}OO'$将“缩放的物体坐标”变换为眼睛坐标。

## 5.4 Hierarchy
经常将一个物体（对象）当作由某些固定的或者移动的多个子对象（sub-objects）组装而成，这种观点其实很有用。每个子对象可以拥有自己的正交标准帧（orthonormal frame），比如说$\vec{\mathbf{\alpha}}^t$。（同时还有缩放帧-scaled frame）。然后我们可以用其自己的坐标系存储这个子对象的顶点。给定这个层次体系，我们想拥有这种能力，即轻松以整体建模物体的运动，同时可以独立建模子对象的运动。

For example, when modeling a robot with moving limbs, we may use an object and scaled object frame to represent the trunk, a sub-object frame to represent a rotatable shoulder, and a scaled sub-sub-object frame to to represent the upper arm (which moves along with the shoulder). (See Figure 5.4).

When we move the whole object, by updating its O matrix, we want all of the subjects more along in unison (see Figure 5.5). To get this behavior, we represent the sub-object’s frame using a rigid body matrix that relates it to the object’s frame, (instead of relating it to the world frame). Thus, we store a rigid body matrix A which we interpret as deﬁning the relationship: a t = o t A, as well as a scale matrix A ′ deﬁning its t scaled sub-object frame as a ′ = ⃗ a t A ′ . To reposition the sub-object within the object,

all we do is update the matrix A. To draw the sub-object, we use the matrix E −1 OAA ′ which transforms “scaled sub-object coordinates” into eye coordinates. Clearly this idea can be nested recursively, and we can represent a sub-sub-object as b t = ⃗ a t B, t and a scaled sub-sub-object as b ′ = b t B ′ .


