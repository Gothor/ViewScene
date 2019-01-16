#version 330
uniform vec4 color;

out vec4 fragColor;

void main(void) {
  // fragColor = color;
  fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
