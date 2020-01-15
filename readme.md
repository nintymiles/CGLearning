# CG Learning
首先从**Foundation of 3D Computer Graphics**开始学习Computer Graphics（CG）基础知识，这是是一本非常合适的Computer Graphics入门书籍，值得一遍又一遍的阅读。内容深浅适中，解读十分到位。

> 虽然这本教科书的实例代码和作业项目都是基于OpenGL，并且也是基于OpenGL管线进行了相关的原理讲解，但是这本教材中并没有任何OpenGL本身的学习内容。书中涉及的图形技术并没有使用OpenGL中的高级特征，只需要具备基本的现代可编程管线特征的版本即可。这意味着可以从一个精简版本的OpenGL着手（可以完全抛掉OpenGL中那些传统功能性编程的老古董），减少基于OpenGL学习CG技术的初始曲线的陡峭度，比如从OpenGL ES 3.0开始入手学习。本书中所有的实例几乎都可以无缝地移植到OpenGL ES3.0环境中。

## 作业项目（assignment project）
### 一些问题

1. 由于OpenGL的进化，目前Picking已经无法通过在屏幕（默认帧缓存)中通过读取像素的方式实现（正在考虑使用射线相交的方式实现）。  
2. 目前所有的项目已经实现了Mac环境下的运行，部分实现了iOS版本，windows版本正在进行中...
3. 网站所给出的项目模版基于glut和较早期的OpenGL（4.0之前）版本，在实现课后项目时，我将其更换为GLFW版本，并对一些版本不兼容错误做出了改正（比如绘制时必须具备VAO对象）

### 项目情况
| Project | Description | Detail |
| --- | --- | --- | 
| Assignment2 Hello World 3D| 已完成 | [详情](OpenGL/FoundationOfCG/Assignment2/readme.md)  |
| Assignment3 Quaternion and Arcball| 已完成 |  [详情](OpenGL/FoundationOfCG/Assignment3/readme.md)   |
| Assignment4 Hierarchical Transformations and Pickin| 初步完成，Picking部分未完成，需要新方法 |  |
| Assignment5 Keyframe Animation| 初步完成，不少地方需要改进 |   |
| Assignment6 Materials and Bump Mapping| 初步完成，Picking部分未完成 |    |
| Assignment7 Meshes and Subdivision Surfaces| 初步完成，Catmull-clark规则的实现需要改进 |    |
| Assignment8 Furry Animation| 初步完成，Furry效果需要改进 |   |

## 正在进行的3D计算机图形基础（**Foundation of 3D Computer Graphics**）的翻译
已经完成的章节

- [第一章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter01-Introduction.pdf)
- [第二章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter02-Linear.pdf)
- [第三章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter03-Affine.pdf)
- [第四章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter04-Respect.pdf)
- [第五章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter05-Frames-In-Graphics.pdf)
- [第七章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter07-Quaternions.pdf)
- [第八章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter08-BallsTrackAndArc.pdf)
- [第九章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter09-Smooth-Interpolation.pdf)
- [第十章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter10-Projection.pdf)
- [第十一章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter11-Depth.pdf)
- [第十二章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter12-From-Vertex-To-Pixel.pdf)
- [第十三章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter13-Rational-Linear-Interpolation.pdf)
- [第十四章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter14-Materials.pdf)
- [第十五章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter15_texture_mapping.pdf)
- [第十六章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter16-Sampling.pdf)
- [第十七章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter17_Reconstruction.pdf)
- [第十八章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter18_Resampling.pdf)
- [第十九章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter19_Color.pdf)
- [第二十章](FoundationOf3DComputerGraphics_Chinese_translation/PDF/Chapter20-WhatIsRayTracing.pdf)
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


