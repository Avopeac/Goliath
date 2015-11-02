#version 330
out vec4 color;
in vec2 ourUv;
uniform sampler2D texUnit;
void main(void)
{
    color = texture(texUnit, ourUv);
}
