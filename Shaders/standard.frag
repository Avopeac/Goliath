#version 330 core

out vec4 color;

in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
	color = vec4(normalize(ourNormal), 1.0);
}