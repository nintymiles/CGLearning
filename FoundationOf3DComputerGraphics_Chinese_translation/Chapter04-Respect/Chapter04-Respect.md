# 关联（Respect）

## 4.1 帧的重要性（The Frame Is Important）
在计算机图形学中我们同时跟踪多种不同的帧（frames）。例如，我们针对场景中每个物体都会拥有一个不同的帧。我们如何使用和组织这些帧的详细说明在第5章中会被讲述。因为存在这许多帧，当运用矩阵定义变换时，我们需要尤其小心。

Suppose we specify a point and a transformation matrix, this does not fully speciﬁed the actual mapping. We must also specify what frame we are using. Here is a simple example showing this. Suppose we start with some point ˜p as well as the 
假如我们指定一个点和一个变换矩阵（transformation matrix），这并不能完全明确出实际的变换。我们还要确定出我们正在使用什么帧（frame）。这里有一个展示这种情况的简单例子。假设我们开始于点$\tilde{p}$以及一个矩阵
$$ \large{ S =  \begin{bmatrix}
2 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\  0 & 0 & 0 & 1
\end{bmatrix} }$$
现在我们要指定帧$\vec{\mathbf{f}}^t$。借助这个帧，点可以借助某种恰当的坐标矢量（coordinate vector）表示为$\tilde{p}=\vec{\mathbf{f}}^t\mathbf{c}$。如果我们使用这个矩阵去变换这个点，就如在第3章中所讲述的，我们获得$\vec{\mathbf{f}}^t\mathbf{c} \Rightarrow \vec{\mathbf{f}}^tS\mathbf{c}$。在这种情形中，矩阵的效用是从原点（origin）通过一个为2的伸缩因子（scale factor）变换点，沿着$\vec{\mathbf{f}}^t$第一个轴（x轴）的方向。

假设我们挑选另外某个帧$\vec{\mathbf{a}}^t$，同时假设这个帧关联于原来的帧，通过矩阵方程式$\vec{\mathbf{a}}^t = \vec{\mathbf{f}}^tA$。我们可以在新帧中表达最初的点，借助一个新的坐标系$\tilde{p}=\vec{\mathbf{f}}^t\mathbf{c} = \vec{\mathbf{a}}^t\mathbf{d}$，此处$\mathbf{d}=A^{-1}\mathbf{c}$。

现在，如果我们关联于$\vec{\mathbf{a}}^t$使用矩阵S去执行在点上的变换，我们获得$\vec{\mathbf{a}}^t\mathbf{d} \Rightarrow \vec{\mathbf{a}}^tS\mathbf{d}$。在这种情形中，我们伸缩了相同的点$\tilde{p}$，但是这次我们已经从$\vec{\mathbf{a}}^t$的原点（origin）且在其第一个轴（x轴）的方向上伸缩（移动）了这个点。这是一个不一样的变换（参考图示$\text{Figure 4.1}$）。图示$\text{Figure4.2}$展示了在帧上旋转变换的相同依赖性，借助于一个固定的旋转矩阵R。

The important thing to notice here is that the point is transformed (non uniform scaling in this case) with respect to the the frame that appears immediately to the left of the transformation matrix in the expression. Thus we call this the left of rule. We read

˜p = f t c ⇒ f t Sc as “˜p is transformed by S with respect to f t ”. We read

˜p = a t A −1 c ⇒ a t SA −1 c

as “˜p is transformed by S with respect to ⃗ a t ”.

We can apply the same reasoning to transformations of frames themselves. We read

f t ⇒ f t S as “f t is transformed by S with respect to f t ”. We read

f t = a t A −1 ⇒ a t SA −1 as “f t is transformed by S with respect to ⃗ a t ”

