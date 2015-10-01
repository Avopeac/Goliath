#version 330

layout(location = 0) in vec3 positionHigh;
layout(location = 1) in vec3 positionLow;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

out vec3 ourPosition;
out vec3 ourNormal;
out vec2 ourUv;

out float ourClipZ;
out float ourLogConstHalf;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 eyeVecHigh;
uniform vec3 eyeVecLow;
uniform mat4 relativeToEye;

uniform bool logarithmicDepth;
uniform float logarithmicConstant;
uniform float nearPlaneDistance;
uniform float time;

void main()
{

	mat4 modelView = view * model;
	mat3 normalMat = transpose(inverse(mat3(modelView)));
	vec3 viewNormal = normalMat * normal;

	vec2 waveDirection = vec2(40.0, 0.0);
	float k = 40;//2.0*3.14/10.0;
	float T = 5.0;
	float omega = 2.0*3.14/T;
	float A = 20000.0;
	vec3 newPosH;
	float highDot = (dot(waveDirection, positionHigh.xz));
	float lowDot = (dot(waveDirection, positionLow.xz));
	float omegaTime = omega*time;
	float sinValue = sin(omegaTime)*sin(highDot)*sin(lowDot) + cos(omegaTime)*sin(highDot)*cos(lowDot) - sin(omegaTime)*cos(highDot)*cos(lowDot) + cos(omegaTime)*cos(highDot)*sin(lowDot);
	float cosValue = cos(omegaTime)*cos(highDot)*cos(lowDot) + sin(omegaTime)*sin(highDot)*cos(lowDot) - cos(omegaTime)*sin(highDot)*sin(lowDot) + sin(omegaTime)*cos(highDot)*sin(lowDot);
	
	newPosH.xz = positionHigh.xz + positionLow.xz - (waveDirection/k) * A * sinValue;
	newPosH.y = positionHigh.y + positionLow.y + A * cosValue;

	vec3 newPosL = positionLow;
	//newPosL.xz = positionLow.xz - (waveDirection/k) * A * sinValue;
	//newPosL.y = positionLow.y + A *cosValue;

	//Reduce floating point errors with large positions
	vec3 t1 = newPosL - eyeVecLow;
	vec3 e = t1 - newPosL;
	vec3 t2 = ((-eyeVecLow - e) + (newPosL - (t1 - e))) + newPosH - eyeVecHigh;
	vec3 highDiff = t1 + t2;
	vec3 lowDiff = t2 - (highDiff - t1);

	//Logarithmic depth buffer
	vec4 viewPos =  relativeToEye * vec4(highDiff, 1.0);
	gl_Position = proj * viewPos;
	gl_Position.z = log2(max(nearPlaneDistance, 1.0 + gl_Position.w)) * logarithmicConstant - 1.0;

	ourPosition =  viewPos.xyz;
	ourNormal = normalize(viewNormal.xyz);
	ourUv = uv;

	//Remove artifacts from logarithmic depth buffer adjustments in fragment shader
	ourClipZ = 1.0 + gl_Position.w;
	ourLogConstHalf = logarithmicConstant * 0.5;

}
