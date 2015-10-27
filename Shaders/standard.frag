#version 330 core

// This shader uses the Cook-Torrence shading model from http://content.gpwiki.org/D3DBook:%28Lighting%29_Cook-Torrance
// Check out this source for more information about PBR https://docs.google.com/document/d/1Fb9_KgCo0noxROKN4iT8ntTbx913e-t4Wc2nMRWPzNk/edit#

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
	return min(1.0, min((p * ndotv) * q, (p * ndotl) * q));
}

float beckmann_rough(float ndotv, float roughsqr)
{
	float ndv2 = ndotv * ndotv;
	float br1 = exp((ndv2 - 1.0)/(roughsqr * ndv2));
	float br2 = roughsqr * ndv2 * ndv2;
    return br1 / br2;
}

float gaussian_rough(float ndoth, float gaussian, float roughsqr) { return gaussian * exp(-acos(ndoth)/(roughsqr)); }
float schlick_fresnel(float reflectance, float vdoth) { return reflectance + pow(1.0 - vdoth, 5) * (1.0 - reflectance); }

void main()
{
	//Temporary
    vec3 lightPos = normalize(view * vec4(0, 1, 0, 0)).xyz;
    vec3 n = normalize(ourNormal);
    vec3 v = normalize(-ourPosition);
    vec3 l = normalize(lightPos);
    vec3 h = normalize(v + l);

    float ndotl = max(0.0, dot(n, l));
	float ndoth = max(0.0, dot(n, h));
	float vdoth = max(0.0, dot(v, h));
	float ndotv = max(0.0, dot(n, v));
	float roughsqr = roughness * roughness;

	float r = 0;
	//Gaussian distribution on microfacets
	if (distribution == 0)
	{
		r = gaussian_rough(ndoth, gaussian, roughsqr);
	}

	//Beckmann distribution on microfacets
	if (distribution == 1)
	{
		r = beckmann_rough(ndotv, roughsqr);
	}

	float g = geometric(ndotl, ndoth, vdoth, ndotv);
	float f = schlick_fresnel(reflectance, vdoth);
	float rs = (f * g * r) / (ndotv * ndotl);
	vec3 final = max(0.0, ndotl) * (specularColor * rs + diffuseColor);
	color = vec4(final, 1.0);
}


