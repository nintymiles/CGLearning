//
// Copyright (C) 2018 The Android Open Source Project
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
#version 300 es

in highp vec3    myVertex;
in highp vec3    myNormal;
in mediump vec2  myUV;
in mediump vec4  myBone;

out lowp    vec4    colorDiffuse;

out mediump vec3 position;
out mediump vec3 normal;

uniform highp mat4      uMVMatrix;
uniform highp mat4      uMVPMatrix;
uniform highp mat4      uMMatrix;

uniform highp vec3      vLight0;

uniform lowp vec4       vMaterialDiffuse;
uniform lowp vec3       vMaterialAmbient;
uniform lowp vec4       vMaterialSpecular;

void main(void)
{
    highp vec4 p = vec4(myVertex,1);
    gl_Position = uMVPMatrix * p;

    highp vec3 worldNormal = vec3(mat3(uMMatrix[0].xyz, uMMatrix[1].xyz, uMMatrix[2].xyz) * myNormal);
    highp vec3 ecPosition = vec3(uMMatrix * p);

    // brighten the teapot color by 50%
    colorDiffuse = clamp(vMaterialDiffuse * 1.5,
                         vec4(0.0, 0.0, 0.0, 0.0),
                         vec4(1.0, 1.0, 1.0, 1.0));

    normal = worldNormal;
    position = ecPosition;
}
