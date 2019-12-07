# 平滑插值（Smooth Interpolation）
让我们考虑一种被称为关键帧动画（keyframe animation）的技术。在这种环境中，一个动画绘制器（animator）会描述位于一系列具体时刻之上的3D计算机图形动画的快照（snapshot）。每个快照（snapshot）根据某种集合的建模参数被定义。这些参数可能包含很多对象的位置（locations）和方位（orientations）信息，当然也包含相机的位置和方位信息。除此还包含模型活动部件的关节角度等信息。要从这些关键帧生成平滑动画，计算机的工作就是在连续的时间范围上平滑地“填入”参数值。如果一个这种动画参数被称为c，那么我们的每个具体快照（snapshot）可被称为$c_i$,此处i为某种范围的整数，那么我们的工作就是将这个参数的快照（snapshots）转变为一个关于时间的联系函数，$c(t)$。我们通常想让函数$c(t)$足够平滑，以便生成的动画不会显得太断断续续。

在本章中，我们会讨论在位于局部实数范围上的这样一套具体值上平滑插值的简单方式，这些方式一般借助splines函数。举个例子，在图示$\text{Figure 9.1}$中我们展示了一个函数$c(t)，t \in [0..8] $,这个函数在关联于整数值$c_i,i \in -1..9 $的具体值上插值，它们被展示为蓝色点（具体对额外的不参与插值的在-1和9处的具体值的需求随后澄清）。我们的splines函数将由独立的块函数（piece functions）组成，这里每个块函数是某种低等多项式函数。这些多项式块函数会被专门挑选以便它们可以平滑地”缝合在一起“。splines函数经常被用于计算机图形中，因为它们易于表达，评估和控制。实际上，它们的行为相比单一的高等多项式函数要容易预测得多。

我们也会展示这样的splines函数表达如何被用于描述空间和平面中的曲线。这样的曲线有超越动画之外的许多用途。例如，它们可以在字体中被用于描述字符形状的轮廓。

## 9.1 立方贝塞尔函数（Cubic Bezier Functions）
我们通过观察如何表达一个立方多项式函数$c(t),t \in [0..1]$(参考图示$\text{Figure 9.2}$开始。有很多可能的方式可以做这件事情，此处我们描述Berzier表达方法。这是一种方便的表达，其中的参数拥有几何解释，并且评估减弱为重复的线性插值。

在立方贝塞尔表达法（cubic Bezier representation）中，一个立方函数被4个被称为控制值（control value）的数值$c_0,d_0,e_0,c_1$所描述。在图示$\text{Figure 9.2}$中，我们可在2D(t,c)平面视化这种数据为点，这些点拥有坐标$[0,c_0],[1/3,d_0],[2/3,e_0],[1,c_1]$。我们同时以浅蓝色绘制一个由连接这些点所形成的折线（poly-line）；这条折线被称为控制多边形（control polygon）。

要评估在任何t指上的函数c(t)，我们执行下面的线性插值序列：

$$
   f = (1 − t)c_0 + td_0 \qquad\qquad\qquad\qquad (9.1)\ \\
	g = (1 − t)d_0 + te_0 \qquad\qquad\qquad\qquad (9.2) \\
	h = (1 − t)e_0 + tc_1 \qquad\qquad\qquad\qquad (9.3) \\
   m = (1 − t)f + tg  \qquad\qquad\qquad\qquad\; (9.4) \\
   n = (1 − t)g + th  \qquad\qquad\qquad\qquad\; (9.5) \\
   c(t) = (1 − t)m + tn \qquad\qquad\qquad\qquad (9.6) \\
$$

在图示$\text{Figure 9.3}$中，当t=0.3时，我们可视化这种计算步骤为2D空间中的线性插值。

### 9.1.1 属性（Properties）
很容易验证函数c(t)具有下列属性。

通过展开上面的评估步骤，我们可以证实函数c(t)具有下面的形式：

$$c(t)= c_0(1 − t)^3 + 3d_0t(1 − t)^2 + 3e_0t^2(1 − t) + c_1t^3$$

清晰无误地，这是一个立方函数。更进一步，$c_i$被函数插值：$c(0)=c_0$ 和 $c(1)=c_1$。通过推导，我们看到 $c'(0) = 3(d_0-c_0) $ 和 $ c'(1) = 3(c_1-e_{0})$。在图示$\text{Figure 9.2}$中，我们确实观察到c(t)函数的斜率（slope）匹配在0和1处控制多边形（control polygon）的斜率。我们也可以看到如果我们设置$c_0 = d_0 = e_0 = c_1 = 1$，那么对于所有的t值，函数c(t)=1。这种属性被称作一致性分区（partition of unity），那么这意味着为所有的控制值（control value）增加一个常量值也仅相当于给函数c(t)增加这个常量值。

### 9.1.2 平移（Translation）
如果我们想让一个立方函数在t=i和t=i+1时分别在$c_i$和$c_{i+1}$上插值，同时调用两个另外的控制值$d_i$和$e_i$，我们就不得不“平移”方程（9.1）中的评估算法，从而获得下列评估表达。

$$
   f = (1 − t + i)c_i + (t-i)d_i \qquad\qquad\qquad\qquad (9.7)\ \\
	g = (1 − t + i)d_i + (t-i)e_i \qquad\qquad\qquad\qquad (9.8) \\
	h = (1 − t + i)e_i + (t-i)c_{i+1} \qquad\qquad\qquad\quad (9.9) \\
   m = (1 − t + i)f + (t-i)g  \qquad\qquad\qquad\qquad\; (9.10) \\
   n = (1 − t + i)g + (t-i)h  \qquad\qquad\qquad\qquad\; (9.11) \\
   c(t) = (1 − t +i)m + (t-i)n \qquad\qquad\qquad\quad (9.12) \\
$$

## 9.2 Catmull-Rom Splines

Let us return now to our original problem of interpolating a set of values c i for i ∈ −1..n + 1. An easy way to do this is using Catmull-Rom splines. This method deﬁnes a function c(t) for values of t ∈ [0..n]. The function is deﬁned by n cubic functions, each supported over a unit interval t ∈ [i..i + 1]. The pieces are chosen to interpolate the c i values, and to agree on their ﬁrst derivatives.

Each function is described in its Bezier representation, using four control values: c i , d i , e i , and c i+1 . From our input data, we already have the c i values. To set the d i and e i values we impose the constraint c ′ (t)| i = 1 2 (c i+1 − c i−1 ). In other words, we look forward and backwards one sample to determine its slope at t = i; this is why we need the extra extreme values c −1 and c n+1 . Since $c'(i) = 3(d_i-c_i) = 3(c_i-e_{i-1})$, in the Bezier representation, this tells us that we need to set

$$ 
	d_i = \frac{1}{6}(c_{i+1}-c_{i-1}) + c_i  \qquad\qquad(9.13)\\
	e_i = -\frac{1}{6}(c_{i+2}-c_{i}) + c_{i+1} \qquad\quad(9.14)
$$

This process is visualized in Figure 9.4. Here we show the c, d and e values (in red) that are needed for one cubic segment. We show the control polygon of one cubic piece in light blue. The d and e are determined using the chords shown in dark blue.

## 9.3 Quaternion Splining

The Bezier and Catmull-Rom representations can be used to interpolate any real valued animation parameter. For example, if we have a time varying translation, each of its three deﬁning parameters t x , t y and t z can be interpolated independently.

If we want to interpolate a set of orientations, the theory does not directly apply. There is ongoing research on the best theoretical approach to use in this case. One efﬁcient hack is to simply substitute appropriate quaternion operations for the scalar operations in the algorithms above. With these substitutions, scalar addition becomes quaternion multiplication, scalar negation becomes quaternion inversion, and scalar multiplication becomes quaternion power. Doing so, Bezier evaluation steps of the form

$r = (1 − t)p + tq$ become $r = slerp(p, q, t)$

and the d i and e i quaternion values are deﬁned as

$$ 
d_i = ((c_{i+1}c_{i−1}^{−1} )^\frac{1}{6})c_i \\
e_i = ((c_{i+2}c_{i}^{−1} )^\frac{-1}{6})c_{i+1}
$$

As in Section 7.4, in order to interpolate “the short way”, we negate the quaternion c i+1 c −1 before applying the power operator if its ﬁrst coordinate is negative. i−1

Other slerp based quaternions spline methods are described in [68] and [66]. Another more natural, but more expensive method based on n-way spherical blends is described in [10].

Another alternative, often used in practice, is to simply treat the quaternions as points in R 4 , do standard splining on these four coordinates, and then normalize the result to get a unit-norm quaternion. In this context, the conditions needed for continuity between segments are described in [22].

## 9.4 Other Splines

There are a variety of other types of spline functions whose names we will mention, as you may run across these, but will not go into in detail. These splines are studied heavily in the ﬁeld of computer aided geometric design. The interested reader may wish to read [21] and [59].

A natural cubic spline is a function that interpolates its control values and minimizes the integral of second derivative. Such a spline is in some sense “the smoothest” interpolant. It turns out that a natural spline will be made up of cubic pieces which meet up with agreement in both ﬁrst and second derivative. A linear system needs to be solved to determine such a natural cubic spline. The behavior of this spline is global: moving one control value will affect the function over its entire range.

A uniform (cubic) B-spline represents a function using a set of cubic polynomials that are stitched together so that they match in second derivative as well. Unlike the natural cubic spline, this function changes only locally with changes in a control value. A B-spline, however, does not interpolate any of its control values, but merely approximates them.

A non uniform B-spline is a generalization of the uniform B-spline that allows for non-uniform spacing of the control values over the t line, while maintaining two degrees of continuity.

Finally, a non uniform rational B-spline (or NURB) is a generalization of the non uniform B-spline where each “piece” of the function is a rational expression (a ratio) of two polynomial functions. The rational form allows one to model many classical algebraic shapes.

## 9.5 Curves in Space ......

For the purpose of animation, we have looked at univariate scalar functions (where we think of the free variable as time). One can also use a spline representation to describe curves in space. In this case, the spline curve is controlled by a set of control points ˜c i in 2D or 3D. Applying the spline construction independently to the x y and z coordinates, one gets a point-valued spline function ˜c(t); you can think of this as a point ﬂying through space over time, tracing out the spline curve, γ.

Using the Bezier construction with four control points, we obtain a Bezier curve (see Figure 9.5), while using the Catmull-Rom construction on a set of control points, we obtain a Catmull-Rom curve (see Figure 9.6).

In the context of curves, the partition of unity property implies that if one translates all of the control points by some ﬁxed amount, then this just translates the curve γ by that amount.

Additionally, for Bezier (and also B-spline) curves, for each t, the value ˜c(t) is obtained by some mixture of the positions of the control points, where the mixing amounts are always between 0 and 1. As a result, the curve must lie in the “convex hull” of the control polygon deﬁned by connecting the control points in space. This implies that the curve will not wander too far away from its control polygon.


