# Solar System Simulation 3D ThreeJS Projection Migaration

## the understanding of threejs space simulation projection
关于各个行星的trajectory，目前的实现对所有的轨道仍然简单处理为正圆形，实际的情形可能并非正圆形。

- 问题1：如何使用OpenGL ES实现圆形轨道的几何体生成和绘制？

### ObitControl类应该是控制3D对象运动的？这个类比较复杂，需要深入理解。



## 关于ThreeJS源码的理解
ThreeJS及其实例简直是OpenGL ES的实例宝库，包含了各种例子。

目前的想法是对关键的例子进行剖析理解，然后再移植回到OpenGL中一些

### ThreeJS源码的materials目录
material基类封装了材料的基本属性信息，那么这个目录下的各种material扩展类其实应该是定义了不同材料的具体属性信息。

material组件其实和多数的cg教科书没有区别，其中包含shader（shading计算逻辑）、材料属性等的封装。

ThreeJS中将material和geometry匹配组合在一个mesh对象中。

#### ShaderMaterial类
这个类实现了通用着色器的使用，各种定制的算法，基本的WebGL shading算法可以从这个途径进行。

### WebGLRender类
主要的渲染逻辑都在这里，当定义了materials和geometry，其都在此处被组合从而进行渲染。

### 关于ThreeJS的ellipsecurve功能
此功能用于绘制特定形状（椭圆形）的曲线，包含xRadians和yRadians两个参数（若xRadians==yRadians，则为圆形）。此处这个类根据这些参数生成了对应的几何形状(线条)，并且可以生成几何对象集合。