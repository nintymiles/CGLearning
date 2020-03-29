# C++中struct的使用，以及包含struct定义头文件的交叉使用引起的Use of undeclared identifier错误

## struct在C++中使用时的特点？
首先：struct在C++中已经是头等(first class)类型，在C++中允许抛弃struct关键字使用struct类型，但是C中不行。

其次：在C++中struct被扩展使用，比如struct中还可以定义member function。可以理解C++中的OOP特征为完全架构在struct之上。在C++中struct和class是完全等价的，当然除了在成员变量的默认访问控制属性上的些微差别之外。（其实swift也借鉴了C++中struct的用法。）

总结：在C++中struct类型定义之后，可以像int,float等原生类型一样使用。而在C中，struct关键字在使用struct类型时不可或缺（以typedef方式定义类型别名并不是真正去掉了struct关键字）。在C++中struct类型的使用极为方便灵活。

## struct类型的初始化？
可以对struct类型中的public member variable（struct中member variable的访问属性默认是public）以多种方式初始化：

```c 
struct PlanetInfo{
    string name;
    float distance_AU;
    float diameter;
};

//传统方式，注意必须以定义的成员变量顺序进行
PlanetInfo pi1= {"mercury",0.387,4800}; 

//c++11标准，可乱序初始化
PlanetInfo pi2= {.distance_AU=0.387,.name="mercury",.diameter=4800};

//c++11标准，list-initialization
PlanetInfo pi3  {"mercury",0.387,4800};

```

## 当提供了简单struct类型定义的头文件，在struct使用中为什么有时依然会出现“Use of undeclared identifier”的错误？
在C++中，当定义对象时，有时候在引入一个简单struct类型的头文件时，会出现“Use of undeclared identifier '[StructType]'”的错误。

这通常是由于struct类型所在的头文件无法被正常编译导致的，此时，struct类型定义并没有被编译器加载解析，故而出现“Use of undeclared identifier（使用为定义类型）"的错误。

究其原因，一般情况下这个struct类型通常作为某些复杂对象的附加被定义在相同的头文件中，而这个复杂类型又引用了当前头文件中定义的对象。而这种不同源文件中的交叉引用产生了类似于对象间的循环引用所造成的后果，真实原因其实是编译器的工作方式所引起的。

解决方式，只需要前置struct类型定义，类似于前置class定义一样（其实在C++
中实质就是相同的），然后在实现文件中引入struct类型对应的头文件，就可以避免这种错误。

```
//前置struct类型定义
struct IntersectionData; 
//class IntersectionData; //实际上，在C++中完全可以使用class代替struct关键字，完全是等价的

class Ray{
public:
    vec3 origin;
    vec3 direction;
    
    Ray();
    Ray(vec3 de, vec3 di);

    vector<IntersectionData> intersectSphere(Sphere* sphere);
    ... ...
```


