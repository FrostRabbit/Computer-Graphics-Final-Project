#version 430

// x, y, z
layout(location = 0) in vec3 position;
// x, y, z
layout(location = 1) in vec3 normal;
// u, v
layout(location = 2) in vec2 texCoord;

uniform mat4 Projection;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;

out vec2 TexCoord;

void main() {
	gl_Position = Projection * ViewMatrix * ModelMatrix * vec4(position, 1.0);
	TexCoord = texCoord;
}
