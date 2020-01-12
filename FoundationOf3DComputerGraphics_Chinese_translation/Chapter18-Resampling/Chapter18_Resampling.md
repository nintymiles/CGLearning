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
I(i,j)  \leftarrow  \iint_{M_{\Omega}} dx_tdy_t |det(D_N)| \, \sum_{k,l} B_{k,l}(x_t,y_t) T[k][l] 
}$$
那就是说，我们需要在纹理域中的区域$M$上积分，并且混合那些数据在一起。
当我们的变换M实际上缩小了纹理，那么M拥有T上的大脚印。(参考图示$\text{Figure 18.1})。如果M正放大纹理，那么M在T上拥有一个非常小的脚印。M也可以实现更有趣的内容，就如仅仅在一个方向缩小。
When our transformation M effectively shrinks the texture, then M(Ωi,j ) has a
large footprint over T (xt, yt). (See Figure 18.1). If M is blowing up the texture, then
M(Ωi,j ) has a very narrow footprint over T (xt, yt). During texture mapping, M can
also do funnier things, like shrink in one direction only.
