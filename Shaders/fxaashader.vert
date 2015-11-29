#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
out vec2 ourUv;
out vec2 ourTexelSize;
uniform sampler2D texUnit;
uniform sampler2D depthUnit;

void main(void)
{
	ourTexelSize = 1.0 / textureSize(texUnit, 0);
	ourUv = uv;
	gl_Position = vec4(position, 1.0);
}
