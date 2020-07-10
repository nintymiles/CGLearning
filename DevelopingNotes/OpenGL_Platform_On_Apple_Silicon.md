# OpenGL/ES能否运行在Apple Silicon（arm mac）之上？
众所周知，苹果已经在自家的操作系统平台上开始了废弃OpenGL的过程。OpenGL相关的SDK已经被标记为deprecated，并且不再随着操作系统升级被更新。在刚开始的WWDC20上，苹果宣布了Mac OS逐步迁移到自己的硬件CPU/GPU之上。

苹果在[平台转换文档](https://developer.apple.com/documentation/xcode/porting_your_macos_apps_to_apple_silicon)详细列出了现有的MacOS Apps如何转换到apple silicon平台上。其中关于GPU部分明确指出了OpenGL和OpenCL在apple silicon平台上仍然被支持（虽然早已处于deprecated状态），也就是说苹果目前还不能干脆的将OpenGL从自家平台清除，可能因为这对应了很多基于OpenGL的应用。

## google的angle项目
angle项目为google在各操作系统平台的主流图形API（OpenGL/OpenGLES/Direct3D/Vulkan/Metal）之上使用C++封装的OpenGL ES实现。

