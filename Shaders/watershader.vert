#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
out vec3 ourPosition;
out vec3 ourNormal;
out vec2 ourUv;
out vec3 ourColor;
uniform mat4 mv;
uniform mat4 mvp;
uniform float time;

void main()
{
	ourNormal = normalize(normal);
    vec4 viewPos = mv * vec4(position, 1);
	ourPosition = position.xyz;
    ourUv = uv;
	ourColor = color;
	float far =	100000000.0;
	float c = 0.001;
    gl_Position = mvp * vec4(position, 1);
	gl_Position.z = (2.0 * log(c * gl_Position.w + 1.0) / log(c * far +  1) - 1) * gl_Position.w;
}
