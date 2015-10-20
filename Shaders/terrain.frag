#version 330 core

out vec4 color;
in vec3 ourPosition;
in vec3 ourNormal;
in vec2 ourUv;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform mat4 relativeToEye;
uniform vec3 light;

void main()
{
	vec3 normal = normalize(ourNormal);
	//lightsource at infinite distance
	vec3 lightDir = (relativeToEye * vec4(light.xyz, 0)).xyz;
	lightDir = normalize(lightDir);

	//in eye space
	vec3 viewDir = normalize(-ourPosition);

	//half way
	vec3 halfDir = normalize(lightDir + viewDir);

	float diffuse = max(0.0, dot(lightDir, normal));
	
	float specular = 0.0;
	float shininess = 5.0;
	if (diffuse > 0)
	{
		specular = pow(max(dot(normal, halfDir), 0.0), shininess);
	}

	float gamma = 2.2;
	
	color = vec4(0.3257, 0.2851, 0.0845, 1.0) * diffuse; // + vec4(1.0, 1.0, 1.0, 1.0) * specular;
	color.rgb = pow(color.rgb, vec3(1.0 / gamma));
}
