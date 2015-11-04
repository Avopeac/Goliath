#version 330
out vec4 color;
in vec2 ourUv;
uniform sampler2D texUnit;
uniform vec2 direction;
uniform float size;
void main(void)
{
    //Find texel size
    float blur = size / textureSize(texUnit, 0).x;
	vec4 sum = vec4(0);
    float hstep = direction.x;
    float vstep = direction.y;
	vec2 tc = ourUv;
    sum += texture(texUnit, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.0162162162;
    sum += texture(texUnit, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(texUnit, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(texUnit, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.1945945946;
    sum += texture(texUnit, vec2(tc.x, tc.y)) * 0.2270270270;
    sum += texture(texUnit, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.1945945946;
    sum += texture(texUnit, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.1216216216;
    sum += texture(texUnit, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.0540540541;
    sum += texture(texUnit, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.0162162162;
	color = vec4(sum.rgb, 1.0);
}

