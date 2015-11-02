#version 330
out vec4 color;
in vec2 ourUv;
uniform float texScale1;
uniform float texScale2;
uniform float exposure;
uniform float gamma;
uniform sampler2D texUnit1;
uniform sampler2D texUnit2;
void main(void)
{
	vec3 colorOne = texScale1 * texture(texUnit1, ourUv).xyz;
	vec3 colorTwo = texScale2 * texture(texUnit2, ourUv).xyz;
	vec3 result = vec3(1.0) - exp(-(colorOne + colorTwo) * 1.0f);
	result = pow(result, vec3(1.0 / 2.2));


	color = vec4(result, 1.0);
}
