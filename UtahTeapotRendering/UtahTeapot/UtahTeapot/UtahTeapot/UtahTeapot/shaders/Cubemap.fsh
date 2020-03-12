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
//  ShaderPlain.fsh
//

#version 410 core

precision mediump float;

uniform lowp vec3       vMaterialAmbient;
uniform lowp vec4       vMaterialSpecular;

uniform highp vec3   vLight0;

uniform samplerCube samplerObj;

in lowp vec4 colorDiffuse;

in mediump vec3 position;
in mediump vec3 normal;

out mediump vec4 fragColor;

void main()
{
    mediump vec3 halfVector = normalize(-vLight0 + position);
    mediump float NdotH = max(dot(normalize(normal), halfVector), 0.0);
    mediump float fPower = vMaterialSpecular.w;
    mediump float specular = pow(NdotH, fPower);
    
    vec3 reflected = reflect(normalize(position),normalize(normal));

    lowp vec4 colorSpecular = vec4( vMaterialSpecular.xyz * specular, 1 );
    vec3 texCoord = reflected;
    //why flipped texCoord.y
    texCoord.y = -texCoord.y;
    //fragColor = texture(samplerObj, normalize(texCoord));
    fragColor = colorDiffuse * texture(samplerObj, texCoord);// + colorSpecular* vMaterialSpecular + vec4(vMaterialAmbient.xyz, 1.0);
}
