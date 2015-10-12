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

uniform mat4 proj;

uniform vec3 eyeVecHigh;
uniform vec3 eyeVecLow;
uniform mat4 relativeToEye;

uniform bool logarithmicDepth;
uniform float logarithmicConstant;
uniform float nearPlaneDistance;

void main()
{
	mat4 normalMat = transpose(inverse(relativeToEye));
	vec3 viewNormal = vec3(normalMat * vec4(normalize(normal),0));

	//Reduce floating point errors with large positions
	vec3 t1 = positionLow - eyeVecLow;
	vec3 e = t1 - positionLow;
	vec3 t2 = ((-eyeVecLow - e) + (positionLow - (t1 - e))) + positionHigh - eyeVecHigh;
	vec3 highDiff = t1 + t2;
	vec3 lowDiff = t2 - (highDiff - t1);

	//Logarithmic depth buffer
	vec4 viewPos = relativeToEye * vec4(highDiff + lowDiff, 1.0);
	gl_Position = proj * viewPos;
	gl_Position.z = log2(max(nearPlaneDistance, 1.0 + gl_Position.w)) * logarithmicConstant - 1.0;

	ourPosition = viewPos.xyz;
	ourNormal = viewNormal;
	ourUv = uv;

	//Remove artifacts from logarithmic depth buffer adjustments in fragment shader
	ourClipZ = 1.0 + gl_Position.w;
	ourLogConstHalf = logarithmicConstant * 0.5;

}
