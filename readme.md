# CG Learning
首先从**Foundation of 3D Computer Graphics**开始学习Computer Graphics（CG）基础知识，这是是一本非常合适的Computer Graphics入门书籍，值得一遍又一遍的阅读。内容深浅适中，解读十分到位。

> 虽然这本教科书的实例代码和作业项目都是基于OpenGL，并且也是基于OpenGL管线进行了相关的原理讲解，但是这本教材中并没有任何OpenGL本身的学习内容。书中涉及的图形技术并没有使用OpenGL中的高级特征，只需要具备基本的现代可编程管线特征的版本即可。这意味着可以从一个精简版本的OpenGL着手（可以完全抛掉OpenGL中那些传统功能性编程的老古董），减少基于OpenGL学习CG技术的初始曲线的陡峭度，比如从OpenGL ES 3.0开始入手学习。本书中所有的实例几乎都可以无缝地移植到OpenGL ES3.0环境中。

## 已经完成的作业项目（assignment project）
除了picking由于OpenGL的进化，目前已经无法通过在屏幕（默认帧缓存)中通过读取像素的方式实现外（正在考虑使用射线相交的方式实现），assignment1-6已经完成，assignment7-8也在实现中，但由于catmull-clark和shell渲染算法还在学习中，目前只是实现基础材料的渲染。

## 正在进行的**Foundation of 3D Computer Graphics**的翻译项目
已经完成的章节

- [第二章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter02-Linear.pdf)
- [第三章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter03-Affine.pdf)
- [第四章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter04-Respect.pdf)
- [第五章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter05-Frames-In-Graphics.pdf)
- [第八章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter08-BallsTrackAndArc.pdf)
- [第九章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter09-Smooth-Interpolation.pdf)
- [第十章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter10-Projection.pdf)
- 第十一章 进行中...
- [第十二章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter12-From-Vertex-To-Pixel.pdf)
- [第十三章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter13-Rational-Linear-Interpolation.pdf)
- [第十四章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter14-Materials.pdf)
- [第十五章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter15_texture_mapping.pdf)
- [第十九章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter19_Color.pdf)
- [PDF文件目录](FoundationOf3DComputerGraphics_Chinese_translation/PDF/)

## 总结文字
### OpenGL

- [凹凸映射（bump mapping）中法线映射（normal map）纹理数据的解读和使用](DevelopingNotes/the-normal-map-of-bump-mapping-2019-12-19.md)
- [GLSL中，如何对matrix类型进行初始化](DevelopingNotes/Matrix-Initialization-In-GLSL-2019-12-20.md)
- [在OpenGL中用Projection Matrix 分别左乘和右乘均匀或不均匀Scale Matrix的效果](DevelopingNotes/About-ProjectionMatrix-Left-Right-Multiplied-By-Diff-Scale-Matrix.md)
- [OpenGL/OpenGLES中的调试问题](DevelopingNotes/opengl_debugging_notes.md)
- [在GLFW中实现定时器](DevelopingNotes/about_glfw_timer_and_vertice_animation.md)
- [关于在Windows上使用OpenGLES 3.0进行开发的环境设置](DevelopingNotes/OpenGLES_Development_Environment_Setting_Issues.md)

### 其它
- [extern关键字的使用](cpp-the-use-of-extern-keyword-2019-12-19.md)

## File Structures

- BaseFramework为通用代码目录。
- 针对OpenGL ES和OpenGL平台的assignments项目代码分别位于FoundationOfCG-MIT和OpenGL目录
- 本书的中文翻译位于FoundationOf3DComputerGraphics_Chinese_translation目录
- 学习及开发过程中的记录文字位于DevelopingNotes目录


