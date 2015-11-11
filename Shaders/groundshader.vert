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
uniform samplerCube noiseCube;
uniform samplerCube normalNoiseCube;

void main()
{
	vec3 normalizedPos = normalize(position);
	ourNormal = normalizedPos;
    ourUv = uv;

	vec4 sampl = texture(noiseCube, normalizedPos);
	float height = 0.1 * (sampl.r + sampl.g + sampl.b) * 0.3333;
    //vec4 viewPos = view * model * vec4(height * normalizedPos, 1.0);
	vec4 viewPos = view * model * vec4(4.0f * height * normalize(position) + position, 1.0);
	ourPosition = viewPos.xyz;
	float far =	100000.0;
	float c = 1.0;
	vec4 clipPos = proj * viewPos;
	clipPos.z = (2.0 * log(c * clipPos.w + 1.0) / log(c * far +  1) - 1) * clipPos.w;
    gl_Position = clipPos;
}
