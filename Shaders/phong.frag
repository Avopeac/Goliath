#version 330 core

// This shader uses the Cook-Torrence shading model from http://content.gpwiki.org/D3DBook:%28Lighting%29_Cook-Torrance

out vec4 color;
in vec2 ourUv;
in vec3 ourNormal, ourPosition, lightDir;
uniform vec3 diffuseColor, specularColor;
uniform float roughness, gaussian, reflectance;
uniform int distribution;
uniform mat4 view;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

float geometric(float ndotl, float ndoth, float vdoth, float ndotv)
{
    float p = 2.0 * ndoth;
	float q = 1.0 / vdoth;
	float g1 = (p * ndotv) * q;
	float g2 = (p * ndotl) * q;
	return min(1.0, min(g1, g2));
}

float beckmann_rough(float ndotv, float roughness)
{
    float r2 = roughness * roughness;
	float ndv2 = ndotv * ndotv;
	float ndv4 = ndv2 * ndv2;
	float br1 = exp((ndv2 - 1.0)/(r2 * ndv2));
	float br2 = r2 * ndv4;
    return br1 / br2;
}

float gaussian_rough(float ndoth, float gaussian, float roughness)
{
    return gaussian * exp(-acos(ndoth)/(roughness * roughness));
}

float schlick_fresnel(float reflectance, float vdoth) 
{
    return reflectance + pow(1.0 - vdoth, 5) * (1.0 - reflectance);
}

void main()
{
	//Temporary
    vec3 lightPos = normalize(view * vec4(0, 1, 0, 0)).xyz;
    
    vec3 n = normalize(ourNormal);
    vec3 v = normalize(-ourPosition);
    vec3 l = normalize(lightPos);
    vec3 h = normalize(v + l);
    vec3 r = reflect(-l, n);
    float ndotl = max(0.0, dot(n, l));
	float ndoth = max(0.0, dot(n, h));
	float vdoth = max(0.0, dot(v, h));
	float ndotv = max(0.0, dot(n, v));

	float rc = 0;
	//Gaussian distribution on microfacets
	if (distribution == 0)
	{
		rc = gaussian_rough(ndoth, gaussian, roughness);
	}

	//Beckmann distribution on microfacets
	if (distribution == 1)
	{
		rc = beckmann_rough(ndotv, roughness);
	}

	float g = geometric(ndotl, ndoth, vdoth, ndotv);
	float f = schlick_fresnel(reflectance, vdoth);

	float rs1 = f * g * rc;
	float rs2 = ndotv * ndotl;
	float rs = rs1 / rs2;

	vec3 final = max(0.0, ndotl) * (specularColor * rs + diffuseColor);
	color = vec4(final, 1.0);
}


