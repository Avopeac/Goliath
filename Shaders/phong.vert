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
	mat4 normal_matrix = inverse(transpose(view * model));
    ourNormal = mat3(normal_matrix) * normal;
    vec4 viewPos = view * model * vec4(position, 1.0);
	ourPosition = viewPos.xyz;
    gl_Position = proj * viewPos;
}
