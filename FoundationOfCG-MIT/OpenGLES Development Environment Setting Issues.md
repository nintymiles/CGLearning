# 关于在Windows上使用OpenGLES 3.0进行开发的环境设置

## 在Windows台式机上开发OpenGLES的可行性
由于OpenGLES是OpenGL的子集，所以在Windows上直接进行OpenGLES的开发是可行的。利用GLFW就可以实现这种目的。GLFW是用于OpenGL、OpenGLES和Vulkan应用开发的跨平台库，提供了简洁、平台独立的API，用于管理本地窗口（EGL\EAGL）生成、OpenGL/OpenGLES环境上下文和渲染表面的初始化和生成、以及负责读取输入并处理事件等。

不借助GLFW等第三方库，那么在Windows上直接使用EGL和OpenGLES API并配合OpenGLES模拟器（各移动图形卡厂商和Google都有提供）也是一种较好的选择。

总之，我们完全可以将OpenGLES渲染过程的代码在Windows平台上运行。由于OpenGL在Windows（PC）环境中已经十分成熟，所以只要Windows（PC）上拥有对应版本的OpenGL图形驱动，OpenGLES渲染代码就基本可以在其上运行。利用GLFW的跨平台特性，那么也可以实现OpenGLES代码跨移动和PC平台运行。

OpenGLES 3.0开发环境在windows上的设置主要可分为两种：
1. 使用GLFW，并配合Glad直接在Windows平台上进行OpenGLES开发。
2. 不借助GLFW，以裸奔形式借助OpenGLES模拟器在Windows上进行开发。

### 使用GLFW在Windows上开发OpenGLES的环境设置
使用GLFW进行OpenGLES在windows上的模拟开发，这种使用情形不是很多，网上的资料也很少。GLFW也没有提供相关使用情形的样例，并且其自身的文档中对OpenGLES环境如何设置提及的并不多，只是在GLFW的定义中提及支持OpenGLES各版本。另外在编译设置的文档中不显眼地提及了特定的宏定义用于指示GLFW如何选定OpenGLES的特定版本的头文件引入。

要使用GLFW配合Glad的使用进行OpenGLES 3.0开发，需要2个方面的设置：
* 在引入<GLFW/glfw3.h>头文件时，在include之上定义特定的GLFW头文件选择宏（GLFW header option Macro）名称，用以确保GLFW正确引入所选定OpenGLES对应版本的头文件，比如意欲使用OpenGLES3.0版本，则代码设置如下：
```
	#define GLFW_INCLUDE_ES3
	#include <GLFW/glfw3.h> 
``` 

关于OpenGLES的头文件控制宏名称清单：
```
GLFW_INCLUDE_ES1 makes the GLFW header include the OpenGL ES 1.x GLES/gl.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES2 makes the GLFW header include the OpenGL ES 2.0 GLES2/gl2.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES3 makes the GLFW header include the OpenGL ES 3.0 GLES3/gl3.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES31 makes the GLFW header include the OpenGL ES 3.1 GLES3/gl31.h header instead of the regular OpenGL header.

GLFW_INCLUDE_ES31 makes the GLFW header include the OpenGL ES 3.2 GLES3/gl32.h header instead of the regular OpenGL header.
```

* GLFW的初始化设置，确保OpenGLES 3.0版本的上下文环境被启动，则使用如下的代码：
```
	//设置OpenGLES 3.0 profile
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
```


* Glad加载对应OpenGLES 3.0 API的设置
OpenGLES 3.0 API的加载可以借助Glad库（保证正确上下文环境的OpenGLES API函数地址），首先请确保当前所使用的Glad设置中包含OpenGLES 3.0版本，然后使用如下的API调用即可：
```		
		//这个调用的含义是根据当前GLFW所初始化的OpenGL/ES环境初始化对应的API函数
		if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize OpenGL/ES context" << std::endl;
			return -1;
		}
```

### 裸奔（不借助GLFW）进行OpenGLES 3.0开发的环境设置（一般需要借助OpenGLES模拟器）
如果使用GLFW环境，那么完全不需要使用OpenGLES模拟器，一般情况下PC图形卡的OpenGL驱动包含对OpenGLES的支持，使用GLFW和Glad启用对应设置即可。

如果不使用GLFW和Glad库，那么一般情况下需要安装OpenGLES模拟器。比如**OpenGLES 3.0 Programming Guide**书中就不借助第三方辅助库，而是使用Cmake直接配置OpenGLES模拟器配合使用。

这种情形下，一般可选择Mali OpenGLES模拟器，可以从ARM公司的网站下载。建议下载可安装的exe版本，因为这种安装版本可以自动设置环境变量，而这在OpenGLES网上相关资料不多的情形下就显得价值巨大，省去很多茫然无措（参考文档不详细、资料不足）的折腾时间。其余按照网上对**OpenGLES 3.0 Programming Guide**源码环境的设置即可。



