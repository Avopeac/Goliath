#version 330
layout(location = 0) in vec3 position;
layout(location = 4) in vec3 scaledPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;

out vec3 ourPosition;
out vec3 ourNormal;
out vec2 ourUv;
out vec3 ourColor;
out vec3 ourScaledPosition;

out vec3 c0;
out vec3 c1;
out float t0;

uniform vec3 cameraPos;
uniform float cameraHeight;
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

uniform mat4 mv;
uniform mat4 mvp;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;

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
	vec4 viewPos = mv * vec4(position, 1);
	ourPosition = viewPos.xyz;
	ourScaledPosition = scaledPosition;
	ourNormal = normalize(normal);
    ourUv = uv;
	ourColor = color;

	//Atmosphere
    vec3 ray = scaledPosition - cameraPos;
    float far = length(ray);
    ray /= far;
    float near = getNearIntersection(cameraPos, ray, cameraHeight2, outerRadius2);
    vec3 start = cameraPos + ray * near;
    far -= near;
    float startAngle = dot(ray, start) / outerRadius;
    float depth = exp((innerRadius - outerRadius) / scaleDepth);
	float cameraAngle = dot(-ray, scaledPosition) / length(scaledPosition);
	float lightAngle = dot(lightDir, scaledPosition) / length(scaledPosition);
	float cameraScale = scaleFunc(cameraAngle);
	float lightScale = scaleFunc(lightAngle);
	float cameraOffset = depth * cameraScale;
	float temp = lightScale + cameraScale;
	const float samples = 5.0;
	float sampleLength = far / samples;
	float scaledLength = sampleLength * scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint = start + sampleRay * 0.5;
	vec3 frontColor = vec3(0, 0, 0);
	vec3 attenuate;
	for (int i = 0; i < int(samples); ++i){
		float height = length(samplePoint);
		float depth = exp(scaleOverDepth * (innerRadius - height));
		float scatter = depth * temp - cameraOffset;
		attenuate = exp(-scatter * (invWaveLength * Kr4Pi + Km4Pi));
		frontColor += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	c0 = frontColor * (invWaveLength * KrEsun);
	c1 = frontColor * KmEsun;
	t0 = length(cameraPos - scaledPosition);
	
	float fPlane = 1000000000.0;
	float c = 0.001;
    gl_Position = mvp * vec4(position, 1);
	gl_Position.z = (2.0 * log(c * gl_Position.w + 1.0) / log(c * fPlane +  1) - 1) * gl_Position.w;
}
