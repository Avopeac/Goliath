#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 ourPosition;
out vec3 ourNormal;
out vec2 ourUv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	ourUv = uv;
	ourNormal = normal;
	vec4 pos = view * model * vec4(position, 1.0);
	ourPosition = pos.xyz; 
	gl_Position = proj * pos;
}