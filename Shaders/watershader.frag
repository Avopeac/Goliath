#version 330
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourColor;
uniform float time;
uniform sampler2D waterNormalTex;

void main()
{
	vec3 lightDir = normalize(vec3(0,1,0));
	vec3 normalDir = normalize(ourNormal);

	float speed = 0.02;
	vec3 ntex = 0.5 * (texture(waterNormalTex, 6 * ourUv + speed * vec2(time)).rgb + texture(waterNormalTex, ourUv + speed * 0.2 * vec2(-time)).rgb);
	vec3 dn = normalize(2.0 * ntex - 1.0);
	normalDir = normalize(normalDir + 0.5 * dn);

	
	//Lighting
	float ndotl = clamp(dot(normalDir, lightDir), 0.0, 1.0);
    float ndots = clamp(0.5 + 0.5 * ndotl, 0.0, 1.0);
    float ndoti = clamp(dot(normalDir, normalize(lightDir * vec3(-1, 0, -1))), 0.0, 1.0);
	float specular = 0.0;
	if (ndotl > 0.0){
		vec3 viewDir = normalize(-ourPosition);
		vec3 reflDir = normalize(reflect(-lightDir, normalDir));
		vec3 halfDir = normalize(normalDir + reflDir);
		specular = clamp(dot(halfDir, viewDir), 0.0, 1.0);
		specular = pow(specular, 90);
	}

	//Light colors	
	vec3 sunColor = vec3(1.64,1.27,0.99);
	vec3 skyColor = vec3(0.16,0.20,0.28);
	vec3 reflSunColor = vec3(0.40,0.28,0.20);
    vec3 lighting =  sunColor * ndotl;
	lighting += skyColor * ndots;
    lighting += reflSunColor * ndoti;
	
	vec3 oceanColor = vec3(0.1, 0.6, 0.97);
    vec3 final = oceanColor * lighting + vec3(1.0) * specular * 1.5;
    color = vec4(final, 0.8);
}


 


