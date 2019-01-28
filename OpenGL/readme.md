
## Involved Problems
- Through `expr -f hex/oct/bin -- pixel(variable)`,we can print infomation by specified number format in LLDB.
- When you see some negative mouse cursor values,then you must put mouse cursor outside of a glfw window.
- glfwpollevents() and glfwwaitevents() behave very differnetly when the current window is put in background.at the moment,glfwwaitevents() suspends all threads (put them to sleep) until at least one avalable window event is fired while glfwpollevents() just cares about event queues instead available window events (in particular these quickly polled window events didn't really trigger any event callback).
- In OpenGL,when you draw an object just changing its scale (uniformly or non uniformly), the effective way is to keep an scale matrix around the object (dont try to initialize the object with different aspect sizes every time drawing,will be ineffective).
- Workaround for packaging files with command line application. Command line application wraps recources (shaders) by 'copy files' build configuration. (uncheck 'copy only when insalling' option to enable copying during debug phase).
- When you organize your desktop OpenGL application through command line approach, you can't package any of your resource file into a non bundle. So be careful of the shader files path,before start building,specify the right shader path. 
- Fail to add shader source string loading support using GLSL macro.


## To Do List
- Using arcball interface to manuplate cubes rotation around the mixed auxiliary frame. (Done 20190117)
- draw cubes and spheres using RigTForm and Quaternion style instead of the matrix4 style. (Done Ex.21 20190117)

- Done Ex.17 draw two tesellate spheres to replace cubes (20190114)
![Alt](./Screenshots/ex.17.png "Ex.17 Screenshot")

- Done Ex.16 of Foundation of 3D Computer Graphics (20190113)
Use 'O' key to toggle between two cubes.
![Alt](./Screenshots/ex.15-16.png "Ex.16 Screenshot")

- Done Ex.15 of Foundation of 3D Computer Graphics (20190113)
Implement moving two cubes around corresponding auxiliary frames respectively.

- Done making basic enviroment work and draw two cubes by using uniform coordinate offset variable (20190112) 




