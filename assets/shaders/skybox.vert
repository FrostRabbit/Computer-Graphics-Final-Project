#version 330 core
layout (location = 0) in vec3 position_in;

out vec3 textureCoordinate;

uniform mat4 view;
uniform mat4 projection;

void main() {
  textureCoordinate = position_in;
  mat4 view2 = view;
  view2[3][0] = 0;
  view2[3][1] = 0;
  view2[3][2] = 0;
  gl_Position = projection * view2 * vec4(position_in, 1.0);
  gl_Position.z = gl_Position.w;

}
