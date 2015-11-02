#version 330
out vec4 color;
in vec2 ourUv;
uniform float texScale1;
uniform float texScale2;
uniform sampler2D texUnit1;
uniform sampler2D texUnit2;
void main(void)
{
	//Blend together these textures with respect to some scaling factor
	vec3 colorOne = texScale1 * texture(texUnit1, ourUv).xyz;
	vec3 colorTwo = texScale2 * texture(texUnit2, ourUv).xyz;
	color = vec4(colorOne + colorTwo, 1.0);
}
