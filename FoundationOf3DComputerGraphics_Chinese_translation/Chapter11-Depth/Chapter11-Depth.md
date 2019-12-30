

# 深度（Depth）

## 11.1 可视性（Visibility）
在真实物理世界中，如果物体A处于物体B的前方，那么来自物体B的光线会被物体A阻挡，从而无法到达相机（成像），因而它将不会显示在图像中（比如，图示$\text{Figure 10.2}$的蓝色六边形）。在计算机图形中，我们需要以可计算方式建模这种情形。

存在多种方式可以用来确保只有对相机可见的表面出现在图像中。一种思路是通过深度排序三角形，然后以从后到前的顺序绘制它们。这种思路的关键是，最前边的三角形将会在被遮挡的三角形之上重绘从而产生正确图像。这种所谓的画家方式存在好多个难题。例如，场景可能包含互相渗透的三角形。也可能包含非互相渗透三角形的可视循环情形，这种情形被展示在图示$\text{Figure 11.1}$。

另一种通用型非常好的情形，我们会在第20章中讨论，为光线投射方式（ray casting）。这种方式中，对于每个像素，顺着像素的视线，我们明确计算每个被看到的场景点。最近的相交点随后被用于给像素上色。

在一个类似OpenGL这样的光栅化渲染器中，我们借助某种被称作z-buffer（z-缓存）的机制来执行我们的可视性计算。这种方式中，可以以任何顺序绘制三角形。在帧缓存的每个像素上，我们不仅存储一个色彩值而且还有一个“当前深度”值。这个值表达了用于设置像素当前值的几何体的深度。当一个新三角形尝试设置一个像素的色彩时，我们首先将其深度和存储在z-缓存中的值进行比较。只有这个三角形中被观察到的点更接近（眼睛）时，我们才会覆写这个像素的色彩和深度值。因为这种动作完成在每像素的基础上，其可以正确地处理互相渗透的三角形情形。

### 可视性计算的其它用法（Other Uses of Visibility Calculations）
可视性计算在计算一个被观察点的色彩时也可能是重要的。实际上，在决定是否被观察点可以直接看到某种“光源”或者是否在阴影中等方面也是重要的。在OpenGL中这可以借助阴影映射算法来完成，在小节15.5.2中。在光线追踪的环境中，我们只要使用射线相交代码就可以看到什么几何体被从始于被观察点到光源的射线相交。

可视性计算也可以被用于加速渲染处理。如果我们知道某个物体相对于相机被遮挡，那么我们首先就不必去渲染这个物体。这可能被用于室内场景的例子，其中我们通常不能看得太远以至于超出周围的房间。在这种环境中，我们可以使用一种保守可视性检测；这种检测会快速告知我们是否某个物体可能或者明确不可见。如果物体可能可见，那么我们就继续使用z-缓存渲染。但是如果物体明显不可视，那么我们可以完全跳过这种绘制。

## 11.2 基础数学模型（Basic Mathematical Model）
在OpenGL中，我们借助z-缓存计算可视性。要使用z-缓存，除了一个点的$[x_n,y_n]^t$外，我们还需要一个深度值。要完成这个任务，针对每个用眼睛坐标描述的点，我们借助下列矩阵表达定义其$[x_n,y_n,z_n]^t$坐标。
$$ \begin{array}{c}
\begin{bmatrix} x_nw_n \\ y_nw_n \\ z_nw_n \\ w_n \end{bmatrix}  = \begin{bmatrix} x_c \\ y_c \\ z_c \\ w_c \end{bmatrix}  =  \begin{bmatrix} s_x & 0 & -c_x & 0 \\ 0 & s_y & -c_y & 0 \\ 0 & 0 & 0 & 1 \\ 0 & 0 & -1 & 0 \end{bmatrix}\begin{bmatrix} x_e \\ y_e \\ z_e \\ 1 \end{bmatrix}  \tag{11.1}
\end{array} $$
再次，原始的输出被称作裁切坐标，并且和以前一样，要获得$x_n$和$y_n$的值，我们需要除以$w_n$值。但是现在我们还得到了$z_n=\frac{-1}{z_e}$值。我们计划使用这个$z_n$在z-缓存中进行深度比较。

让我们首先验证从方程式（11.1）中获得的$z_n$值确实可以被用于做深度比较。假定两个点$\tilde{p}_1$和$\tilde{p}_2$，分别有眼睛坐标$[x_e^1,y_e^1,z_e^1,1]^t$和$[x_e^2,y_e^2,z_e^2,1]^t$。假设它们都位于眼前，也就是说$z_e^1<0$和$z_e^2<0$。同时假设$\tilde{p}_1$比$\tilde{p}_2$更靠近眼睛，也就是说$z_e^2<z_e^1$。因而$-\frac{1}{z_e^2}<-\frac{1}{z_e^1}$,这就意味着$z_n^2<z_n^1$。

总之，我们可以把将从眼睛坐标给出的点转换为以标准化设备坐标所给出的点的处理当作是一种诚实的3D几何变换。这种变换即不是线性的也不是放射的，但是在某种意义上被称为3D投射变换。

投射变换有一点儿可笑；例如，上面的情形中，任何具有$z_e=0$的点将会引起“被0除”的情形。在这个时点，关于投射变换（参数在下面给出）我们需要知道的最重要的事情为它们保留了点的同线性（co-linearity）和同平面性（co-planarity），参考图示$\text{Figure 11.2}$和图示$\text{Figure 11.3}$。同线性意味着如果3个或更多的点位于一条线上，被变换的点也会在某条线上。

作为这种同平面性保留的结果，我们知道针对一个固定三角形上的点，针对某种固定的a，b和c，我们会有这种表达$z_n=ax_n+by_n+c$。因而，对于一个点正确的$z_n$值可，只要我们知道其在三角形中3个顶点的值，就可以借助在2D图像域上的线性插值来计算它（关于线性插值的更多内容，请参考附录B）。

然而，要注意，距离并没有被投射变换保留。再次观察$\text{Figure 11.2}$和$\text{Figure 11.3}$，我们看到在成像面上平均间隔的像素并没有对应于眼睛空间几何体上平均间隔的点。同时，这种平均间隔的像素确实对应于标准化设备坐标中平均间隔的点。

作为距离扭曲的结果，在屏幕上进行$z_e$值的线性插值会给出错误答案。考虑图示$\text{Figure 11.4}$中的2D绘制，对于互相渗透的线段，一个橙色的和一个蓝色的。顶点的$z_e$值被展示。两个线段右侧相交在图像平面的中点，所以在正确的图像中，上半部分应该是蓝色的而下半部分应该是橙色的。假设在图像空间中我们在每个线段上线性插值$z_e$值。所有蓝色像素会插值出值为-1的$z_e$值。而在橙色三角形上，从底部（$z_e=-1$）开始到顶部（大约$z_e=-1000000$），被插值的$z_e$值将会立即小于-1。因此，在z-缓存图像中，几乎整个图像都会是蓝色的！在第13章中，我们会看到我们如何正确地在屏幕上的一个三角形中插值出$z_e$值。

### Lines are preserved by projective transforms (optional)

Here we sketch the steps needed to establish that lines are preserved by projective transforms.

First, let us deﬁne a projective transform acting on a 3D afﬁne space. Fixing our frame, each point is represented by c, a 4-coordinate vector with 1 as its last entry. We then multiply this by a chosen 4-by-4 matrix P, to obtain d = Pc. There are no restrictions on the fourth row of P, but we will assume that the matrix is invertible. Finally, we divide each of the four entries in d by the fourth entry of d. This gives us our result e, which we interpret as the coordinates of another point in our afﬁne space. (Let us ignore the case where the fourth entry happens to be 0.) Such a transformation c ⇒ e. is called a projective transformation.

To see that this kind of transformation preserves co-linearity, let us think of c as a coordinate vector in the 4D linear space R 4 . If we started with points that all lie on some line in our 3D afﬁne space, then all the associated c coordinates must also represent vectors in R 4 that lie on some corresponding 2D vector subspace of R 4 . Let us now think of the multiplication by P as simply a linear transform acting on vectors in R 4 . This, as any linear transform, must map 2D subspaces to 2D subspaces. So all of the resulting d coordinates must lie on some ﬁxed 2D subspace in R 4 . Finally, when we divide by the fourth coordinate, we are simply scaling each of the resulting vectors so that they also lie on the 3D hyperplane in R 4 with a last coordinate of 1. Any such e must lie in a 1D line within the 3D hyperplane. This hyperplane is isomorphic to our 3D afﬁne space, and thus the resulting points of the projective transform must be co-linear.

## 11.3 Near And Far

When using the projection matrix of Equation (11.1), there can be numerical difﬁculties when computing z n . As z e goes towards zero, the z n value diverges off towards inﬁnity. Conversely, points very far from the eye have z n values very close to zero. The z n of two such far away points may be indistinguishable in a ﬁnite precision representation, and thus the z-buffer will be ineffective in distinguishing which is closer to the eye.

In computer graphics, this issue is typically dealt with by replacing the third row of the matrix in Equation (11.1) with the more general row [0, 0, α, β]. Once again, it is easy to verify that if the values α and β are both positive, then the z-ordering of points (assuming they all have negative z e values) is preserved under the projective transform.

To set α and β, we ﬁrst select depth values n and f called the near and far values (both negative), such that our main region of interest in the scene is sandwiched be2fn tween z e = n and z e = f. Given these selections, we set α = f−n f+n and β = − f−n . We can verify now that any point with z e = f maps to a point with z n = −1 and that a point with z e = n maps to a point with z n = 1 (See Figures 11.5 and 11.6). Any geometry not in this [near..far] range is clipped away by OpenGL and ignored (clipping is covered in Section 12.1).

Putting this together with the projection matrix of Equation (10.6), we obtain the matrix
$$ \begin{bmatrix}
 \frac{1}{\alpha\tan(\frac{\theta}{2})} & 0 & 0 & 0 \\
 0 & \frac{1}{\tan(\frac{\theta}{2})} & 0 & 0 \\
 0 & 0 & \frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
 0 & 0 & -1 & 0 
\end{bmatrix}$$

Alternatively, putting this together with the projection matrix of Equation (10.7), and using the near plane to deﬁne our bounding rectangle using the scalars l, r, b, t, we obtain the frustum projection matrix 

$$ \begin{bmatrix}
 \frac{1}{\alpha\tan(\frac{\theta}{2})} & 0 & \frac{r+l}{r-l} & 0 \\
 0 & \frac{1}{\tan(\frac{\theta}{2})} & \frac{t+b}{t-b} & 0 \\
 0 & 0 & \frac{f+n}{f-n} & -\frac{2fn}{f-n} \\
 0 & 0 & -1 & 0 
\end{bmatrix} \tag{11.2}$$

The effect of this projection matrix is to map the frustum to the canonical cube with corners [−1, −1, −1] t , [1, 1, 1] t .

For various historical reasons, there are a few variants of this matrix that appear in the literature and in OpenGL documentation. For example, some conventions assume that n is given as the distance to the near plane, and is thus a positive number. As another example, some conventions ﬂip the signs of the entries in the third row of the matrix. In the resulting (left handed) normalized device coordinates, nearer means smaller z n . Since we do not do this ﬂipping, we must explicitly tell OpenGL which sign to use for depth comparison; we do this with the call to glDepthFunc(GL GREATER).

## 11.4 Code

In OpenGL, use of the z-buffer is turned on with a call to glEnable(GL DEPTH TEST). We also need a call to glDepthFunc(GL GREATER), since we are using a right handed coordinate system where “more-negative” is “farther from the eye”.

Putting this all together, all we really need to do is to write procedures Matrix4 makeProjection(double minfov, double aspectratio, double zNear, double zFar) and Matrix4 makeProjection(double top, double bottom, double left, double right, double zNear, double ZFar) that return the appropriate projection matrices. We also need the procedure sendProjectionMatrix(projmat) to send the projection matrix to the appropriately named matrix variable in the vertex shader. The code for these procedures is found on our website. The actual multiplication with the projection matrix is done in the vertex shader as we did in Section 6.3.


