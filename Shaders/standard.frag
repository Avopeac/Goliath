#version 330 core

out vec4 color;

in vec3 ourColor;
in vec2 ourUv;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
	color = texture(texture_diffuse1, ourUv);
}