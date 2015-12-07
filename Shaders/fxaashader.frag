#version 330
#define FXAA_REDUCE_MIN (1.0 / 128.0)
#define FXAA_REDUCE_MUL (1.0 / 16.0)
#define FXAA_SPAN_MAX 16.0

out vec4 color;
in vec2 ourUv;
in vec2 ourTexelSize;
uniform sampler2D texUnit;
uniform sampler2D depthUnit;

vec3 fxaa()
{
	vec2 ts = ourTexelSize;
	vec2 fc = gl_FragCoord.xy;
	vec3 rgbNW = texture(texUnit, (fc + vec2(-1, -1)) * ts).rgb;
	vec3 rgbNE = texture(texUnit, (fc + vec2( 1, -1)) * ts).rgb;
	vec3 rgbSW = texture(texUnit, (fc + vec2(-1,  1)) * ts).rgb;
	vec3 rgbSE = texture(texUnit, (fc + vec2( 1,  1)) * ts).rgb;
	vec3 rgbMD = texture(texUnit, (fc + vec2( 0,  0)) * ts).rgb;
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaMD = dot(rgbMD, luma);
	float lumaMin = min(lumaMD, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaMD, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) * ts;
	vec3 rgbA = 0.5 * (texture(texUnit, fc * ts + dir * (1.0 / 3.0 - 0.5)).rgb + texture(texUnit, fc * ts + dir * (2.0 / 3.0 - 0.5)).rgb);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(texUnit, fc * ts + dir * (0.0 / 3.0 - 0.5)).rgb + texture(texUnit, fc * ts + dir * (3.0 / 3.0 - 0.5)).rgb);
	float lumaB = dot(rgbB, luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)) {
		return rgbA;
	}

	return rgbB;
}

void main(void)
{
    color = vec4(fxaa(), 1);
}
