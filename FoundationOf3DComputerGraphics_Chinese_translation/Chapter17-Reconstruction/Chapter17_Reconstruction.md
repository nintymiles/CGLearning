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

$$\normalsize{ \begin{array}{rl}
I(i + x_f , j + y_f ) & \leftarrow  & (1 − y_f )((1 − x_f )I[i][j] + (x_f)I[i + 1][j])  \\
 & & +(y_f)((1 − x_f )I[i][j + 1] + (x_f)I[i + 1][j + 1]) 
\end{array} \qquad\qquad \tag{17.1} }$$

where x f and y f are the fracx and fracy above. Rearranging the terms we get

$$\normalsize{ \begin{array}{rrl}
I(i + x_f , j + y_f) & \leftarrow & I[i][j]\\
&& + (−I[i][j] + I[i + 1][j])x_f \\
&& + (−I[i][j] + I[i][j + 1])y_f \\
&& + (I[i][j] − I[i][j + 1] − I[i + 1][j] + I[i + 1][j + 1])x_fy_f
\end{array} }$$

Doing this, we see that the reconstructed function has terms that are constant, linear, and bilinear terms in the variables (x f , y f ), and thus also in (x, y). This is where the name bilinear comes from. It is also clear that this reconstruction is symmetric with respect to the horizontal and vertical directions and thus the horizontal-ﬁrst ordering in the pseudo-code is not critical.
同时这也清晰无误地表明了这种重建对于水平和垂直方向是对成的，因而伪码中水平优先的顺序表达不是影响对错的关键因素。

##17.3 基础函数（Basis Functions）

To get some more insight on the general form of our reconstruction methods, we can go back to Equation (17.1) and rearrange it to obtain

$$\normalsize{ \begin{array}{rrl}
I(i + x_f , j + y_f) & \leftarrow & (1 − x_f − y_f + x_fy_f)I[i][j] \\
&& +(x_f − x_fy_f)I[i + 1][j] \\
&& +(y_f − x_fy_f)I[i][j + 1] \\
&& +(x_fy_f)I[i + 1][j + 1]
\end{array} }$$

In this form, we see that for a ﬁxed position (x, y), the color of the continuous reconstruction is linear in the discrete pixel values of I. Since this is true at all (x, y), we see that our reconstruction in fact must be of the form 
$$
I(x, y) \leftarrow \sum_{i,j}B_{i,j}(x, y)I[i][j] \tag{17.2}
$$

for some appropriate choice of functions B i,j (x, y). These B are called basis functions; they describe how much pixel i,j inﬂuences the continuous image at [x, y] t .

In the case of bilinear reconstruction, these B functions are called tent functions. They are deﬁned as follows: let H i (x) be a univariate hat function deﬁned as

$$ \begin{array}{c}
H_i(x) & =  & x−i+1  & for & i−1<x<i  \\
				&& −x+i+1 & for & i<x<i+1 \\
				&& 0  & else &
\end{array}$$

See Figure 17.2. (In 1D, the hat basis can be used to take a set of values on the integers and linearly interpolate them to obtain a continuous univariate function.) Then, let T i,j (x, y) be the bivariate function

$$T_{i,j}(x, y) = H_i(x)H_j(y)$$

 This is called a tent function (see Figure 17.3). It can be veriﬁed that plugging these tent functions into Equation (17.2) gives us the result of the bilinear reconstruction algorithm.

Constant reconstruction can be modeled in this form as well, but in this case, the basis function, B i,j (x, y), is a box function that is zero everywhere except for the unit square surrounding the coordinates (i, j), where it has constant value 1.

More generally, we can choose basis functions with all kinds of sizes and shapes. In high quality image editing tools, for example, reconstruction is done using some set of bi-cubic basis functions [50]. In this sense, a pixel is not really a little square. It is simply a discrete value that is used in conjunction with a set of basis functions to obtain a continuous function.


###17.3.1 边缘保留（Edge Preservation）
Linear methods, those that reconstruct an image using Equation (17.2) naively ﬁll in the space between the discrete pixels. When we look closely at the reconstructed continuous image, the edges appear to blur out. There are more advanced and non-linear techniques that attempt to maintain sharp edges even in the reconstruction, but these are beyond our scope. For more on this, see [18] and references therein.
线性方法，尤其那些借助方程（17.2）重建一张图像的线性方法仅只是在具体像素间进行简单填充。当我们靠近观察重建的连续图像，边缘会显得模糊。存在有更高级的非线性技术，这些技术甚至在重建时也尝试维护锋利（立即过度）的边缘，但是超越了我们的学习范围。关于这个主题的更多资料，参考$[18]$及其中的参考文献。


