#Some involved problems
- ViewController resolution problem 2019/01/11
    When create OpenGL ES project to be run only on iPhone device, if you want viewcontroller to be fit for iPhone device resolution,you need to remove viewcontroller properties "use trait variation" from right pane.
- Bundle resources cant be found 2019/01/11
    When you want to bundle resources (shader files etc.) into packages, You just import files with option "create groups" not "create file references".
- You can't get 60 FPS on iOS 12, but you can get it on iOS 9

#To do list
1. how to draw half translucent objects in OpenGL?

	Just enable blend Fragment Operation
	```
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_SRC_MINUS_ALPHA);
	```
2. Applying arball interface on this project??? do it tonight.
