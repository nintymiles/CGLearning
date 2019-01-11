//
//  ViewController.m
//  ArcballProj
//
//  Created by SeanRen on 2019/1/11.
//  Copyright © 2019 SeanRen. All rights reserved.
//

#import "ViewController.h"
#include "NativeTemplate.h"

#include "perfMonitor.h"



@interface ViewController () {
    UITextField *textField;
    OpenGL_Helper::PerfMonitor perfMonitor;
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    //    [_context release];
    //    [super dealloc];
}

- (BOOL) shouldAutorotate
{
    [EAGLContext setCurrentContext:self.context];
    
    float scaleFactor = self.view.layer.contentsScale;
    
    GraphicsResize(self.view.bounds.size.width * scaleFactor, self.view.bounds.size.height * scaleFactor);
    
    return true;
}

////重载函数的方式无法获得60FPS的frame rate，此设置无效
//-(NSInteger)preferredFramesPerSecond{
//    return 60;
//}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;  //默认格式
    
    self.preferredFramesPerSecond = 60;
    
    textField = [[UITextField alloc] initWithFrame:CGRectMake(8, 20, 80, 24)];
    [textField setFont:[UIFont systemFontOfSize:16 weight:UIFontWeightLight]];
    [textField setTextColor:[UIColor cyanColor]];
    [view addSubview:textField];
    
    [self setupGL];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    
    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }
    
    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    //Optional code to demonstrate how can you bind frame buffer and render buffer.
    GLint defaultFBO;
    GLint defaultRBO;
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);
    
    glBindFramebuffer( GL_FRAMEBUFFER, defaultFBO );
    glBindRenderbuffer( GL_RENDERBUFFER, defaultRBO );
    
    GraphicsInit();
    
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        
        TouchEventDown( pos.x, pos.y,touch.tapCount,true );
        }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        TouchEventMove( pos.x, pos.y,touches.count );
        }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch;
    CGPoint pos;
    
    for( touch in touches )
        {
        pos = [ touch locationInView:self.view ];
        
        TouchEventRelease( pos.x, pos.y,touch.tapCount,false );
        }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    GraphicsRender();
    float fps;
    perfMonitor.Update(fps);
    [textField setText:[NSString stringWithFormat:@"FPS %.2f",fps]];
    
}


@end
