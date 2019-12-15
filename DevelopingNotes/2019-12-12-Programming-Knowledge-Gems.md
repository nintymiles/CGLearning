# 编程知识拾粹

## shared_ptr的用法
shared_ptr用于保证对象的高效使用，避免内存的无效占用。
其原理是将类对象有效管理起来，以指针的形式提供高效访问。
其实质就是指针，在使用shared_ptr实例时，将其**当作一般的指针**使用即可。

## OpenGL中VBO内容的验证
可通过mapperBuffer的方式验证VertexBuffer中的数据是否正确。

> 通常用这种映射可将缓存中的所有数据都读取出来，读出的数据量可以通过定义的容器数组尺寸来控制。
        
```cpp
 // 通过mapperBuffer的方式验证VertexBuffer中的数据并没有问题
         
 GLfloat *vtxMappedBuf;
 
 GLfloat vtxBuf[56] ;
 
 GLint bufferSize = pvw.vb->length() * (pvw.vb>getVertexFormat().getVertexSize());
 vtxMappedBuf = (GLfloat*) glMapBufferRange ( GL_ARRAY_BUFFER, 0, (unsigned int)bufferSize , GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
 if ( vtxMappedBuf == NULL ) {
 printf( "Error mapping vertex buffer object." );
 }
 
 memcpy (  vtxBuf,vtxMappedBuf, bufferSize );
     
```

## GLError（0x502）问题的发生
猜想，此项目中所出现的0x502问题主要是由于glEnable/DiableVertexArray()方法和glVertexAttributePointer不在一个Vertex Array Object的作用域范围所导致？（抑或是不在一个线程所导致？？？）
总之，Vertex Array Object在当前OpenGL版本的强制使用可能是问题的关键。

> 用glGetError可以进行盲人摸象式的OpenGL程序调试，以类似C中printf的方式。虽然无效率，但是对于OpenGL来说也是一种有效的方式。（OpenGL没有有效的可跟踪式的调试解决方案）。

## 对OpenGL中program进行验证的程序
对program验证有一定的提示作用，但是不准确。比如，在vertex array object问题修正后依然验证有问题，但是这确实提示了还有更深入的问题，待后续继续验证。

> Validation Failed: No vertex array object bound.

```cpp
    glValidateProgram(programDesc_->program);

    GLint validated = false;
    // Check the validate status
    glGetProgramiv ( programDesc_->program, GL_VALIDATE_STATUS, &validated );

    if(!validated){
        printProgramInfoLog(programDesc_->program);
    }
    checkGlError("after validating Program");
```

## 关于PPM文件格式的加载读取中的注意事项
PPM文件为简单的RGB格式图像存储格式。以byte格式存储，一般读取后都以字符型数组形式存放。


```c
	int width, height;
   vector<PackedPixel> pixData;
	ppmRead(ppmFileName.c_str(), width, height, pixData);
        
   //用数组读出vetor数据，方便检测  
   GLubyte pixels[width * height * 3];
   int a=0;
   for(PackedPixel pp:pixData){
       pixels[a] = pp.r;
       pixels[a+1] = pp.g;
       pixels[a+2] = pp.b;

       a += 3;
   }
```

## 关于2D texture纹理的加载
若使用三线性过滤器，但是没有设置渐近式纹理，那么将获取不到色彩（black）。

## extern关键字的使用
在C++中，如何定义一个跨源文件的全局可用的变量，方式就是通过extern关键字。但是extern只是执行一种声明，声明这个变量可以分开编译，也就是如果有全局变量，如果要在超过一个源文件中使用，首先要做的是对这个变量进行extern declaration（注意是声明）。随后可以对这个变量声明初始化。

### 使用规则
使用时，extern变量声明和变量定义最好各自独立对待，而不要混合使用。

这是一个很容易混淆的地方，变量声明和变量定义有明确的区分，虽然变量定义可以理解为在变量声明的基础上进行，但是变量定义意味着变量声明和存储及值初始化同时进行，是一个整体，不能分开。

也就是说变量定义不能先使用单纯声明方式，然后对变量名称直接进行赋值操作。这会导致编译错误，因为变量没有寻址内存，必须对变量进行定义。

```cpp
extern int j; //加了extern关键字意味着只声明不定义，这个变量声明不能直接赋值,比如直接以j=0使用。
int j; //这是变量定义，虽然没有明确初始化。就是说在声明的同时，也进行了内存寻址和值的初始化，虽然这是隐含动作。
```

但是extern关键字也支持合并方式直接做变量定义，也就是既有分开编译的声明支持，也有在本处文件中的变量定义。

```cpp
extern int j = 0;
```

> 声明和定义的区别：
>> 一个声明只是确定变量的名称和类型。而一个定义则是在声明的基础上进一步关联了存储和初始值


