# 采样（Sampling）
现在是时候让我们更聚焦一点在我们对待图形的方式上。目前为止，存在两种我们已经隐含使用的图像概念，具体（也称作，数码）图像和连续图像。在后面的章节中，我们会更靠近地调查这种对立性，同时讨论在具体和连续模型之间自由变换的合适方式。

本章中，我们会聚焦在多种计算机图形中生成电子图像时可能出现的视觉失真。这些失真包括在三角形边缘令人烦恼的锯齿形失真，并且可经由其名称:锯齿状失真（aliasing artifacts）来判断。在某种意义上，锯齿状失真可能发生在大量视觉复杂性填充在一个单一像素中时。在这种情形中，当我们尝试通过观察图像中一个单一点地址来决定像素的色彩值时，我们可能得到一个不是最优的结果。就如我们会看到的，这些失真可以通过将像素正方形区域在多个采样地址的色彩平均在一起来减轻。这种处理被称作抗锯齿。

## 16.1 两种模型（Two Models）
连续图像，$I(x_w,y_w)$，为双变量函数。正如在小节12.3中所讨论的，函数的域，$\Omega = [-.5..W-.5] \times [-.5..H-.5]$，为实数值化的2D窗口坐标$[x_w,y_w]^t$。（在本章剩下的内容和下一章中，除非被需要，否则我们去掉"w"下标。）函数的范围是一个色彩空间，我们使用一种RGB(线性）色彩空间。

具体图像$I[i][j]$是色彩值的两维数组。每个这样的数组条目被称为一个像素。数组的尺寸为宽W乘以高H，因而i是一个整数，区间为$[0..W-1]$，j也是一个整数，区间为$[0..H-1]$。我们为每对连续图像坐标$x_w=i$和$y_w=j$关联一对整数$i,j$，也就是说，碰巧为整数值的实数坐标。每个色彩值为色彩空间中表达一个色彩的标量三元组。再次，我们借助一个RGB（线性）色彩空间。最终，每个R，G，B坐标被表达为某种数值格式。

在OpenGL中，我们借助3D中被着色的三角形描述一个场景。根据投射，这些映射到连续2D图像域中上色的三角形，因而定义了一个连续图像。最终我们希望把这个图像显示到一台监视器上，或者把它保存到一个图像文件中。如此，我们需要知道如何把连续图像转换为具体图像。

## 16.2 问题（The Problem）
从连续图像到具体图像转换的最简单和最直观的方式为借助点采样，也就是说，要获得像素$i,j$的值，我们采样位于整数值化的域地址处的连续图像函数：
$$\large{ I[i][j] \leftarrow I(i, j)  }$$

可以证实存在很多点采样导致的不想看到的失真情形。例如，考虑一个由黑和白三角形组成的场景。当点采样图像时，在接近黑白三角形边缘的地方，图像会显示出某种阶梯状的图案；这种失真被称为“锯齿”。（参考图示$\text{Figure 16.6}$）。在动画过程中，某些像素值会突然从黑变到白，同时锯齿状的边缘图案会显示为爬行的样子。这被称作“爬行的锯齿（the crawing jaggies）”。

当处理非常小的三角形时，我们还会得到另外的糟糕失真情形。假如我们正在渲染一张由一群斑马组成的图片（或者是一张正在远离观察者的棋盘上的小的黑白正方形）。假设一只斑马在背景中远离，很远以至于其只覆盖一个单一像素。那个像素的色彩会是什么？如果这种点采样碰巧落在一个黑色三角形上，那么像素会是黑的，如果碰巧落在一个白色三角形上，那么像素会是白的。（参考图示$\text{Figure 16.1}$。）如果我们运气不好，最终的像素色彩会组成某种令人烦恼的“云纹（moire）”图案，就如图示$\text{16.6}$中所示。在动画过程中，如果斑马移动，这些像素将会呈现黑白变换的模式。这种失真被称为“闪烁（flickering）”。假如一个细节高度清晰的纹理被应用在某个屏幕上的小区域，那么在纹理映射中也会出现这种失真。（参考图示$\text{Figure 18.3}$）。

当在一个小区域连续图像有很多细节，那么这些类型的错误就会出现。在斑马例子中，连续图像在几个像素的空间中就包含了整群斑马（的细节）。对于锯齿三角形例子，连续图像具有可以立即过渡的精确锋利的边缘，而不是渐变的边缘。出于技术原因，这些种问题被统称为锯齿（aliasing）。

16.3 解决方案（The Solution）
当在小区域存在大量细节，我们能够在具体（图像）表达中维护所有信息是不可能的。但是仅通过采纳单一样本来决定我们正在生成的像素的色彩会让事情变得更糟，很可能只是拿到了一个没有意义的值。有意义的行为是我们最好借助在某种合适的区域中的平均值设置这个像素的值。在斑马例子中，可能我们最好只是设置这个像素为灰色。

存在很多方法以数学方式建模这种问题并寻求最优解。例如，采用“傅立叶分析”，你可以观察哪些“频率”使用一定数目的像素能够表达，以及处理不可表达频率的最好方式。针对这样的讨论，参考课后书目[6]。从一个最优化的观点，你可以尝试最小化原始连续图像和最终在屏幕上显示的光图像之间的差异。（在屏幕上，每个像素被绘制在有限范围内，因此我们可以将光图像当作屏幕上的合成连续图像。）

从这些多种观点，对这些观点我们这里不追求细节，证实了最好借助下面这种形式的表达来设置像素值：
$$ \large{
I[i][j] \leftarrow \iint_\Omega dxdyI(x,y)F_{i,j}(x,y) \tag{16.1}
}$$
此处$F_{i,j}(x,y)$为某种函数，其告知我们$[x,y]^t$的图像值应该多么强烈影响$i,j$处的像素值。在这种设置中，函数$F_{i,j}(x,y)$被称作过滤器（filter）。


In other words, the best pixel value is determined by performing some continuous weighted averaging near the pixel’s location. Effectively, this is like blurring the continuous image before point sampling it to obtain a discrete image. The use of Equation (16.1) to create a discrete image is referred to as anti-aliasing. 

The optimal choice for F i,j turns out to depend on the particular mathematical approach used to model the problem. In theory, such an optimal F i,j might have a wide support, and even take on negative values. In practice we do not require complete optimality, and we often choose the ﬁlters F i,j (x, y) to be something that can more easily computed with. The simplest such choice is a box ﬁlter, where F i,j (x, y) is zero everywhere except over the 1-by-1 square centered at x = i, y = j. Calling this square Ω i,j , we arrive at < < I[i][j] ← dx dy I(x, y) (16.2)

Ω

i,j

In this case, the desired pixel value is simply the average of the continuous image over the pixel’s square domain. (See Figure 16.2.)

In computer graphics, even the integral of Equation (16.2) is too difﬁcult to exactly compute. Instead, it is approximated by some sum of the form:

1 I[i][j] ← n

n &

I(x k , y k )

(16.3)

k=1 where k indexes some set of locations (x k , y k ) called the sample locations. We call this oversampling. (See Figure 16.3.)

To do oversampling, the renderer ﬁrst produces a “high resolution” color and zbuffer “image”, where we will use the term sample to refer to each of these high resolution pixels. Then, once rasterization is complete, groups of these samples are averaged together, using Equation (16.3), to create the ﬁnal, lower resolution image.

If the sample locations for the high resolution image form a regular, high resolution grid, then this is called super sampling. See Figure 16.6 (bottom) to see the effect

of super sampling using a very high oversampling rate. We can also choose other sampling patterns for the high resolution “image”, as visualized in Figure 16.3. Such less regular patterns can help us avoid systematic errors that can arise when using Equation (16.3) to approximate Equation (16.2). For a more detailed discussion of some of the beneﬁts of such non-regular patterns, see [12, 49].

In OpenGL, the details of the sampling patterns are hardware dependent. They can be changed by API calls, or more easily, by changing settings for the hardware driver in an interactive control panel.

In OpenGL, we can also choose to do multisampling. In this case, OpenGL draws to a “high resolution” color and z-buffer, as in oversampling. During the rasterization of each triangle, “coverage” and z-values are computed at this sample level. But for efﬁciency, the fragment shader is only called only once per ﬁnal resolution pixel. This color data is shared between all of the samples hit by the triangle in a single (ﬁnal resolution) pixel. As above, once rasterization is complete, groups of these high resolution samples are averaged together, using Equation (16.3) to create the, ﬁnal lower resolution, image. Multisampling can be an effective anti-aliasing method, since, without texture mapping, colors tend to vary quite slowly over each triangle, and thus they do not need to be computed at high spatial resolution. See Figure 16.6 (middle) to see the effect of using multisampling when rendering a scene without texture mapping.

To deal with aliasing that occurs during texture mapping, we have the advantage of possessing the texture image in hand at the outset of the rendering process. This leads to specialized techniques such as mip mapping described in Section 18.3. We defer our discussion of texture anti-aliasing and mip mapping until after we have covered the topic of image reconstruction in Chapter 17.

16.3.1 In the Wild

In digital cameras, antialiasing is accomplished by a combination of the spatial integration that happens over the extent of each pixel sensor, as well as by the optical blurring that happens at due to the lens. Some cameras also include additional optical elements speciﬁcally to blur the continuous image data before it is sampled at the sensors. Aliasing can sometimes be seen in digital images of scenes containing certain regular patterns, such as those in tweed jackets.

In human vision, aliasing artifacts are not typically encountered. Most of the antialiasing, at least in the foveal (central) region of vision, is due to the optical blurring of light, which happens well before it hits the receptor cells [78]. The irregular spatial layout of the sensor cells in the retina also helps by effectively providing spatial jitter (randomness) which turns noticeable aliasing into less conspicuous noise.

16.3.2 Antialiased Lines

Lines are ideally one dimensional, and the notion of what it means to draw a properly ﬁltered line is a more complicated question. Smooth line drawing is available in OpenGL, but this is beyond our scope. The interested reader may see [48] and references therein.

