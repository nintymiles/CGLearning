# GLSL中，如何对matrix类型进行初始化
GLSL中matrix类型的初始化，主要有3种情形，分别为通过标量数字，矢量和矩阵进行。

通过标量数字和矢量初始化矩阵类型时，矩阵元素以列为主的顺序从参数中获得。

以下以实例方式说明

1. 标量数字参数方式初始化

	```
 	mat3(float, float, float, // 作为矩阵的第一列
		float, float, float,   // 作为矩阵的第二列
		float, float, float);  // 作为矩阵的第三列

	mat4(float, float, float, float, // 第一列
		float, float, float, float,   // 第二列 
		float, float, float, float,   // 第三列 
		float, float, float, float);  // 第四列
	```
2. 矢量参数方式初始化
	
	```
	mat3(vec3, vec3, vec3); 	    // 每个参数作为矩阵的一列 
	mat4(vec4, vec4, vec4, vec4); // 每个参数作为矩阵的一列 
	mat3x2(vec2, vec2, vec2);     // 非正方形矩阵，仍然每个参数作为矩阵的一列 
	```
3. 矢量和标量数字参数混合方式初始化

	以矢量和标量数字混合方式初始化矩阵，体现了这种类型初始化时的巨大弹性，只要保证有足够的部件可用于初始化矩阵即可。
	
	```
	mat2x3(vec2, float,  // 作为矩阵的第一列
		float, vec2);     // 作为矩阵的第二列
	```
4. 用矩阵作为参数来初始化矩阵
	
	```
	mat3x3(mat4x4); //采用mat4x4矩阵的左上角3x3矩阵作为此矩阵的元素
	mat4x4(mat3x3); //把mat3x3矩阵放入mat4x4矩阵的左上角，将右下角部件设置为1，其余为0
	mat2x3(mat4x2); //采用mat4x2矩阵的左上角2x2矩阵作为此矩阵的前两行，最后一行设置为0,0
	```

