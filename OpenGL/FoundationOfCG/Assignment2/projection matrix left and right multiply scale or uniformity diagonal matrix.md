## 在OpenGL中用Projection Matrix 分别左乘和右乘Scale Matrix或Uniformity Diagonal Matrix后对最终成像效果的影响

### 环境描述
本文所述Projection Matrix均对应右手性（right handedness）的frame变换，也即near/far值都为负值，Projection Matrix变换所产生的z-buffer值越大距离眼睛坐标原点（camera）越近。 在OpenGL环境中对应非默认的深度检测函数设置`glDepthFunc(GL_GREATER)`。

#### 基准Projection Matrix变换过程及成像参数描述
我们将最初的Projection Matrix对应的MVP变换称为基准变换。Vertex Shader在执行MVP变换时，实际应用于顶点（Vertices）的顺序分别是Model Matrix，View Matrix，Projection Matrix。其中Model Matrix将对象坐标变换为world coordinate，View Matrix将world coordinate变换为眼睛坐标（eye coordinate），Projection Matrix将眼睛坐标变换为裁切clip coordinate。也就是说MVP按照字面顺序被先后应用于vertex变换。为了后面描述的方便，这里我们将基准MVP变换的眼睛坐标记为`[Xe,Ye,Ze,1]ᵗ`,clip coordinate记为`[Xc,Yc,Zc,Wc]ᵗ`. 在经过clipping和perspective division之后，clip coordinate变换为normalized device coordinate`[Xn,Yn,Zn]ᵗ`。最后normalized device coordinate被用于成像。
####注： `[X,Y,Z,W]ᵗ`标记表示集合为垂直集合。

我们从四种对应情形入手分别讨论，首先给定`P`为标准的view frustum Projection Matrix，`S`为Scale Affine Matrix(对角线vector为[3,3,3,1])，`O`为Uniformity Diagonal Matrix（对角线vector为[3,3,3,3]）。

### 情形一：将P替换为PS，即Projection Matrix 右乘（right multiplies）Scale Affine Matrix
#### 对最终成像的影响：成像中可能会少一些对象，也可能会多一些对象，但是和基准变换相比保留的对象的成像没有变化
这种情况我们按照Vertex Shader变换顶点时的应用顺序，在Projection Matrix变换前，对眼睛坐标沿着每个axis进行了3倍放大，这样就获得了眼睛坐标`[3Xe,3Ye,3Ze,1]ᵗ`。之后眼睛坐标被Projection Matrix变换为clip coordinate`[3Xc,3Yc,Zc‘,3Wc]ᵗ`，考虑由于所有新的Xc，Yc，Zc坐标的裁切范围都被放大了3倍，但是其中Zc‘并没有同比放大3倍，可据Projection Matrix计算推算存在可能使得原来vertex的`Zc‘<=-3Wc`，可能与基准变换原来的顶点相比有一部分深度（Ze值）小的顶点（远离相机）会被裁切，而原来vertex的Zc'也不如原来那样接近3Wc，可能会导致基准变换被裁切的near值前的一部分点现在不被裁切。对于view frustum中留下的vertex，在执行perspective division之后所获得的normalized device coordinate为`[Xn,Yn,Zn‘]ᵗ`，`（Xn，Yn)`的值没变化，从而我们看到最终成像和基准成像没变化（除了可能会少一些原来的对象，也可能会多出一些新对象外）。

### 情形二：将P替换为PQ，即Projection Matrix 右乘（right multiplies）Uniformity Diagonal Matrix
#### 对最终成像的影响：成像相比基准成像没有任何变化

 相比于情形一，scale matrix首先将眼睛坐标的每个部件都放大3倍，包括第4部件-`[3Xe,3Ye,3Ze,3]ᵗ`。这样经过projection matrix变换后，我们就获得新的clip coordinate `[3Xc,3Yc,3Zc,3Wc]`。从新的clip coordinate我们可以看出虽然x，y，z轴坐标都同比放大3倍，但是w变量也被放大三倍，这样裁切后相比基准变化的顶点完全一样。再经过perspective division后，我们获得和基准变换完全相同的`[Xn,Yn,Zn]ᵗ`。也就是说成像不会有任何变化。

### 情形三：将P替换为SP，即Projection Matrix 左乘（left multiplies）Scale Affine Matrix
#### 对最终成像的影响：成像中对象的数目会经历（可能剧烈）减少，同时view frustum之外的新顶点不可能在进入view frustum之内。相比基准留下的对象成像各坐标放大3倍，同样顶点的对象成像比基准放大9倍
这种情形我们直接从裁切坐标开始考虑，我们可以理解为基准MVP变换后生成裁切坐标`[Xc,Yc,Zc,Wc]ᵗ`，然后Scale Matrix将前3个部件放大3倍成为新的clip coordinate-`[3Xc,3Yc,3Zc,Wc]ᵗ`。所有的部件都放大3倍，除了W变量（Wc）保留不变。这会导致在clipping phase，原来的顶点在x，y，z维度上被裁切掉的概率大大增加。同时near值之前深度的新顶点有可能新进入view frustum之内同时view frustum之外的新顶点不可能在进入view frustum之内（以Zc坐标为例，会导致生成的clip坐标值更大活更小，只能被裁切掉）。在经历perspective division之后，获得的新normalized device coordinate变为`[3Xn,3Yn,3Zn]ᵗ`。也就是说最终的成像会加入新顶点，但是原有顶点的对象成像会放大9倍（不考虑三角形所有顶点都在view frustum之外，但是有三角形边穿透view frustum的情形，暂不知道这种情形会被如何处理?猜测可能是整体被裁切掉了）。
#### 注：由于Projection Matrix变换后经过Perspective division之后，对象的深度距离信息被扭曲（非线性），只保留了前后相对深度（位置）信息，再加上考虑在趋近Ze=0时的Zn值的变化趋势更加不线性。也即如果如题所述将Zc放大三倍，而W变量不放大，那么由于Zc分布的不均匀性，可能导致在深度方向上更容易被裁切掉。

### 情形四：将P替换为QP，即Projection Matrix 左乘（left multiplies）Uniformity Diagonal Matrix
#### 对最终成像的影响：成像相比基准成像没有任何变化
从裁切过程开始同于情形二。




