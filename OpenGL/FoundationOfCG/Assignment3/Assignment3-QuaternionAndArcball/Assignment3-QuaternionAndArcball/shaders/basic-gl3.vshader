#version 410 core

uniform mat4 uProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;
uniform float uXCoordOffset;

in vec3 aPosition;
in vec3 aNormal;

out vec3 vNormal;
out vec3 vPosition;

void main() {
vNormal = vec3(uNormalMatrix * vec4(aNormal, 0.0));

vec3 position = aPosition;
//应用x轴移位在object coordinate上
position.x += uXCoordOffset;
//send position (in eye coordinates) to fragment shader
vec4 tPosition = uModelViewMatrix * vec4(position, 1.0);
vPosition = vec3(tPosition);
gl_Position = uProjMatrix * tPosition;

//若将shader中x轴移位施加在裁切坐标上，则移动物体的操作将变得不可简单预测
//gl_Position.x += uXCoordOffset;
}
