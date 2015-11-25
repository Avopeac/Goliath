#version 420
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
out vec3 ourPosition;
out vec3 ourNormal;
out vec3 ourWorldPosition;
out vec2 ourUv;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 mvp;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;
void main()
{
	vec4 modelPos = model * vec4(position, 1);
    vec4 viewPos = view * modelPos;
	ourPosition = viewPos.xyz;
	ourWorldPosition = modelPos.xyz;
	ourNormal = normalize(normal);
    ourUv = uv;
	ourColor = color;
	
	float far =	10000.0;
	float c = 1.01 - clamp(length(-ourPosition.z), 0, 1);
    gl_Position = mvp * vec4(position, 1);
	gl_Position.z = (2.0 * log(c * gl_Position.w + 1.0) / log(c * far +  1) - 1) * gl_Position.w;
}
