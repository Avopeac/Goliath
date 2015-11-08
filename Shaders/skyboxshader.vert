#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec3 texDir;
uniform mat4 proj;
uniform mat4 view;

void main(void)
{
	gl_Position = proj * view * vec4(position, 1.0);
	texDir = position;
}
