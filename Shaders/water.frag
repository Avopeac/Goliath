#version 430

out vec4 FragColor;
in float gDisplacement;
in vec3 gNormal;
in vec3 gFacetNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

uniform vec3 sunlightDir;

void main()
{
	vec3 wNormal = normalize(gFacetNormal);
	float light = max(0.0, dot(sunlightDir, wNormal));
    FragColor = vec4(0.0, light / 10.0, 0.1 + light / 5.0, 1.0);
}
