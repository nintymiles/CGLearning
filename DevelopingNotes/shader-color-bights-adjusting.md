# CG中如何调整色彩亮度

## shader中色彩亮度的调整方法
如果要调节一个色彩，那么这个行为可以对应到RGB坐标的缩放操作上。一个色彩若要到达其最大亮度，则通过缩放其RGB值，使得至少其中一个值达到最大值1（实际上可以大于1）。

在shader中如果要使得一个色彩变得更加明亮，则使用一个大于1的标量因子对RGB坐标进行整体放大。然后使用函数将最大值限制到1，最小值限制到0，代码例子如下：


```glsl
//将漫射色彩调亮50%，注意clamp函数将色彩各部件限制到有效范围
colorDiffuse = clamp(vMaterialDiffuse * 1.5,
                         vec4(0.0, 0.0, 0.0, 0.0),
                         vec4(1.0, 1.0, 1.0, 1.0));
```

> 注：如果RGB坐标放大的倍数过大，则所有部件值都达到1，整个色彩就显示为白色。这种情形下，色调（color hue）已经变化。如果要较好保持色调，那么放大的倍数应该仅能让其中的最大值刚好到达1。


## VAO的正确用法
vao在OpenGL中用来设置vbo，ubo缓存绑定和缓存指针的批量设定，避免每次都发送很多个api调用，减少api调用开销。

一般来说vao在model启动时候就要设置好，在rendering时直接调用。

## Cube texture的渲染关注点
首先，要正确计算出reflected vector，而计算反射矢量使用world坐标还是eye坐标，其实影响不大。
其次，注意reflected vector的计算的方式，可以使用built-in函数relfect(),也可以自己实现反射方程式。若使用内置函数，注意入射矢量参数和平常所使用的反射光学模型中的矢量方向相反。
再次，使用texture函数直接使用relfected vector作为参数获取对应的环境色彩，而如果cube map 中的texture正确设置，refelected vector不需要任何处理，可直接使用。（需要注意的是，bmp图像读取时可能会出现flip-vertical的情形，使用stbi加载时需要flip。）
再次，设置cube map texture时，每个轴向上的texture要根据其对应坐标系的方位来设定。

