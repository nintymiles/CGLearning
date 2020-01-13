# 重采样（Resampling）
现在，图像采样和图像重建的基础我们都已然讨论过了，我们可以回顾纹理映射的处理过程。我们会看到，在这种情形中，我们始于一个具体图像然后终于一个具体图像。总之，这种映射技术性地涉及了重建和采样两个阶段。以此为上下文，我们将解释用于反锯齿纹理映射中的渐进式纹理映射（mip mapping）。

## 18.1 理想重采样（Ideal Resampling）
假设我们开始于一个具体图像或者纹理$T[k][l]$并且对这种图像应用某种2D扭曲以获得一个输出图像$I[k][l]$。我们应该怎么设置每个输出像素？实际上，在纹理映射中，三角形和相机几何体实际上扭曲到窗口中被渲染图像的某部分之上。

理想情况下我们应该遵从下列步骤集合

- 借助基函数$B_{k,l}(x_t,y_t)$集合重建一个连续纹理$T(x_t,y_t)$，就如在第17章中那样。
- 针对连续图像应用几何扭曲。
- 针对过滤器$F_{i,j}(x_w,y_w)$集合获得具体输出图像，就如在第16章中那样。

让几何变换通过一个映射$M(x_w,y_w)$被描述，这个映射从连续窗口到纹理坐标。那么，把这3个步骤放在一起，我们得到
$$\large{ \begin{array}{rcl}
I(i,j) & \leftarrow & \iint_{\Omega} dx_wdy_w F_{i,j}(x_w,y_w) \sum_{k,l} B_{k,l}(M(x_w,y_w)) T[k][l] \\
& = &  \sum_{k,l} T[k][l] \iint_{\Omega} dx_wdy_w F_{i,j}(x_w,y_w) B_{k,l}(M(x_w,y_w))
\end{array} }$$
这个表达式告诉我们每个输出像素如何能够作为输入纹理像素的线性组合方式被获得。有时候更容易在纹理域上可视化这种整合，而不是在窗口域中。借助反转映射$N=M^{-1}$,这可以被表达为
$$\large{ \begin{array}{rcl}
I(i,j) & \leftarrow & \iint_{M_{\Omega}} dx_tdy_t |det(D_N)| \, F_{i,j}(N(x_t,y_t)) \sum_{k,l} B_{k,l}(x_t,y_t) T[k][l] \\
& = &  \iint_{M_{\Omega}} dx_tdy_t |det(D_N)| \, F'_{i,j}(x_t,y_t) \sum_{k,l} B_{k,l}(x_t,y_t) T[k][l]
\end{array} }$$
此处$D_N$为N的雅可比行列式（Jacobian）并且$F'=F \dot N$。当F为一个盒式过滤器，这个公式变为
$$\large{
I(i,j)  \leftarrow  \iint_{M_{\Omega}} dx_tdy_t |det(D_N)| \, \sum_{k,l} B_{k,l}(x_t,y_t) T[k][l]  \tag{18.1}
}$$
那就是说，我们需要在纹理域中的区域$M$上积分，并且混合那些数据在一起。

当我们的变换M实际上缩小了纹理，那么$M(\Omega_{i,j})$拥有$T(x_t,y_t)$上的大脚印。(参考图示$\text{Figure 18.1}$)。如果$M$正放大纹理，那么$M(\Omega_{i,j})$在$T(x_t,y_t)$上拥有一个非常小的脚印。$M$也可以实现更有趣的内容，就如仅仅在一个方向缩小。


## 18.2 放大（Blow up）
虽然方程式（18.1）既包含了一个重建（$\sum$)组件，也包含了一个过滤（$\int$)组件，在我们放大纹理的情形中，过滤组件在输出结果上有最小的影响。实际上，$M(\Omega_{i,j})$的脚印在纹理空间中可能小于一个像素，从而没有很多细节需要模糊/平均。如此，几分步骤可以被去掉，并且重采样可以被实现为
$$\large{
I(i,j)  \leftarrow   \sum_{k,l} B_{k,l}(x_t,y_t) T[k][l]  \tag{18.2}
}$$
此处$(x_t , y_t ) = M(i, j)$。换句话说，我们只是点采样被重建和变换的纹理。例如，这就是我们在第17章中所提议的。

双线性重建最常见被使用在这种情形中。我们借用API调用`glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)`告知OpenGL做这件事情。对于碎片着色器中的单一纹理查找，硬件需要获取4个纹理像素并且恰当地混合它们。比如说，对于图示$\text{Figure 18.1}$中拥有橙色脚印的像素这种方式会运行良好。

## 18.3 渐进式映射（Mip Map）
在一个纹理被缩小的情形中，那么，要避免锯齿化，方程（18.1）中的过滤组件不应该被忽略。不幸的是，可能存在大量的纹理像素具备$M(\Omega_{i,j})$的脚印（footprint），并且我们可能不能以常量时间进行纹理查找。例如观察图示$\text{Figure 18.1}$中的黄色脚印。

OpenGL中的标准方案是使用渐进式映射。参考图示$\text{Figure 18.3}$。在渐进式映射中，你开始于一个初始纹理$T^0$，然后生成一系列越来越低分辨率（更模糊的）纹理$T^i$。每个后继的纹理为前一个的两倍模糊。并且因为它们具有连续变得更少的细节，它们可以在水平和垂直方向都是用1/2数目的像素被表达。这种集合，称为一个渐进式映射，在任何三角形渲染被完成之前被构建。因而，最简单的构建一个渐进式映射的方式是平均$2\times2$像素块从而产生低分辨率图像中的每个像素；更成熟的图像缩小技术也可以被使用。

渐进式映射可以使用你自己的代码构建，然后使用`glTexImage2D`传递到OpenGL。可替换地，它们可以使用`gluBuild2DMipmaps`调用被构建和自动加载（并且不需要随后对`glTexImage2D`的调用）。因为这些不同的纹理每个都有不同的分辨率，每个都拥有自己的视口变换（viewport transform）从经典纹理坐标变换到就如小节12.3.1和练习35中的基于像素的情形。

在纹理映射过程中，针对每个纹理坐标$(x_t,y_t)$，硬件评估正在进行多大程度的伸缩。这种缩小因子然后被用于从渐进式映射中选择一个合适分辨率的纹理$T^i$。因为我们挑选了一个合适的低分辨率纹理，额外的过滤不再需要，并且再一次，我们可以使用方程式（18.2），这个方程式可以在常量时间被完成。

要避免空间和时间的不连续性，这里/这个时候纹理渐进式映射在不同级别切换，我们可以使用所谓的三线性过滤（trilinear interpolation）。我们使用双线性插值从$T^i$重建一个色彩同时还从$T^{i+1}$重建另一个色彩。这两个色彩随后被线性插值。这第桑感插值因子基于我们以何种接近的程度选择级别i或者i+1。使用三线性插值的渐进式映射通过调用`glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR)`来指定。三线性插值要求OpenGL获取8个纹理像素并且针对每个被请求的纹理访问恰当地混合它们。


It is easy to see that mip mapping does not do the exactly correct computation. First of all, each lower resolution image in the mip map is obtained by isotropic shrinking, equally in every direction. But, during texture mapping, some region of texture space may get shrunk in only one direction (see Figure 18.2). Second of all, even for isotropic shrinking, the data in the low resolution image only represents a very speciﬁc, dyadic, pattern of pixel averages from the original image. These speciﬁc averages are then blended together when a texture pixel is fetched.

The Equation (18.1) can be better approximated at the expense of more fetches from various levels of the mip map to approximately cover the area M(Ω i,j ) on the texture. This approach is often called anisotropic ﬁltering, and can be enabled in an API or using the driver control panel. See Figure 18.3.

