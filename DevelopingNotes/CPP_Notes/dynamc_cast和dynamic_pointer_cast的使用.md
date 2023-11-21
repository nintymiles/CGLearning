# `dynamic_cast`和`dynamic_pointer_cast`的区别

## 基本区别：

`dynamic_cast`是语言关键字。用于执行从基类型到派生类型的转换，基类型要求至少有一个虚函数（virtual funciton），也就是说主要针对类指针和引用这些动态类型。

这个操作符实际上只是检查引用或者指针所绑定对象的动态类型，如果和要转换的动态类型相同，则转换完成，否则返回空指针（针对指针）或者抛出异常（针对引用）。

`dynamic_pointer_cast`不是语言关键字，在标准库`<memory>`中定义，位于std命名空间中，专用于智能指针shared_ptr的转换。可以理解为智能指针领域的`dynamic_cast`操作符。

总结一下：它们的基本区别，就是`dynamci_cast`用于裸指针和引用等动态类型的转型，而`dynamic_pointer_cast`主要用于智能指针的转型。

### 例子：

```cpp
//注意:dynamic_pointer_cast的模版参数只需要指定到类名即可
std::shared_ptr<ShaderMaterial> shaderMaterial = std::dynamic_pointer_cast<ShaderMaterial>(boxMesh->material);
...

//注意:dynamic_cast操作符需要指定到指针或者引用级别
Texture* Texture::Create( GLuint type, std::vector<std::string>& texFiles) {
    if (type == GL_TEXTURE_2D) {
        return dynamic_cast<Texture*>(new Texture2d(texFiles[0]));
    } else if (type == GL_TEXTURE_CUBE_MAP) {
        return dynamic_cast<Texture*>(new TextureCubemap(texFiles));
    }
    
    return nullptr;
}
```

## 共同点：

他们的共同点是都用于继承体系的向下转型。

`dynamic_cast` 是一种RTTI机制操作符。

另外：`dynamic_cast`执行动态类型向下转型动作时，性能并不差，具体参考，具体参考https://zhuanlan.zhihu.com/p/580330672





dynamic_cast Operator that performs a checked cast from a base type to a derived type. When the base type has at least one virtual function, the operator checks the dynamic type of the object to which the reference or pointer is bound. If the object type is the same as the type of the cast (or a type derived from that type), then the cast is done. Otherwise, a zero pointer is returned for a pointer cast, or an exception is thrown for a cast to a reference type.

https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast

Creates a new instance of [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr) whose stored pointer is obtained from r's stored pointer using a cast expression.

If r is empty, so is the new `shared_ptr` (but its stored pointer is not necessarily null). Otherwise, the new `shared_ptr` will share ownership with the initial value of r, except that it is empty if the `dynamic_cast` performed by `dynamic_pointer_cast` returns a null pointer.

Let `Y` be typename [std::shared_ptr](http://en.cppreference.com/w/cpp/memory/shared_ptr)<T>::element_type, then the resulting [std::shared_ptr](https://en.cppreference.com/w/cpp/memory/shared_ptr)'s stored pointer will be obtained by evaluating, respectively:

1,2) static_cast<Y*>(r.get())

3,4) dynamic_cast<Y*>(r.get()). If the result of the `dynamic_cast` is a null pointer value, the returned `shared_ptr` will be empty.

5,6) const_cast<Y*>(r.get())

7,8) reinterpret_cast<Y*>(r.get())

The behavior of these functions is undefined unless the corresponding cast from `U*` to `T*` is well formed:

1,2) The behavior is undefined unless static_cast<T*>((U*)nullptr) is well formed.

3,4) The behavior is undefined unless dynamic_cast<T*>((U*)nullptr) is well formed.

5,6) The behavior is undefined unless const_cast<T*>((U*)nullptr) is well formed.

7,8) The behavior is undefined unless reinterpret_cast<T*>((U*)nullptr) is well formed.