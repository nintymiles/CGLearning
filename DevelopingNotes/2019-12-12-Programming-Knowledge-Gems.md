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

## 关于用glReadPixels从screen上读取像素的痛苦经历（OpenGL 4.1）
一个简单的API调用glReadPixels无法读取像素。
glReadPixels在Mac OS X无法读取screen pixels（default framebuffer）。根本原因是glReadPixels需要framebuffer和在同一个OpenGL context中。
也就是说如果借助glReadPixels进行Picking操作，那么这种方式将无法进行。

## enable_shared_from_this接口的作用？
Constructs a new enable_shared_from_this object. The private std::weak_ptr<T> member is value-initialized.
也就是说允许对象以shared_ptr指针的方式被使用，实现此接口的方法会初始化一个std::weak_ptr<T>成员变量。

## 关于virtual function？
Virtual Functions and Scope
We can now understand why virtual functions must have the same parameter list in the base and derived classes (§ 15.3, p. 605). If the base and derived members took arguments that differed from one another, there would be no way to call the derived version through a reference or pointer to the base class. 

 In C++, dynamic binding happens when a virtual function is called through a reference (or a pointer) to a base class.
 
 C++中，virtual函数是动态绑定方式调用的关键。
 
## Mac中使用VSCode+BasicTex的环境配置
LaTex为文档格式表达语言，被广泛应用于书籍的撰写和排版。

基本LaTex环境的安装

1. BasicTeX􏰘􏳆􏳇 `brew cask install basictex`或者直接下载BasicTeX.pkg包安装，默认安装目录为`\Libarary\Tex`
2. 􏳈􏳉BasicTeX􏲰􏳊􏳋􏰃 `sudo tlmgr update --self`
3. latexmk `sudo tlmgr install latexmk`,这一步骤比较关键，因为BasicTex相比完整的MacTex（4G的臃肿体型）缺少了这个VSCode所需要的关键部件（需要确认）
4. VSCode中安装3个插件LaTexWorkshop，LaTex Language support，LaTex Preview

**使用方式**

安装成功后，VSCode会对.tex后缀的文件进行识别，此时在左侧栏工具条上会出现Tex菜单项，点开后就可以进行LaTex文件的编译，日志查看，预览等动作。
 
## 实现VBO缓存的定时刷新
如果要动态更新VBO数据，通常的方式是使用相同的缓存，但是动态更新缓存中的数据。

```cpp
void upload(){

if (dynamicUsage) {
            // We always call glBufferData with a NULL ptr here, so that OpenGL knows
            // that we're done with old data, and that if the old vbo is in use, it can
            // allocate us a new one without stalling the pipeline
            glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
        }
        else {
            glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        }

}


static void animateVerticesOfSubdivision(){
    vector<VertexPN> vtx;
    int size = (int)g_divisionVtx.size();
    for(int i=0;i<size;i++){
        float scale=sin(CS175_PI/2)*(globalElapseTime/TIME_INTERVAL);
        VertexPN vtxPN = g_divisionVtx[i];
        vtx.push_back(VertexPN(vtxPN.p*scale,vtxPN.n));
    }
    g_subdivisionMesh->upload(&vtx[0], (int)vtx.size());
}
```

