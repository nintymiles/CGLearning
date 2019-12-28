# 光线追踪是什么（What is Ray Tracing）
光线追踪表达了一种不同标准OpenGL管线的渲染方式。这种技术的原理大部分超越了本书的范围，但是我们会给出基础概念的概要。关于这个主题在本书中要参考更多，请查看本书课后练习[23]或[56]。

## 20.1 Loop Ordering
从最基本的视角，我们可以将OpenGL基于光栅化的渲染看作下面的算法形式


```psuedo code
initialize z-buffer  //启动z-buffer
for all triangles    //遍历所有三角形
	for all pixels covered by the triangle  //遍历三角形所覆盖的像素
		compute color and z    //计算像素的色彩和z值
			if z is closer than what is already in the z-buffer  //如果z比z-buffer中所见的更接近
				update the color and z of the pixel //更新这个像素的色彩和z值
```

This algorithm has the nice property that each triangle in the scene is touched only once, and in a predictable order. It is for this reason, for example, that even the ofﬂine Renderman software from Pixar uses this basic algorithm. Another nice property is that, during the processing of a triangle, setup computation is done only once and can be amortized over the all of the pixels in it. As we have already seen, rasterization-based rendering can be enhanced with fancy shading computation and even with multipass algorithms. There are also advanced algorithms such as occlusion culling (see for example [11] and references therein) that attemp to avoid rendering triangles that we know will be occluded by other objects in the scene.

In basic ray tracing, we reverse the loop orders to obtain

for all pixels on the screen
for all objects seen in this pixel

if this is the closest object seen at the pixel 
compute color and z set the color of the pixel

In the second line, we need to compute which objects are seen along the line of sight of a pixel (see Figure 20.1). This step requires computing the intersection between a ray and the scene. Here are a few advantages of ray tracing.

• Using ray tracing, we never waste computation calculating the color of an occluded object.

• Since we have an ordered list of intersections along ray, (non refractive) transparency is easy to model. This is because we have an ordered list of intersections along a ray, which allows us to apply the over operator from Equation (16.4).

• Using ray intersections, we can directly render smooth objects, without having to ﬁrst dice them up into triangles.

• It is also easy to render solid objects described by volumetric set operations, such as the union or intersection of objects. (See Exercise 54).

Most importantly, once ray intersection code is in place, we can use this infrastructure to do all kinds of assorted calculations that involve chasing geometric rays through the scene. For example, it is easy, using ray tracing, to model perfect mirror reﬂection, and to compute shadows.

20.2 Intersection

The main computation needed in ray tracing is computing the intersection of a geometric ray (˜p, d) with an object in the scene. Here ˜p is the start of the ray, which goes off in direction d.

20.2.1 Plane

Suppose we wish to compute the intersection of (˜p, d) with a plane described by the equation Ax + By + Cz + D = 0. We start by representing every point along the ray using a single parameter λ ⎡ ⎤ ⎡ ⎤ ⎡ ⎤ x p x d x ⎣ y ⎦ = ⎣ p y ⎦ + λ ⎣ d y ⎦ (20.1) z p z d z

Plugging this into the plane equation, we get

0

= =

A(p x + λd x ) + B(p y + λd y ) + C(p z + λd z ) + D

λ(Ad x + Bd y + Cd z ) + Ap x + Bp y + Cp z + D

And we see that

−Ap x − Bp y − Cp z − D λ=

Ad x + Bd y + Cd z

In the solution, λ tells us where along the ray the intersection point is (negative valued λ are backwards along the ray). Comparisons between λ values can be used to determine which, among a set of planes, is the ﬁrst one intersected along a ray.

20.2.2 Triangle

If we want to intersect a ray with a triangle, we can break it up into two steps. In the ﬁrst step, we compute the A, B, C, D of the plane supporting the triangle, and compute the ray-plane intersection as above. Next, we need a test to determine if the intersection point is inside or outside of the triangle. We can build such a test using the “counter clockwise” calculation of Equation 12.4 as follows. Suppose we wish to test if a point ˜q is inside or outside of a triangle ∆(˜p 1 ˜p 2 ˜p 3 ) in 2D (See Figures 20.2 and 20.3).
Consider the three “sub” triangles ∆(˜p 1 ˜p 2 ˜q), ∆(˜p 1 ˜q˜p 3 ) and ∆(˜q˜p 2 ˜p 3 ) . When ˜q is inside of ∆(˜p 1 ˜p 2 ˜p 3 ), then all three sub-triangles will agree on their clockwisedness. When ˜q is outside, then they will disagree.

20.2.3 Sphere

The ideas behind ray-plane intersection can be adapted to calculate the ray-sphere intersection. In this case, the sphere with radius R and center c is modeled as the set of points [x, y, z] t that satisfy the equation (x − c x ) 2 + (y − c y ) 2 + (z − c z ) 2 − r 2 = 0. Plugging this into Equation (20.1), we get

0

= =

(p x + λd x − c x ) 2 + (p y + λd y − c y ) 2 + (p z + λd z − c z ) 2 − r 2 (d x 2 + d y 2 + d z 2 )λ 2 + (2d x (p x − c x ) + 2d y (p y − c y ) + 2d z (p z − c z ))λ

+(p x − c x ) 2 + (p y − c y ) 2 + (p z − c z ) 2 − r 2

We can then use the quadratic formula to ﬁnd the real roots λ of this equation. If there are two real roots, these represent two intersections, as the ray enters and exits the sphere. If there is one (doubled) real root, then the intersection is tangential. If there are no real roots, then the ray misses the sphere. As above, any of these intersections may be backwards along the ray.

At the intersection, the normal of the sphere at [x, y, z] t is in the direction [x c x y − c y , z − c z ] t . This fact may be useful for shading calculations.

,

20.2.4 Early Rejection

When computing the intersection between a ray and the scene, instead of testing every scene object for intersection with the ray, we may use auxiliary data structures to quickly determine that some set of objects is entirely missed by the ray. For example, one can use a simple shape (say a large sphere or box) that encloses some set of objects. Given a ray, one ﬁrst calculates if the ray intersects this volume. If it does not, then clearly this ray misses all of the objects in the bounded set, and no more ray intersection tests are needed. This idea can be further developed with hierarchies and spatial data structures. See for example [73], and references therein.

20.3 Secondary Rays

Once we have a ray intersection infrastructure in place, it is easy to simulate many optical phenomenon. For example, we can compute shadows due to a point light source. To determine if a scene point is in shadow, one follows a “shadow ray” from the observed point towards the light to see if there is any occluding geometry. (See Figure 20.1.)

Another easy calculation that can be done is mirror reﬂection (and similarly refrac-tion). In this case, one calculates the bounce direction using Equation (14.1) and sends a “bounce ray” off in that direction (see Figure 20.1). The color of the point hit by this ray is then calculated and used to determine the color of the original point on the mirror. This idea can be applied recursively some number of times to simulate multiple mirror (or refractive) bounces. See Figure 20.4.

20.3.1 Even More rays

As described in more detail below in Chapter 21, more realistic optical simulation requires the computation of integrals, and these integrals can often be approximated by summing up contributions along a set of samples. Computing the values for these samples often involves tracing rays through the scene.

For example, we may want to simulate a scene that is illuminated by a large light with ﬁnite area. This, among other things, results in soft shadow boundaries (see Figure 21.8 below). Lighting due to such area light sources can be approximated by sending off a number of shadow rays towards the area light and determining how many of those rays hit the light source. Other similar effects such as focus effects of camera lenses and interreﬂection are discussed in Chapter 21. These too can be calculated by tracing many rays through the scene.

