#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 vPosition;
out vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vPosition = position;
	vNormal = normalize(normal);
}
