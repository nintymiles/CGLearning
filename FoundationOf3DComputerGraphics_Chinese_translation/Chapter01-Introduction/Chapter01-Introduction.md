# 介绍
计算机图形是一个神奇的技术成功故事。基础思路，表达，算法和硬件方式在19世纪60年代和70年代被锻造出来，并且在随后的20年内发展成熟。到90年代中期，计算机图形技术已经相当成熟，但是他们的影响力在某种程度上被限制在“高端”应用，诸如超级计算机上的科学可视化，以及昂贵的飞行模拟器。现在很难相信，但是很多本科毕业的计算机专业的学习甚至对于3D计算机图形是什么都没有概念！

之前的10年见证了计算机图形技术的大规模商业化。每一台现代PC都可以产生高质量的计算机生成的图像，大部分是以视频游戏和虚拟生活环境的形式存在。整个动画工业已经从高端（比如，皮克斯电影）转换到了儿童的日常电视节目中。对于真人实拍的电影，特性领域已经被完全革新；今天的观看者在他们看到最令人吃惊的计算机生成的特效时不会大惊小怪，这完全是期望看到的。

## 1.1 OpenGL

OpenGL began as an API that implemented a very speciﬁc sequence of operations used for doing 3D computer graphics. As the underlying hardware became cheaper, more and more ﬂexibility was put into the hardware and exposed to the user by the OpenGL API. Over time, it has become feasible to fully control certain portions of the graphics computation. The user does this by writing small special purpose programs, called shaders, that are passed to and complied by the API. In OpenGL these shaders are written in a C-like special purpose language called GLSL. As we will describe below, the two main programmable portions are controlled by a vertex shader and a fragment shader. These speciﬁc sections of the computation have been made programmable both because there is great utility in giving the user ﬂexibility in them, but also because this computation can be done using single instruction multiple data (SIMD) parallelism. 以单个几何顶点方式被存储的数据可以独立于其它顶点被处理。同样的，确定一个屏幕像素色彩的计算可以完全独立于其它像素被计算。

In a current OpenGL program, much (but not all) of the actual 3D graphics is done by the shaders that you write, and are no longer really part of the OpenGL API itself. In this sense, OpenGL is more about organizing your data and your shaders, and less about 3D computer graphics. In the rest of this section, we will give an overview of the main processing steps done by OpenGL. But we will also give some high level descriptions of how the various shaders are typically used in these steps to implement 3D computer graphics.

In OpenGL we represent our geometry as a collection of triangles. On the one hand, triangles are simple enough to be processed very efﬁciently by OpenGL, while on the other hand, using collections of many triangles, we can approximate surfaces with complicated shapes (see Figure 1.1). If our computer graphics program uses a more abstract geometric representation, it must ﬁrst be turned into a triangle collection before OpenGL can draw the geometry.



