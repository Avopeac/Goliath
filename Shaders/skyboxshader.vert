#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec3 texDir;
out vec3 viewPos;
uniform mat4 proj;
uniform mat4 view;

void main(void)
{
	vec4 vPos = view * vec4(position, 1.0);
	viewPos = vPos.xyz;
	gl_Position = proj * vPos;
	texDir = position;
}
