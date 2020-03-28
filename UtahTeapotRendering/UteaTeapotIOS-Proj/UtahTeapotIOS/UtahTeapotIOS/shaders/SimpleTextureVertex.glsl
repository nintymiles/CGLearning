#version 300 es

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 TextureCoord;

uniform mat4 uMVPMatrix;

out vec2 vTex;

void main()
{
    vTex = TextureCoord;
    gl_Position = uMVPMatrix * vec4(VertexPosition,1.0);
}
