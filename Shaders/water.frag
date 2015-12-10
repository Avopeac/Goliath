#version 430

out vec4 FragColor;

in vec3 gPosition;
in float gDisplacement;
in vec3 gNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

uniform vec3 sunlightDir;
uniform vec3 wCameraPos;

void main()
{
	const vec4 color = vec4(0.0, 0.2, 0.3, 1.0);
	const float ambient = 0.05;
	const float diffuseStrength = 1.0;
	const float specularStrength = 5.0;
	const int shininess = 128;

	vec3 wNormal = normalize(gNormal);
	vec3 lightDir = normalize(sunlightDir);

	vec3 viewDir = normalize(wCameraPos - gPosition);
	vec3 reflectDir = reflect(-lightDir, wNormal);

	float specularLight = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	float diffuseLight = diffuseStrength * max(0.0, dot(lightDir, wNormal));

	FragColor = (ambient + diffuseLight + specularLight) * color;
}
