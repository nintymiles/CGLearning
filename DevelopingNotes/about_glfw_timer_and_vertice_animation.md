# 在GLFW中实现定时器
GLFW中没有timer（定时器）功能，因为开发者认为针对跨平台特性这会带来额外的复杂性和限制。通常GLFW中使用glfwGetTime函数在特定的刷新率（帧速）下测量时间的方式来实现。

```cpp
static float globalElapseTime;
static const float TIME_INTERVAL = 5.0f;

//apply time interval to globalElapseTime
globalElapseTime = TIME_INTERVAL;
float previous = glfwGetTime();
while(!glfwWindowShouldClose(window) ){
        animateVerticesOfSubdivision();
                        
        float now = glfwGetTime();
        float delta = now - previous;
        previous = now;
            
         // multiple timer (varaible) can be set
         globalElapseTime -= delta;
         if (globalElapseTime <= 0.f){
         		cout << "Timer triggered: " << globalElapseTime << endl;
              
              //reset timer  
              globalElapseTime = TIME_INTERVAL;
          }
          glfwSwapBuffers( window );
            
          glfwPollEvents(); //不管有没有window event，都会立即返回并恢复主线程的运行，保持glfw程序的正常运行，在这种模式下，timer才能正确执行。
            
          //glfwWaitEvents(); //不会立即返回并恢复主线程，必须等待至少一个window event。
 }
 
 glfwTerminate();

```

## 关键的问题
若要timer机制正常工作，必须使用`glfwPollEvents()`调用，如果使用`glfwWaitEvents()`则timer无法正常工作。原因在于glfwPollEvents不管队列中有没有window event都是立即返回并恢复主线程执行。而glfwWaitEvents则必须等待至少有一个window event进入到对列中，然后才会返回并恢复主线程。

