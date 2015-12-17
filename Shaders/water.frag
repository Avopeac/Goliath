#version 430

out vec4 FragColor;

in vec3 gPosition;
in float gDisplacement;
in vec3 gNormal;
in vec3 gTriDistance;
in vec3 gPatchDistance;

in vec3 g_c0;
in vec3 g_c1;
in float g_t0;

uniform vec3 sunlightDir;
uniform vec3 wCameraPos;
uniform float waterLevel;

void main()
{
	const vec4 color = vec4(0.0, 0.2, 0.3, 1.0);
	const float ambient = 0.00;
	//const float ambient = 0.01;
	const float diffuseStrength = 1.0;
	const float specularStrength = 3.0;
	const int shininess = 60;

	vec3 wNormal = normalize(gNormal);
	vec3 lightDir = normalize(sunlightDir);

	// Adjusting wCameraPos instead of gPosition for better precision
	vec3 viewDir = normalize(wCameraPos / waterLevel - gPosition);
	vec3 reflectDir = reflect(-lightDir, wNormal);

	float specularLight = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	float diffuseLight = diffuseStrength * max(0.0, dot(lightDir, wNormal));

	vec4 final = (ambient + diffuseLight + specularLight) * color;

	vec3 atmosphereColor = g_c0 + 0.25 * g_c1;
	float falloff = pow(g_t0, 0.19);
	atmosphereColor = 1.0 - exp(atmosphereColor * -0.9);

	final *= 1.0 + atmosphereColor.b;

	FragColor = vec4(final.xyz + falloff * atmosphereColor, 1.0);
}
