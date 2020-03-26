#version 300 es

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColor;

uniform mat4 uMVPMatrix;

out vec3 vColor;

void main()
{
    vColor = VertexColor;
    gl_Position = uMVPMatrix * vec4(VertexPosition,1.0);
}
