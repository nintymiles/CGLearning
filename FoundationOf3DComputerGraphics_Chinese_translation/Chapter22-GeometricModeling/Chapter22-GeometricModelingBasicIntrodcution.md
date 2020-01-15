# 几何建模：基础介绍（Geometric Modeling: Basic Intro）
在计算机图形中，我们需要具体方式表达形状。几何建模就是关于怎样表达、生成和修改这些形状的主题。几何建模是一个大型主题，其本身值得拥有（并且已经有了）自己相关的书籍（例如参考书目[21,76,7]）。这里我们将简单概括一下这个主题以给你一点相关概念。我们会多花费一点时间在细分表面的表达上，因为这些正在变成流行的表达方式。

## 22.1 Triangle Soup（三角形乱序组合）
对于在OpenGL中的渲染，最直观的表达是triangle soup；三角形集合，每个通过3个顶点被描述。（参考图示$\text{Figure 22.1}$作为样例。）这种数据经常能够被更好组织以减少冗余。例如，在很多情形中，尤其当它被很多三角形共享时，每个顶点仅存储一次就有意义。另外，连接信息可以借助诸如“三角形扇(triangle fans)”和“三角形带(triangle strips)”被更紧凑地被表达。参考课后书目[79]

相关的表达包含quad soup（使用四条边的多边形）和polygon soup（针对任意数目边的多边形）。针对硬件渲染，这种多边形首先需要切分为三角形，并且随后能够借助通用三角形渲染管线被绘制。

基于三角形的几何体存在很多种能够被生成的方式。实际上，三角形几何体可以借助一个几何建模工具被从头开始生成，可以通过镶嵌平滑表面表达来获得（参考下面），或者通过直接扫描一个实际物体来获得（参考图示$\text{Figure 22.2}$）。

## 22.2 网格（Meshes）

In the soup representation, there is no way to “walk along” the geometry. One cannot easily (in constant time) ﬁgure out which triangles meet at an edge, or which triangles surround a vertex. Walking along a mesh can be useful, for example, if one is trying to smooth the geometry, or if one wishes to simulate some physical process over the geometry.

A mesh data structure is a representation that organizes the vertex, edge, and face data so that these queries can be done easily. (See for example Figure 22.3.) There are a variety of different mesh data structures and they can be tricky to implement. For a good reference on mesh data structures, see [63].

## 22.3 Implicit Surfaces

One way to represent a smooth surface is as the set of points that evaluate to zero under some given trivariate function f(x, y, z). This is called an implicit representation. For example, simple shapes, like spheres and ellipsoids, can be represented as the zero set of a quadratic trivariate function. Additionally, we can deﬁne an implicit function as % a sum of simpler elemental implicit functions: f(x, y, z) = i f i (x, y, z). Doing so, say, with a set of sphere functions, creates a smoothly blended union of the individual spheres (see Figure 22.4). This technique is known as blobby modeling, and is ideal for modeling organic shapes such as sea mammals.

Implicit surfaces have some nice properties. If we deﬁne a surface as the zero set of f, then we can think of the points where f evaluates to a positive value as the volumetric interior of the surface. As such, it is easy to apply volumetric set operations, such as union, intersection, negation and difference to these interior volumes. For example, the intersection of the volumes deﬁned by functions f 1 and f 2 can be deﬁned using the new function f(x, y, z) = min(f 1 (x, y, z), f 2 (x, y, z)). (See for example Figure 22.5.)

In order to render an implicit surface in OpenGL, we need to create a set of triangles that approximate the implicit surface. This is a non-trivial task.

22.4 Volume

A volume representation is a speciﬁc kind of implicit representation that uses a regular 3D grid of discrete values called voxels. Using trilinear interpolation over each grid cell, these voxels deﬁne a continuous function in 3D. The zero set of this function can then be thought of as an implicit surface.

Volume data is often obtained as the output of a volumetric scanning process, such as an MRI. We can also take some general implicit function representation and then sample it along a regular 3D grid to get a voxel-based approximation.

To render the zero-set of a volume based representation in OpenGL, one needs to extract a set of triangles that approximate the zero-set. Due to the regular pattern of the data, this can be done a bit more easily than for a general implicit function. The standard technique is called the marching cube method [45], but there are newer methods such as dual contouring [32] (Figure 22.5) that may give better results.

22.5 Parametric Patches

Parametric patches represent a section of surface called a patch using 3 coordinate functions x(s, t), y(s, t) and z(s, t). These functions are deﬁned over some square or triangular portion of the (s, t) plane. In most cases, each coordinate function over the patch is represented as a piecewise polynomial bivariate function. (See for example Figure 22.6.)

The most common parametric representation is the tensor-product spline surface. Recall (Section 9.5) that a spline curve can be used to represent a curve in space. Such a spline curve is deﬁned by an input control polygon that connects a sequence of discrete points in space. This spline curve is made up of a set of smaller pieces, each having coordinate functions that are, say, cubic polynomials

For a tensor product spline, this construction is “upgraded “ from curves to surfaces. In this case, the control polygon is replaced by an m-by-n rectilinear control mesh, with vertices in 3D. By appropriately applying the spline deﬁnitions in both the s and t variables, we end up with a parametric patch. Such a patch is made up of small pieces deﬁned over small squares in the (s, t) domain. The coordinates of each such piece are polynomials in s and t. In the case where we upgrade from a cubic spline curve construction, each square piece is deﬁned by a bi-cubic polynomial in (s, t) (i.e., have terms up to the highest power: s 3 t 3 ).

For rendering, splines can easily be approximated by a quad mesh in a number of ways. For example, we can just place a ﬁne grid of sample points over the (s, t) domain and evaluate the spline functions to obtain [x, y, z] t coordinates for each sample. These samples can be used as the vertices of a regular quad mesh. Another approach is to apply a recursive reﬁnement process that takes in a control mesh and outputs a denser control mesh that represents the same surface. After a few steps of subdivision, the quads themselves form a dense control mesh that form a good approximation to the underlying spline surface.

One can typically design surfaces using splines in a geometric modeling package. Spline surfaces are very popular in the computer aided design community. In particular, due to their explicit parametric polynomial representation, it is easy to apply various calculations to such shapes.

Modeling with splines is not without a number of difﬁculties. To model a closed (say ball-like) surface, we have to stitch together some number of patches. Doing the stitching in a smooth manner is tricky. Additionally, if we want patches to meet up along speciﬁc creased curves, we need to explicitly do something called “trimming” to each of the patches. One interesting book about the spline representation is [59].

22.6 Subdivision Surfaces

The subdivision surface is a simple representation that solves many of the difﬁculties associated with parametric patches. The basic idea is to start with a single control mesh. This mesh need not be rectilinear; it can represent a closed surface and can have vertices of any valence. We then use a set of rules to reﬁne the mesh, resulting in a higher resolution control mesh. By applying this process recursively some number of times, we obtain a very high resolution mesh that can be directly rendered as an approximation to a smooth surface. (See for example Figures 22.7 and 22.8.) Subdivision surfaces do not require any patching steps. Additionally, special rules can be applied along certain portions of the control mesh to achieve creases in the surface where desired.

The subdivision rules are deﬁned such that the meshes arising after more and more levels of subdivision converge to a smooth limit-surface (a C 1 immersed submanifold in R 3 ). In many cases, such as in regions where the mesh is rectilinear, the subdivision steps match those used to reﬁne a tensor-product spline surface.

One drawback of a subdivision surface is that the surface representation is deﬁned by a procedure, and not by a formula. Thus, such surfaces are more difﬁcult to mathematically analyze. Additionally, away from the rectilinear regions (at so-called extraordinary points) the limit surface can be somewhat poorly behaved [55] and harder to control. But, for casual use (movies and games), these are not big issues, and subdivision surfaces have proven to be simple, useful, and general.

22.6.1 Catmull-Clark

Here we describe in more detail a speciﬁc subdivision representation due to Catmull and Clark. One starts with some input mesh M 0 (which we will assume is a watertight “manifold with no boundary”). This mesh has connectivity information describing its vertex, edge and face structure. The mesh also has geometric information, mapping each abstract vertex to a point in 3D. 
Now we apply a set of connectivity updates to get a new reﬁned mesh M 1 , with its own connectivity and geometry. The connectivity of M 1 is deﬁned as follows (and shown in Figure 22.7). For each vertex v in M 0 , we associate a new “vertex-vertex” v v in M 1 . For each edge e in M 0 we associate a new “edge-vertex” v e in M 1 . For each face f in M 0 we associate a new “face-vertex” v f in M 1 . These new vertices are connected up with new edges and faces as shown in Figure 22.7. We can easily verify that in M 1 , all faces are quads. In M 1 we call any vertex of valence four “ordinary” and any vertex of valence different from four “extraordinary”.

We apply this subdivision process recursively. Given M 1 , or more generally, M i , for any i ≥ 1, we apply the same subdivision rules to obtain a ﬁner mesh M i+1 . In the new mesh, we have roughly 4 times the number of vertices. But importantly, we can verify that the number of extraordinary vertices stays ﬁxed (see Figure 22.7)! Thus, during subdivision, more and more of the mesh looks locally rectilinear, with a ﬁxed number of isolated extraordinary points in between.

Now all we need are rules to determine the geometry for the new vertices as they are created during each subdivision step. First, let f be a face in M i surrounded by the vertices v j (and let m f be the number of such vertices). We set the geometry of each new face-vertex v f in M i+1 to be

1

v f

=

m f

&

j

v

j

(22.1)

(i.e., the centroid of the vertices in M i deﬁning that face). (Again, for any subdivision level i ≥ 1, we have m f = 4.) See Figure 22.9.

Next, let e be an edge in M i connecting the vertices v 1 and v 2 , and separating the faces f 1 and f 2 . We set the geometry of the new edge-vertex in M i+1 to be

v e =

1 4

(v 1 + v 2 + v f 1 + v f 2 )

(22.2)

See Figure 22.10.

Finally let v be a vertex in M i connected to the n v vertices v j and surrounded by the n v faces f j . Then, we set the geometry of the new vertex-vertex in M i+1 to be

& 1 n v 2 j For an ordinary vertex, with valence n v = 4, this becomes

n v − 2

n v

v v =

1 v+

n 2 v

&

j

v j +

v

f j

(22.3)

v v =

1 2

v+

1 16

&

j

v j +

1 16

&

j

v

f j

(22.4)

See Figure 22.11.

There has been much study into the development of these subdivision rules, and understanding their convergence properties. The ﬁrst complete analysis of the ﬁrst order behavior at extraordinary points is found in Reif [60].

In practice, you don’t need to know any of this; all you need to do to use subdivision surfaces is to implement Equations (22.1), (22.2) and (22.3). The hardest part is simply getting a mesh data structure set up so that you can implement this computation.

