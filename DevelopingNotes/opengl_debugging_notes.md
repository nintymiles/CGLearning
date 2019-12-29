# OpenGL/OpenGLES中的调试问题
在OpenGL/OpenGLES的大部分版本调试错误是比较痛苦的，直到最近的版本，OpenGL才具备了较高效的错误处理机制。下面对这两种方式简要说明

## `glGetError()`的执行机制
传统OpenGL的错误处理模型是，最初的错误编码状态为GL_NO_ERROR，当某个API（命令）没有正确执行时会注册错误编码（error code），但是这个错误编码只有被使用`glGetError()`查询后才能被设置为GL_NO_ERROR，而只有当前的错误编码被这样设置后，后续API的错误才能被注册。

## 困扰的地方
OpenGL/OpenGLES的大部分版本中调试错误只有`glGetError()`一种途径。然而这种方式极为无效。通常情况下，OpenGL并不建议针对每个API调用都检测是否产生异常，这意味着当我们想要探测某个API是否正常执行时，我们放置在这个API之后的`glGetError()`输出并不是这个API执行的错误信息。

## 基于`glGetError()`的解决方法
如果要找到真正的错误元凶，这意味着我们大概有两种方式可以解决问题。这两种方式其实没有实质区别，只是一种策略选择。

1. 在每个API调用后面使用`glGetError()`，根据全覆盖的debug信息定位出错的API。
2. 选择两个可能的API点，确定其之前和之后的API没有出错（当然还是用`glGetError()`确定）。然后采取类似二分算法使用`glGetError()`从两段不断向中间靠拢，知道定位到具体的API

## OpenGL中的现代调试方法
直到OpenGL4.3（OpenGLES3.1）版本，OpenGL中才加入了现代调试方法（modern method for debugging）。这种方式通过注册debugCallback函数由OpenGL主动返回API调用的错误信息。很明显这种机制解决了`glGetError()`给人所带来的困扰。

具体应用方式，首先生成一个debug context，然后启用debug output，随后注册回调函数，最后开启相应的调试信息输出目标、级别等。

```c
glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);  //生成debug context
...

glEnable(GL_DEBUG_OUTPUT);  //启用debug output
...

glDebugMessageCallback( debugCallback, NULL ); //注册callback function
...

glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);    //Enable all messages, all sources, all levels, and all IDs:
...

```

