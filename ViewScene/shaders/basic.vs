#version 330

layout (location = 0) in vec4 vsiPosition;
// layout (location = 1) in vec3 vsiNormal;

uniform mat4 projectionMatrix;
uniform mat4 modelViewMatrix;

void main(void) {
  //gl_Position = vec4(translate + vsiPosition, 1.0/scale);
  // gl_Position = projectionMatrix * modelViewMatrix * vec4(vsiPosition, 1.0);
  gl_Position = projectionMatrix * modelViewMatrix * vsiPosition;
}
