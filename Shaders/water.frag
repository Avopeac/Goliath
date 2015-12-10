#version 430

out vec4 FragColor;
in float gDisplacement;
in vec3 gNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

uniform vec3 sunlightDir;

void main()
{
	vec3 wNormal = normalize(gNormal);
	vec3 light = max(0.0, dot(sunlightDir, wNormal)) * vec3(0.0, 0.3, 1.0);
	FragColor = vec4(light, 1.0);
}
