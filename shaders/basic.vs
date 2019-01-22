#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiUV;

out vec3 vsoNormal;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

void main(void) {
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
  // vsoNormal = vsiNormal;
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
}
