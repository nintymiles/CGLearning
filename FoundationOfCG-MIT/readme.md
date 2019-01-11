#Some involved problems
- ViewController resolution problem 2019/01/11
    When create OpenGL ES project to be run only on iPhone device, if you want viewcontroller to be fit for iPhone device resolution,you need to remove viewcontroller properties "use trait variation" from right pane.
- Bundle resources cant be found 2019/01/11
    When you want to bundle resources (shader files etc.) into packages, You just import files with option "create groups" but instead "create file references".
- If you want to get 60 FPS on iOS (verified on iOS 9/12), you just set 'self.preferredFramesPerSecond = 60;' on view controller.
- If you want to receive multitouch event, you need to enable "Multiple Touch" (default disabled)

#To do list
1. how to draw half translucent objects in OpenGL? done(20190111)

	Just enable blend Fragment Operation
	```
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_SRC_MINUS_ALPHA);
	```
2. Applying arcball interface on this project??? done(20190111).
