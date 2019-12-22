在OpenGL中用Projection Matrix 分别左乘和右乘均匀或不均匀Scale Matrix的效果

## 环境描述
本文所述Projection Matrix均对应右手性（right handedness）的正交标准帧（frame），也即near/far值都为负值，Projection Matrix变换所产生的z-buffer值越大,意味着距离眼睛帧原点（相机/眼睛位置）越近。 在OpenGL环境中对应非默认的深度值比较方式函数设置`glDepthFunc(GL_GREATER)`。

## 基准Projection Matrix变换过程及成像参数描述
我们将最初的标准MVP变换对应的Projection Matrix称为基准Projection Matrix。Vertex Shader在执行MVP变换时，实际应用于顶点（Vertices）的变换顺序分别是Model Matrix，View Matrix，Projection Matrix。其中Model Matrix将对象坐标变换为world coordinate（世界坐标），View Matrix将world coordinate变换为eye coordinate（眼睛坐标），Projection Matrix将眼睛坐标变换为clip coordinate（裁切坐标）。也就是说MVP被按照字符顺序被先后应用于顶点变换。为了后面描述的方便，这里我们将基准MVP变换的eye coordinate记为$[x_e,y_e,z_e,1]^t$,clip coordinate记为$[x_c,y_c,z_c,1]^t$。 在经过裁切（clipping）和透视除法（perspective division）之后，clip coordinate变换为normalized device coordinate(标准设备坐标)$[x_n,y_n,z_n]^t$。最后normalized device coordinate被用于成像。

注：$[x,y,z,w]^t$标记表示集合为垂直集合。

我们从四种情形分别进行讨论，首先给定3个矩阵：

P为标准的Projection Matrix。
$$ \begin{bmatrix}
 \frac{1}{\alpha\tan(\frac{\theta}{2})} & 0 & 0 & 0 \\
 0 & \frac{1}{\tan(\frac{\theta}{2})} & 0 & 0 \\
 0 & 0 & \frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
 0 & 0 & -1 & 0 
\end{bmatrix}$$

S为不均匀Scale Affine Matrix（对角线vector为[3,3,3,1]）。
$$ \begin{bmatrix}
 3 & 0 & 0 & 0 \\
 0 & 3 & 0 & 0 \\
 0 & 0 & 3 & 0 \\
 0 & 0 & 0 & 1 
\end{bmatrix}$$

O为Uniformity Scale Matrix，但非Affine Matrix（对角线vector为[3,3,3,3]）。
$$ \begin{bmatrix}
 3 & 0 & 0 & 0 \\
 0 & 3 & 0 & 0 \\
 0 & 0 & 3 & 0 \\
 0 & 0 & 0 & 3 
\end{bmatrix}$$

### 情形一：将P替换为PS，即Projection Matrix 右乘（right multiplies）Scale Affine Matrix
对最终成像的影响：成像中可能会少一些对象，也可能会多一些对象，但是和基准变换相比保留的对象的成像没有变化

这种情况我们按照Vertex Shader变换顶点时的应用顺序，在Projection Matrix变换前，对眼睛坐标沿着每个axis进行了3倍放大，这样就获得了眼睛坐标$[3x_e,3y_e,3z_e,1]^t$。之后眼睛坐标被Projection Matrix变换为clip coordinate$[3x_c,3y_c,z_c',3w_c]^t$，考虑由于所有新的$x_c，y_c，w_c$ clip coordinate的范围都被放大了3倍，但是其中$z_c'$并没有同比放大3倍，据Projection Matrix可知其所限定的视图平截锥体（view frustum）的z轴上的长度范围并没有改变（仍为[f..n]范围），可能导致在基准变换中原来的一部分深度（$z_e$值）小的顶点（远离相机在far值之后）会被裁切，也可能会导致基准变换中深度在near值前的一部分点现在不被裁切。但是对于view frustum中留下的顶点，在执行perspective division之后所获得的normalized device coordinate $[x_n,y_n,z_n]^t$，尤其是$(x_n,y_n)$的值并没变化，从而我们看到最终成像和基准成像没变化（不考虑遮挡的情形，除了可能会少一些原来的对象，也可能会多出一些新对象外）。

### 情形二：将P替换为PQ，即Projection Matrix右乘均匀（Uniformity）Scale Matrix
对最终成像的影响：成像相比基准成像没有任何变化

相比于情形一，此矩阵首先将眼睛坐标的每个部件都放大3倍，包括第4部件$[3x_e,3y_e,3z_e,3]^t$。这样经过projection matrix变换后，我们就获得新的clip coordinate$[3x_c,3y_c,3z_c,3w_c]^t$。从新的clip coordinate中，我们可以看出虽然x，y，z轴坐标都同比放大3倍，但是w变量也被放大三倍，这样裁切后相比基准变换的顶点完全一样。再经过perspective division后，我们获得和基准变换完全相同的$[x_n,y_n,z_n]^t$。也就是说成像不会有任何变化。

### 情形三：将P替换为SP，即Projection Matrix 左乘（left multiplies）Scale Affine Matrix
对最终成像的影响：成像中对象的数目会经历（可能剧烈）减少，同时view frustum之外的新顶点不可能再进入view frustum之内。对比基准变换，留下的对象坐标分别放大3倍，同样顶点的成像比基准放大9倍。

这种情形我们直接从裁切坐标开始考虑，我们可以理解为基准MVP变换后生成裁切坐标$[x_c,y_c,z_c,w_c]^t$，然后非均匀Scale Matrix将前3个部件放大3倍成为新的clip coordinate $[3x_c,3y_c,3z_c,w_c]^t$。所有的部件都放大3倍，除了w变量（$w_c$）保留不变。这会导致在clipping phase，原来的顶点在x，y，z维度上被裁切掉的概率大大增加。因为生成的View Frustum同比放大3倍，而裁切范围没变，所以不可能有新的点再进入frustum。在经历perspective division之后，相比原来的顶点，获得的新normalized device coordinate变为$[3x_n,3y_n,3z_n]^t$。也就是说最终的成像会放大9倍。

### 情形四：将P替换为QP，即Projection Matrix 左乘（left multiplies）Uniformity Scale Matrix
对最终成像的影响：成像相比基准成像没有任何变化

从裁切过程开始同于情形二。



