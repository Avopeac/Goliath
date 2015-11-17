#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
out vec3 ourPosition;
out vec3 ourWorldPosition;
out vec3 ourNormal;
out vec2 ourUv;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
void main()
{
	vec4 viewPos = view * model * vec4(position, 1.0);
	ourPosition = viewPos.xyz;
	ourWorldPosition = (model * vec4(position, 1.0)).xyz;
	ourNormal = normalize(normal);
    ourUv = uv;
	ourColor = color;
	float far =	100000.0;
	float c = 1.0;
	vec4 clipPos = proj * viewPos;
	clipPos.z = (2.0 * log(c * clipPos.w + 1.0) / log(c * far +  1) - 1) * clipPos.w;
    gl_Position = clipPos;
}
