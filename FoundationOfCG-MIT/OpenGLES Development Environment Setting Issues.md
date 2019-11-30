# 关于在Windows上使用OpenGLES 3.0环境进行开发的设置

## 在Windows台式机上开发OpenGLES的可行性
由于OpenGLES是OpenGL的子集，所以在Windows上进行OpenGLES的开发是可行的。利用GLFW管理本地窗口（EGL）生成，并且管理OpenGLES环境的初始化和加载，借各种OpenGLES模拟器（各移动图形卡厂商和Google都有提供）。我们完全可以将OpenGLES渲染过程的代码在Windows平台上运行。由于OpenGL在Windows环境中已经十分成熟，所以只要Windows PC拥有对应版本的OpenGL图形驱动，那么OpenGLES 3.0渲染代码就可以在其上运行。借助GLFW的跨平台特性，那么也可以实现OpenGLES代码在移动设备之外的运行。

OpenGLES3.0环境在windows上的设置主要涉及这几个方面：GLFW的设置、Glad的设置和OpenGLES模拟器的设置：

### GLFW设置
使用GLFW进行OpenGLES在windows上的模拟开发，这种使用情形不是很普遍，网上的资料也很少。GLFW也没有提供相关使用情形的样例，并且其自身的文档中对OpenGLES环境如何设置提及的不多，只是在网站的特性中提及支持OpenGLES各版本。另外在编译设置的文档中不显眼地提及了特定的宏定义用于指示GLFW如何选定OpenGL的特定版本。

在GLFW使用OpenGLES进行开发，需要设定2个方面：
* 在引入<GLFW/glfw3.h>头文件时，在include之上定义所选定OpenGLES版本的对应的特定宏名称，我们选择OpenGLES3环境，所以，实例如下：

```#define GLFW_INCLUDE_ES3
   #include <GLFW/glfw3.h> 
``` 
关于OpenGLES的头文件控制宏名称清单：

GLFW_INCLUDE_ES1 makes the GLFW header include the OpenGL ES 1.x GLES/gl.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES2 makes the GLFW header include the OpenGL ES 2.0 GLES2/gl2.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES3 makes the GLFW header include the OpenGL ES 3.0 GLES3/gl3.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES31 makes the GLFW header include the OpenGL ES 3.1 GLES3/gl31.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES31 makes the GLFW header include the OpenGL ES 3.2 GLES3/gl32.h header instead of the regular OpenGL header.

* GLFW的初始化设置，确保合适的OpenGLES环境被启动，使用如下的代码启动OpenGLES3.0上下文环境：
`    //设置OpenGLES core profile
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);`


### Glad设置
OpenGLES3.0 API的加载借助Glad库，首先确保当前的Glad实例中包含OpenGLES3.0版本，然后使用如下的
`		if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL context" << std::endl;
			return -1;
		}`

### OpenGLES模拟器的使用
如果使用GLFW环境，那么不需要使用OpenGLES模拟器，一般情况下图形卡所带驱动包含对OpenGLES的支持，使用GLFW和Glad启用对应设置即可。

如果不使用GLFW和Glad库，那么一般情况下需要安装OpenGLES模拟器。比如OpenGLES3.0 Programming Guide书中不使用辅助库，而是借助Cmake直接配置OpenGLES3模拟器配合使用。

这种情形下，一般选择Mali OpenGLES模拟器，可以从ARM公司的网站下载，建议下载可安装的exe版本，因为这种安装版本可以自动设置环境变量，而这在OpenGLES网上相关资料不多的情形下就显得价值巨大，省去很多茫然无措（参考文档不详细、不足）的折腾时间。其余按照网上对OpenGLES3 Programming Guide源码环境的设置即可。

