# Solar System Simulation 3D ThreeJS Projection Migaration

## the understanding of threejs space simulation projection
关于各个行星的trajectory，目前的实现对所有的轨道仍然简单处理为正圆形，实际的情形可能并非正圆形。

- 问题1：如何使用OpenGL ES实现圆形轨道的几何体生成和绘制？

### ObitControl类应该是控制3D对象运动的？这个类比较复杂，需要深入理解。

#### 1. OrbitControl的初步理解
OrbitControl其实是camera对象的操作集合，故而需要在构造器中接收一个camera object作为被操作的对象，还有webgl的dom元素用以设置相关事件（鼠标或手势动作）。

#### 2. OrbitControl事件的注册
在OrbitControl构造函数初始化时，就会注册相关的3D对象运动控制事件。并且会初始化camera的当前状态（this.update）。

#### 3. OrbitControl类定义的实现方式
通过匿名函数一次性执行，在函数中定义了Constraint对象（这意味着Constraint函数的访问范围，只能被OrbitControl对象所使用）和THREE.ObjectControls对象（注意是以THREE的子对象方式定义的）。匿名函数执行后，THREE.OrbitControl对象就位且被初始化。


### EllipseCurve类的实现

1. EllipseCurve类的继承结构设计
    基类为Curve，这个基类设计了直线、曲线相关的通用接口。包括获取相应粒度的点集合，计算法线、曲线距离等。
    EllipseCurve类中主要实现了单个椭圆点的生成逻辑
2. CurvePath类的作用
    CurvePath类为curve对象的集合，用于产生复杂曲线。其中包括生成对应曲线几何数据的功能。


## Javascript的面向对象机制
Javascript通过prototype机制实现面向对象编程，与通常的Cpp/Java方式有明显不同。
每个函数都是对象，我们可以首先定义一个函数，然后将其作为一个对象的构造函数。随后再基于这个构造函数生成对象。


## 关于ThreeJS源码的理解
ThreeJS及其实例简直是OpenGL ES的实例宝库。包含了图像渲染各方面的算法实例，且以生动的方式呈现了OpenGL体系的方方面面。实在是学习的不二之选。

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

### PerspectiveCamera类
PerspectiveCamera类确定了透视相机的参数以及相关投射属性（相机属性）的计算，最主要的目标是确定出投射矩阵以及投射矩阵的反转矩阵。

### ThreeJS的渲染引擎
WebGLRender类实现了渲染引擎的主要逻辑，实现了camera，material，geometry子系统的整合逻辑。
ThreeJS的封装层次比较复杂，WebGLRender的个体封装不易于被理解。
WebGLProgram封装了各种shader的组合逻辑。

