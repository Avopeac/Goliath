#version 330
out vec4 color;
in vec2 ourUv;
uniform float gamma;
uniform float exposure;
uniform sampler2D texUnit;
void main(void)
{
	//Gamma correct the in texture
	vec3 result = texture(texUnit, ourUv).xyz;
	result = pow(result, vec3(1.0 / gamma));
    color = vec4(vec3(1.0) - exp(-result * exposure), 1.0);
}
