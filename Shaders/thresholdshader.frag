#version 330
out vec4 color;
in vec2 ourUv;
uniform sampler2D texUnit;
void main(void)
{
    vec4 tex = texture(texUnit, ourUv);
    color.r = max(0.0, tex.r - 1.0);
	color.g = max(0.0, tex.g - 1.0);
	color.b = max(0.0, tex.b - 1.0);
	color.a = 1.0;
}
