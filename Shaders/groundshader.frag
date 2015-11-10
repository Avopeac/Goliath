#version 330 core
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
uniform samplerCube noiseCube;
uniform samplerCube normalNoiseCube;

void main()
{
	vec3 n = normalize(ourNormal);
	vec3 normal = normalize(n + (2.0 * texture(normalNoiseCube, n).xyz - 1.0));
	vec3 l = normalize(vec3(1,1,0));
	float ndotl = dot(normal, l);
	float diffuse = max(0.001, ndotl);
	vec3 albedo = 1.5 * texture(noiseCube, n).xyz;
	color = vec4(albedo * diffuse, 1.0);
}




 

