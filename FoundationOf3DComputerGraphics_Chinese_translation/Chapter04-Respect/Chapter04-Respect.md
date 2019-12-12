# 关联（Respect）

## 4.1 帧的重要性（The Frame Is Important）
在计算机图形学中我们同时跟踪多种不同的帧（frames）。例如，我们针对场景中每个物体都会拥有一个不同的帧。我们如何使用和组织这些帧的详细说明在第5章中会被讲述。因为存在这许多帧，当运用矩阵定义变换时，我们需要尤其小心。

Suppose we specify a point and a transformation matrix, this does not fully speciﬁed the actual mapping. We must also specify what frame we are using. Here is a simple example showing this. Suppose we start with some point ˜p as well as the 
假如我们指定一个点和一个变换矩阵（transformation matrix），这并不能完全明确出实际的变换。我们还要确定出我们正在使用什么帧（frame）。这里有一个展示这种情况的简单例子。假设我们开始于点$\tilde{p}$以及一个矩阵
$$ \large{ S =  \begin{bmatrix}
2 & 0 & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\  0 & 0 & 0 & 1
\end{bmatrix} }$$
现在我们要指定帧$\vec{\mathbf{f}}^t$。借助这个帧，点可以借助某种恰当的坐标矢量（coordinate vector）表示为$\tilde{p}=\vec{\mathbf{f}}^t\mathbf{c}$。如果我们使用这个矩阵去变换这个点，就如在第3章中所讲述的，我们获得$\vec{\mathbf{f}}^t\mathbf{c} \Rightarrow \vec{\mathbf{f}}^tS\mathbf{c}$。在这种情形中，矩阵的效用是从原点（origin）通过一个为2的伸缩因子（scale factor）变换点，沿着$\vec{\mathbf{f}}^t$第一个轴（x轴）的方向。

Suppose we instead pick some other frame ⃗ a t , and suppose that this frame is related to the original one by the matrix equation ⃗ a t = f t A. We can express the original point in the new frame with a new coordinate vector ˜p = f t c = ⃗ a t d, where d = A −1 c.
假设我们挑选另外某个帧$\vec{\mathbf{a}}^t$，同时假设这个帧关联于原来的帧，通过矩阵方程式$$

Now if we use S to perform a transformation on the point represented with respect to ⃗ a t , we get ⃗ a t d ⇒ ⃗ a t Sd. In this case we have scaled the same point ˜p, but this time we have scaled it from the origin of ⃗ a t in direction of the ﬁrst (x) axis of ⃗ a t . This is a different transformation (see Figure 4.1). Figure 4.2 shows the same dependence on


