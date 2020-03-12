//
// Copyright (C) 2015 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//  ShaderPlain.vsh
//
//OpenGL ES version specification
#version 300 es

//layout(packed, row_major) uniform;

//--------------------------------------------------------------------------------
// 统一参数变量统一设置
//--------------------------------------------------------------------------------
//uniform变量 -- 4X4模型视图矩阵（view视图为眼部视图）
uniform  mat4      uMVMatrix;
//unifrom变量 -- 4X4MVP(实际使用model/view/projection)矩阵
uniform  mat4      uPMatrix;
//unifrom变量 -- 光位置矢量,以中等精度表示
uniform  mediump vec3  vLight0;

//颜色值以低精度表示
//uniform variable, store material diffuse color vector (4 components)
uniform  vec4       vMaterialDiffuse;
//uniform variable, store material ambient color vector (3 components)
uniform  vec3       vMaterialAmbient;
//uniform variable, store material specular color vector (4 components)
uniform  vec4       vMaterialSpecular;

//--------------------------------------------------------------------------------
// vertex inputs(attributes) variables
//--------------------------------------------------------------------------------
// vertex input variable 顶点位置vector
layout(location = 0) in vec3 myVertex;
// vertex input variable 法线vector
layout(location = 1) in vec3 myNormal;
// vertex input variable UV
layout(location = 2) in vec2 myUV;
// vertex input variable Bone
layout(location = 3) in vec4 myBone;

//--------------------------------------------------------------------------------
//  vertex output variables aka. varying variables declaration
//--------------------------------------------------------------------------------
// varying variable 纹理坐标
out vec2    texCoord;
// varying variable 漫射颜色
out vec4    colorDiffuse;

// varying variable 顶点位置（以world frame表示）
out vec3    position;
// varying variable 法线（以world frame表示）
out vec3    normal;


void main(void)
{
    //将顶点的对象坐标以4-coordinate vector赋给P
    vec4 p = vec4(myVertex,1);
    //转换顶点
    gl_Position = uPMatrix * p;

    texCoord = myUV;
    
    //将对象空间的normal vector变为eye frame空间的normal vector
    vec3 eyeNormal = vec3(mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz) * myNormal);
    //在取出顶点坐标为3-coordinate vector方式,ecPosition仍为对象坐标形式
    //vec3 ecPosition = p.xyz;
    vec3 eyePosition = vec3(mat3(uMVMatrix[0].xyz, uMVMatrix[1].xyz, uMVMatrix[2].xyz) * myVertex);
    
    //colorDiffuse的计算有问题，不能向紫铜色靠拢，颜色的调试可以直接依靠fragment的输出
    colorDiffuse = max(0.0,dot(normalize(eyeNormal), normalize(-vLight0 + eyePosition) )) * vMaterialDiffuse ;
    
    //colorDiffuse = vec4(0.6662,0.3831,0.3831,1.5662);

    normal = eyeNormal;
    position = eyePosition;

}
