#重建（Reconstruction）
Now let us look at the opposite problem: given a discrete image I[i][j], how do we create a continuous image I(x, y)? As we will see, this problem is central to resizing images and to texture mapping. For example, in our fragment shader, we may wish to fetch a color from a texture using texture coordinates that fall in between the texture’s pixels. In this case we need to decide what texture colors to use. This process is called reconstruction.

##17.1 常量重建（Constant）

![Figure17.1](media/Figure17.1.png)
Figure 17.1: Top row: a 64-by-64 discrete image. Bottom left: reconstructed using constant reconstruction. Bottom right: reconstructed using bilinear reconstruction.$copyright$Yasuhiro Endo.

##17.2 双线性重建（Bilinear）
Constant reconstruction produces blocky looking images. We can create a smoother looking reconstruction using bilinear interpolation. Bilinear interpolation is obtained by applying linear interpolation in both the horizontal and vertical directions. It can be described by the following code:


```
color bilinearReconstruction(float x, float y, color image[][]){

float int intx = (int) x; 
float int inty = (int) y; 
float fracx = x - intx; 
float fracy = y - inty;

colorx1 = (1-fracx) * image[intx] [inty] + (fracx) * image[intx+1][inty]; 
colorx2 = (1-fracx) * image[intx] [inty+1] + (fracx) * image[intx+1][ inty+1];

colorxy = (1-fracy) * colorx1 + (fracy) * colorx2;

return(colorxy)
}
```

In this code, we ﬁrst apply linear interpolation in x followed by linear interpolation of that result in y.

At integer coordinates, we have I(i, j) = I[i][j]; the reconstructed continuous image I agrees with the discrete image I. In between integer coordinates, the color values are blended continuously. Each pixel in the discrete image inﬂuences, to a varying degree, each point within a 2-by-2 square region of the continuous image. Figure 17.1 compares constant and bilinear reconstruction.

Let us look at the a bit more closely at the 1-by-1 square with coordinates i < x < i + 1, j < y < j + 1 for some ﬁxed i and j. Over this square, we can express the reconstruction as

I(i + x f , j + y f )

←

(1 − y f ) ((1 − x f )I[i][j] + (x f )I[i + 1][j]) (17.1) +(y f ) ((1 − x f )I[i][j + 1] + (x f )I[i + 1][j + 1])

where x f and y f are the fracx and fracy above. Rearranging the terms we get

I(i + x f , j + y f )

←

I[i][j] + (−I[i][j] + I[i + 1][j]) x f + (−I[i][j] + I[i][j + 1]) y f + (I[i][j] − I[i][j + 1] − I[i + 1][j] + I[i + 1][j + 1]) x f y f

Doing this, we see that the reconstructed function has terms that are constant, linear, and bilinear terms in the variables (x f , y f ), and thus also in (x, y). This is where the name bilinear comes from. It is also clear that this reconstruction is symmetric with respect to the horizontal and vertical directions and thus the horizontal-ﬁrst ordering in the pseudo-code is not critical.
同时这也清晰无误地表明了这种重建对于水平和垂直方向是对成的，因而伪码中水平优先的顺序表达不是影响对错的关键因素。

##17.3 基础函数（Basis Functions）


###17.3.1 边缘保留（Edge Preservation）
Linear methods, those that reconstruct an image using Equation (17.2) naively ﬁll in the space between the discrete pixels. When we look closely at the reconstructed continuous image, the edges appear to blur out. There are more advanced and non-linear techniques that attempt to maintain sharp edges even in the reconstruction, but these are beyond our scope. For more on this, see [18] and references therein.
线性方法，尤其那些借助方程（17.2）重建一张图像的线性方法仅只是在具体像素间进行简单填充。当我们靠近观察重建的连续图像，边缘会显得模糊。存在有更高级的非线性技术，这些技术甚至在重建时也尝试维护锋利（立即过度）的边缘，但是超越了我们的学习范围。关于这个主题的更多资料，参考$[18]$及其中的参考文献。


