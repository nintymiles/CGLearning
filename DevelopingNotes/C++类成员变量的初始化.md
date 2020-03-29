# C++类成员变量的默认初始化（隐式初始化）过程

## 类成员变量的默认初始化
类成员的默认初始化过程比较复杂，依据类的不同初始化方式和作用范围，其行为也有不同。下面依据常用方式整理一下类成员变量的初始化过程。假设有一个类Object，拥有成员变量number，pointer，name，pname，refname等来考虑初始化过程。

```cpp
class Object {
    private:
        int *ptr;
        int number；
        string name;
        string *pname;
        string &refname;

    public:
        Object() {}
};
```
当我们调用

```cpp
int main() {
    Object obj;
}
```
类的成员变量将会被怎样进行初始化？

这里我们只考虑在栈（stack）上调用这种最常用的方式，因为这种情形默认初始化的程度是最小的，其它方式以这种行为考虑可以很大程度上涵盖。

## 针对不同类型的成员变量初始化方式的总结
类成员变量的默认初始化（隐式初始化）过程完全相同于函数中本地变量的初始化过程。

对于原生类型，int类型或者指针类型等，他们不会被初始化。但是，他们会使用类实例内存地址中任何已经存在的野值作为自己的值，也就是说，在这种情形下，这些类型的类成员变量的值可能包含不可预知的垃圾值。所以原生类型必须要明确的初始化。

针对对象类型，在这种情形下，默认构造器会被调用。

针对引用类型，变量不被初始化是非法的，会收到编译器的警告，编译无法通过。

针对上面的不同情形，Object类默认初始化过程总结如下

```cpp
    int *ptr;  // 包含任意野值
    int number; // 包含任意野值
    string name; // 空string
    string *name; // 包含任意野值
    string &refname; //编译错误
```

> 更多情形请参考[statckoverflow帖子](https://stackoverflow.com/questions/3127454/how-do-c-class-members-get-initialized-if-i-dont-do-it-explicitly)

