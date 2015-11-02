#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
out vec2 ourUv;
void main(void)
{
	ourUv = uv;
	gl_Position = vec4(position, 1.0);
}
