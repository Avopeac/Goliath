#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
out vec3 vPosition;
out vec3 vNormal;

// Atmosphere variables
out vec3 v_c0;
out vec3 v_c1;
out float v_t0;

uniform vec3 cameraPos;
uniform float atmoSamples;
uniform vec3 sunlightDir;
uniform float innerRadius;
uniform float outerRadius;
uniform float outerRadius2;
uniform float cameraHeight2;
uniform float scale;
uniform float scaleDepth;
uniform float scaleOverDepth;
uniform vec3 invWaveLength;
uniform float Kr4Pi;
uniform float Km4Pi;
uniform float KmEsun;
uniform float KrEsun;
uniform vec3 lightDir;

float getNearIntersection(vec3 cPos, vec3 ray, float cHeight2, float oRadius2)
{
    float B = 2.0 * dot(cPos, ray);
    float C = cHeight2 - oRadius2;
    float D = max(0.0, B * B - 4.0 * C);
    return 0.5 * (-B - sqrt(D));
}

float scaleFunc(float fCos)
{
    float x = 1.0 - fCos;
    return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

void main()
{
	// Atmosphere
	// Position with magic scaling
	vec3 scaledPosition = position * 1.0 / (1.025 * length(position));
	vec3 ray = scaledPosition - cameraPos;
	float far_atmo = length(ray);
	ray /= far_atmo;
	float near_atmo = getNearIntersection(cameraPos, ray, cameraHeight2, outerRadius2);
	vec3 start = cameraPos + ray * near_atmo;
	far_atmo -= near_atmo;
	float startAngle = dot(ray, start) / outerRadius;
	float depth = exp((innerRadius - outerRadius) / scaleDepth);
	float cameraAngle = dot(-ray, scaledPosition) / length(scaledPosition);
	float lightAngle = dot(lightDir, scaledPosition) / length(scaledPosition);
	float cameraScale = scaleFunc(cameraAngle);
	float lightScale = scaleFunc(lightAngle);
	float cameraOffset = depth * cameraScale;
	float temp = lightScale + cameraScale;
	float sampleLength = far_atmo / 5.0;
	float scaledLength = sampleLength * scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;
	vec3 frontColor = vec3(0, 0, 0);
	vec3 attenuate;
	for (int i = 0; i < 5; ++i){
		float height = length(samplePoint);
		float depth = exp(scaleOverDepth * (innerRadius - height));
		float scatter = depth * temp - cameraOffset;
		attenuate = exp(-scatter * (invWaveLength * Kr4Pi + Km4Pi));
		frontColor += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	v_c0 = frontColor * (invWaveLength * KrEsun);
	v_c1 = frontColor * KmEsun;
	v_t0 = length(cameraPos - scaledPosition);

    vPosition = position;
	vNormal = normalize(normal);
}
