# Color

##19.1 简单生理模型（bio-physical model）
###19.1.1 颜色空间的映射
At this point, we already have enough information to roughly map out the color cone.
当前的这个时点（或到当前时刻），我们已经有足够信息来大略映射出颜色锥体（color cone ？颜色锥体的形状）。

Scales of vectors in the cone correspond to brightness changes in our perceived color sensation, and thus are not very interesting. (Though, when we dim an orange light, we actually perceive the color brown.) Thus it is convenient to normalize our color diagram by scale, obtaining a 2D drawing (see Figure 19.2). In this diagram, we have started with the slice of the cone from Figure 19.7. All displayed colors have then been scaled so that one of R, G, or B is at full strength. We cannot display colors outside of the drawn triangle using positive combinations of the R G and B display elements of a monitor. We say that such colors lay outside of the gamut of our display.
颜色锥体（color cone）中矢量（vectors）的缩放对应于我们可察觉到的颜色感知中的亮度变化，那么从而这种行为就不是很有趣。（虽然，在我们讲一束桔（橙）色的光模糊后，我们实际会感知到棕色。）因而通过缩放（scale？）来标准化颜色图是方便的，从而获得一个2D图示（2D drawing）（如图19.2）。在本图示中，我们从来自图19.7中的颜色锥体的切片开始。所有被展示的颜色被缩放一遍R，G或者B中的一个在完全的强度上。以显示器R、G和B显示元件（phosphor）的正线性组合，我们不能够展示出图示中被绘制的三角形范围之外的色彩。我们讲这样的色彩位于显示器展示范围之外。

Colors along the boundary of the cone are vivid and are perceived as “saturated”. Starting from the L axis, and moving along the curved portion, we move along the rainbow colors from red to green to violet. These colors can only be achieved by pure beams. Additionally the color cone’s boundary has a planar wedge (a line segment in the 2D ﬁgure). The colors on this wedge are the pinks and purples. They do not appear in the rainbow and can only be achieved by appropriately combining beams of red and violet. As as we circle around the boundary, we move through the different “hues” of color.
沿着颜色锥体（color cone 色锥？）边缘的色彩是生动鲜明的（vivid？）且被感知为“饱和的”。始于L轴，同时沿着曲线部分移动，我们顺着彩虹的色彩从红到绿再到紫罗兰色。这些色彩仅能通过纯光束被获得。另外，颜色锥体的边缘有一个平面的楔形（在2D图示中以一条线段表示）。位于这个楔形上的色彩是紫色和粉色系。这些色彩不会显示在彩虹中，并且只能通过合适地组合红色和紫罗兰（violent）色光束来获得。当我们围绕锥体边缘转圈，我们会经过不同的色调（hues）。

As we move in from the boundary towards the central region of the cone, the colors, while maintaining their hue, desaturate, becoming pastel and eventually grayish or whitish. (Though in our treatment we will not need to have a speciﬁc color formally selected as white).
当我们从边缘向颜色锥体的中央区域移动进去，那些色彩，同时保持他们的色调，饱和度不断减小，色彩变浅并且最终发灰或发白。（但是在我们的处理中我们将不需要专门正式地选定一个颜色作为白色）

This general description can be numerically formulated in the so called hue saturation value system of describing color.
这种通用描述可以被数字化准确表达在所谓的明确色彩的色调饱和度评估系统

##19.2 数学模型

##19.5 反射建模
###19.5.1 白平衡
Given a ﬁxed scene, if we alter the illuminants, then the colors in an image will change as well. For example, if we switch from a ﬂuorescent to an incandescent bulb, the colors observed by a camera will all move towards yellow. Often, we wish to adjust the image colors in order to approximate the image that would have been taken under a chosen “canonical illuminant” (say daylight). This process is called white balancing. It is not a basis change, but an actual transformation performed on all of the colors. The simplest such kind of transform allows the user to independently scale the R,G and B coordinates with three gain factors.
假定在一个固定场景中，如果我们改变光源（照明物），那么一个图像中的色彩将会同时改变。例如，如果我们从荧光灯切换到白织灯，相机（或观察者）所观察的色彩会整体移向黄色端。经常地，我们希望调整图像色彩以便于近似到同样的图像，而这个图像是在”特定的“经典光源下（比方说白天）拍摄。这个处理被称为“白平衡化“。这不是一个基（basis）变化，而是应用在所有色彩上的变换（transformation）。最简单的这种变换允许用户使用3个增益因子（gain factor）独立的缩放（scale）调整R，G，B坐标。

As just described, we cannot hope to always succeed in producing the true picture of the scene under the canonical illuminant, since we have already lost the spectral information when creating the initial image. Indeed, some objects that should appear different under the canonical illuminant may be metameric under the current illuminant, and have the exact same color coordinates in the current image. No amount of simple white balancing can undo this.
就如刚才描述的，我们不能期望总是可以成功产生场景在经典光源下的真实图片，因为当我们生成最初的图像时就已经丢失了频谱信息。确实，有些在经典光源下呈现出不同外观的对象（物体）在当前光源下可能是条件等色，从而在当前图像中拥有完全相同的颜色坐标。没有确切量级的简单白平衡化操作可以复原这种情形。

##19.6 感官适应性（adaption）
The color data from the retina undergoes signiﬁcant processing in the visual system, and humans by no means directly perceive the raw retinal color coordinates of an observed light beam. This processing results in a great deal of normalization; adapting to global and local trends across the ﬁeld of view.
来自视网膜的颜色数据在视觉系统中经历了明显（显著）的处理，同时人类没有途径可以直接感知一个被观察光束最原始的视网膜颜色坐标。这个处理导致了大量的标准化（流程）；可以适应于横跨整个视域的全局和本地变化趋势。

When the illuminant changes, say from sunlight to overcast sky, each of the directly observed color coordinates on the retina may undergo drastic changes. But these drastic changes are not ultimately perceived, and the colors for each object remain signiﬁcantly “constant”. For example, a scary tiger will be perceived as yellow under a surprisingly wide array of illuminants (impelling us to run). This phenomenon is called color constancy. As per our discussion of white balance, no such color constancy mechanism can be expected to be perfect, as too much spectral information has been thrown away by the process that converts incoming spectral beams into triplets of cone responses in the retina. But this process works to a great degree, which allows us to think about a material (tiger’s fur) as actually possessing a color (scary orange).
当光源变化时，比方说从阳光普照到漫天乌云，视网膜上每个直接被观察到的颜色坐标可能经理剧烈的变化。但是这些剧烈的变化最终没有被人类感知到，同时针对每个目标（对象）的颜色很大程度上保持“恒定”。例如，一个令人恐惧的老虎会在极为广大范围的光源下被感知为黄色（激励人类逃跑）。这个现象被称为颜色恒定（性 clor constancy）。根据我们对白平衡的讨论，不能期望这种颜色稳定（恒定）机制是完美的，因为太多的频谱信息在从输入光谱光束到视网膜中锥体细胞响应的3元响应数值的处理中被丢失。但是在很多程度上，人类视觉系统的这个处理可以正常工作，因而允许我们将一个材料（老虎皮毛）当作实际处理一种颜色（惊慌桔色）。

Even when only a local region of the ﬁeld of view undergoes an illumination change (say some part goes into shadow) our visual processing may adapt differently in this region, again keeping the end perceived colors closely tied with the actual materials observed. (See for example Figure 19.4). This process is not yet fully understood.
甚至当一个视域中的局部区域经历了一个亮度改变（比如说某部分进入阴影中），我们的视觉处理过程可能在这个区域也会有不同程度的适应，再一次保持最终被感知的颜色紧密的贴近实际观察的材料。（看例子图示19.4）。这个处理还没有完全被理解。

When we take a picture under some illuminant, but later view the picture under a different ambient illuminant, the viewer’s adaptation state is affected by both the light coming from the image, as well as the light from the surrounding room. Due to the effect of the room’s light, the colors in the picture can ultimately “look wrong”. This is, in part, why we need to do the white balancing described above.
当我们在某个光源下拍摄一张照片，但是随后观看这张照片在一个不同的环绕光源下，观看者的适应状态既会被来自图片的光线也会被来自周围屋子的光线所影响。由于房间光线的效应，照片中的色彩可能最终“看起来是错的”。这也是部分原因为什么我们需要做上面所表明的白平衡化。

