#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
layout(location = 4) in vec3 sphereNormal;
out vec3 ourPosition;
out vec3 ourNormal;
out vec3 ourSphereNormal;
out vec2 ourUv;
out vec3 ourColor;
uniform mat4 mv;
uniform mat4 mvp;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;
void main()
{
	vec4 viewPos = mv * vec4(position, 1);
	ourPosition = viewPos.xyz;
	ourNormal = normalize(normal);
    ourUv = uv;
	ourColor = color;
	ourSphereNormal = normalize(sphereNormal);

	
	float far =	100000000.0;
	float c = 0.01;
    gl_Position = mvp * vec4(position, 1);
	gl_Position.z = (2.0 * log(c * gl_Position.w + 1.0) / log(c * far +  1) - 1) * gl_Position.w;
}
