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
PPM文件为简单的RGB格式图像存储格式。


```c
	int width, height;
   vector<PackedPixel> pixData;
	ppmRead(ppmFileName.c_str(), width, height, pixData);
        
    
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
若使用三线性过滤器，但是没有设置渐近式纹理，那么获取不到色彩（black）。


