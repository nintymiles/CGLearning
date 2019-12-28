# 采样（Sampling）
It is now time for us to focus a bit more on how we treat images. There are really two notions of images that we have implicitly been using so far, discrete (also called, digital) and continuous. In these chapters we will investigate this dichotomy more closely, and discuss the appropriate ways to move back and forth between the discrete and continuous model.

In this chapter, we will focus on variety of visual artifacts that can arise when creating a digital image in computer graphics. These artifacts include distracting jagged patterns at triangle boundaries, and go by the name: aliasing artifacts. Aliasing artifacts can occur when, in some sense, there is too much visual complexity to ﬁt in a single pixel. In this case, when we try to determine the pixel’s color value by looking at only a single point location in the image domain, we can get a less than optimal result. As we will see, these artifacts can be mitigated by averaging together the colors over many sample locations within the pixel’s square. This process is known as anti-aliasing.

## 16.1 Two Models

A continuous image, I(x w , y w ), is a bivariate function. As discussed in Section 12.3, the domain, Ω = [−.5..W − .5] × [−.5..H − .5], of the function are real valued 2D window coordinates [x w , y w ] t . (In the rest of this chapter and the next, we drop the “w” subscript unless needed.) The range of the function is a color space, for which we use an RGB (linear) color space.

A discrete image I[i][j] is a two dimensional array of color values. Each such array entry is called a pixel. The array’s size is W in width by H in height, thus i is an integer in [0..W − 1], j is an integer in [0..H − 1]. We associate each pair of integers i,j, with the continuous image coordinates x w = i and y w = j, i.e., real coordinates that happen to have integer values. Each color value is a triplet of scalars representing a color in a color space. Again, we use an RGB (linear) color space. Finally, each of the R, G, and B coordinates is represented in some numerical format.

In OpenGL, we describe a scene using shaded triangles in 3D. Under projection, these map to colored triangles in the continuous 2D image domain, and thus deﬁne a continuous image. Ultimately we wish to display this image on a monitor, or save it in an image ﬁle. As such, we need to know how to transform the continuous image into a discrete one.

## 16.2 问题（The Problem）
从连续图像到具体图像转换的最简单和最直观的方式为借助点采样，也就是说，要获得像素$i,j$的值，我们采样位于整数值化的域地址处的连续图像函数：
$$ I[i][j] \leftarrow I(i, j)$$

可以证实存在很多点采样导致的不想看到的失真情形。例如，考虑一个由黑和白三角形组成的场景。当点采样图像时，在接近黑白三角形边缘的地方，图像会显示出某种阶梯状的图案；这种失真被称为“锯齿”。（参考图示$\text{Figure 16.6}$）。在动画过程中，某些像素值会突然从黑变到白，同时锯齿状的边缘图案会显示为爬行的样子。这被称作“爬行的锯齿（the crawing jaggies）”。

当处理非常小的三角形时，我们还会得到另外的糟糕失真情形。假如我们正在渲染一张由一群斑马组成的图片（或者是一张正在远离观察者的棋盘上的小的黑白正方形）。假设一只斑马在背景中远离，很远以至于其只覆盖一个单一像素。那个像素的色彩会是什么？如果这种点采样碰巧落在一个黑色三角形上，那么像素会是黑的，如果碰巧落在一个白色三角形上，那么像素会是白的。（参考图示$\text{Figure 16.1}$。）如果我们运气不好，最终的像素色彩会组成某种令人烦恼的“云纹（moire）”图案，就如图示$\text{16.6}$中所示。在动画过程中，如果斑马移动，这些像素将会呈现黑白变换的模式。这种失真被称为“闪烁（flickering）”。假如一个细节高度清晰的纹理被应用在某个屏幕上的小区域，那么在纹理映射中也会出现这种失真。（参考图示$\text{Figure 18.3}$）。

当在一个小区域连续图像有很多细节，那么这些类型的错误就会出现。在斑马例子中，连续图像在几个像素的空间中就包含了整群斑马（的细节）。对于锯齿三角形例子，连续图像具有可以立即过渡的精确锋利的边缘，而不是渐变的边缘。处于技术原因，这些种问题被统称为锯齿（aliasing）。

16.3 解决方案（The Solution）

When there is a lot of detail in a small region, it is unlikely that we can maintain all of the information in our discrete representation. But by taking only a single sample to determine the pixel’s color we are making matters even worse for ourselves, possibly just picking a meaningless value. It seems sensible that we would be better off setting the pixel value using some kind of average value over some appropriate region. In the zebra example, perhaps we would be better just setting the pixel to gray.

There are a variety of ways to mathematically model this problem and look for an optimal solution. For example, using “Fourier analysis”, one could look at which “frequencies” are representable with a certain number of pixels and how to best deal with the unrepresentable frequencies. For such a discussion, see [6]. From an optimization point of view, one could try to minimize the difference between the original continuous image with the light pattern that is ultimately displayed on the screen. (On the screen, each pixel is drawn with a ﬁnite extent and thus we can think of the light pattern on the screen as a synthesized continuous image.)

From these various points of view, which we will not pursue here in detail, it turns out that it is best to set the pixel value using an expression of the form:

< < I[i][j] ← dx dy I(x, y)F i,j (x, y) (16.1)

Ω

where F i,j (x, y) is some function that tells us how strongly the continuous image value at [x, y] t should inﬂuence the pixel value i,j. In this setting, the function F i,j (x, y) is called a ﬁlter.

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

