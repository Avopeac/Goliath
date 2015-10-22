#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;

out vec3 ourColor;
out vec2 ourUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	ourColor = color;
	ourUv = uv;

	gl_Position = proj * view * model * vec4(position, 1.0);
}