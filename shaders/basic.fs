#version 330
in vec3 vsoNormal;

out vec4 fragColor;

void main(void) {
  vec4 color;  
  // fragColor = vec4(0.0, 1.0, 0.0, 1.0);
  float diffuse = 0, spec = 0;
  // vec2 v = vec2(vsoTexCoord.x, 1-vsoTexCoord.y);
  //vec4 color = texture(tex0, v);
  color = vec4(1.0, 1.0, 1.0, 1.0);
  vec3 N = normalize(vsoNormal);
  // vec3 L = normalize(vsoModPos - lumPos.xyz);
  vec3 L = vec3(1, -1, -1);
  diffuse = dot(N, -L);
	/*
  if(specular != 0) {
    vec3 R = reflect(L, N);
    vec3 V = normalize(-vsoModPos);
    spec = (0.3 + 0.7 * texture(tex1, v).r) * pow(max(0, dot(R, V)), 10);
  }
	*/
  color *= diffuse;
  // fragColor = spec + color; 
  fragColor = color;
  //fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
