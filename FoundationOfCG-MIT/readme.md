# Some involved problems
- ViewController resolution problem -2019/01/11

    When create OpenGL ES project to be run only on iPhone device, if you want viewcontroller to be fit for iPhone device resolution,you need to remove viewcontroller properties "use trait variation" from right pane.
- Bundle resources cant be found -2019/01/11

    When you want to bundle resources (shader files etc.) into packages, You just import files with option "create groups" but instead "create file references".
- If you want to get 60 FPS on iOS (verified on iOS 9/12), you just set 'self.preferredFramesPerSecond = 60;' on view controller. -2019/01/12
- If you want to receive multitouch event, you need to enable "Multiple Touch" (default disabled) -2019/01/12
- Pay attention to window coordinates (input from touch event/mouse),which need to be converted to OpenGL window coordinates when using in OpenGL context. -2019/01/12

# To do list

0. Migrate assignment 2 (helloworld3d) from Desktop OpenGL to OpenGL ES setting
1. how to draw half translucent objects in OpenGL? done(20190111)

	Just enable blend Fragment Operation and initialize a RGBA framebuffer
	
	```
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_SRC_MINUS_ALPHA);
	```
2. Applying arcball interface on arcball project??? done(20190111)

3. Make sky camera aroud the origin of arcball sphere? done(20190112)

	make eye frame rotating around sphere origin by quaternion created arcball interface.
        
4. How to conditionally import OpenGL and OpenGLES library for Mac and iOS? done(20190112)

	Note: TARGET_OS_MAC includes any apple OS,TARGET_OS_IPHONE includes any device/simulator run on iOS,TARGET_OS_OSX only represents desktop Mac OS X.
            
        
	```
	#if defined(__APPLE__) && defined(__MACH__)
	#include <TargetConditionals.h>
	#if TARGET_OS_OSX
	#include <GLFW/glfw3.h>
	#elif TARGET_OS_IPHONE
	#include <OpenGLES/ES3/gl.h>
	#include <OpenGLES/ES3/glext.h>
	#endif
	#endif
	```
    
5. Draw two cubes using same vetex data by a x coordinate offset uniform variable on AssignmentsProj. done(20190113)
        
        
	```
	...
	uniform float uXCoordOffset;
	    
	in vec3 aPosition;    
	void main() {
	...
	gl_Position = uProjMatrix * aPosition;
	gl_Position.x += uXCoordOffset;
	}
	```        
        


