#version 330

layout(location = 0) in vec3 highPosition;
layout(location = 4) in vec3 lowPosition;
layout(location = 1) in vec3 scaledPosition;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;

//out float logZ;
//out float fCoeffHalf;
out vec3 c0;
out vec3 c1;
out vec3 t0;

uniform mat4 mvp;
uniform vec3 highCamera;
uniform vec3 lowCamera;

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

void main(void)
{
    //FP precision tricking
	vec3 t1 = lowPosition - lowCamera;
    vec3 e = t1 - lowPosition;
    vec3 t2 = ((-lowCamera - e) + (lowPosition - (t1 - e))) + highPosition - highCamera;
    vec3 highDifference = t1 + t2;
    vec3 lowDifference = t2 - (highDifference - t1);
    vec3 vertexPos = lowDifference + highDifference;
	
	//Atmosphere
    vec3 ray = scaledPosition - cameraPos;
    float far = length(ray);
    ray /= far;

    vec3 start = cameraPos;
	float height = length(start);
	float depth = exp(scaleOverDepth * (innerRadius - outerRadius));
    float startAngle = dot(ray, start) / height;
	float startOffset  = depth * scaleFunc(startAngle);
	const float samples = 5.0;
	float sampleLength = far / samples;
	float scaledLength = sampleLength * scale;
	vec3 sampleRay = ray * sampleLength;
	vec3 samplePoint =  start + sampleRay * 0.5;
	vec3 frontColor = vec3(0, 0, 0);
	for (int i = 0; i < int(samples); ++i){
		float height = length(samplePoint);
		float depth = exp(scaleOverDepth * (innerRadius - height));
		float lightAngle = dot(lightDir, samplePoint) / height;
		float cameraAngle = dot(ray, samplePoint) / height;
		float scatter = startOffset + depth * (scaleFunc(lightAngle) - scaleFunc(cameraAngle));
		vec3 attenuate = exp(-scatter * (invWaveLength * Kr4Pi + Km4Pi));
		frontColor += attenuate * (depth * scaledLength);
		samplePoint += sampleRay;
	}

	c0 = frontColor * (invWaveLength * KrEsun);
	c1 = frontColor * KmEsun;
	t0 = cameraPos - scaledPosition;
	
	gl_Position = mvp * vec4(vertexPos, 1.0);
	//Depth buffer tricking
	float c = 0.1;
	float fPlane = 1000000000.0;
	gl_Position.z = (2.0 * log(c * gl_Position.w + 1.0) / log(c * fPlane +  1) - 1) * gl_Position.w;
	//float fCoeff = 2.0 / log2(fPlane + 1.0);
	//gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * fCoeff - 1.0;
	//logZ = 1.0 + gl_Position.w;
	//fCoeffHalf = 0.5 * fCoeff;
}
