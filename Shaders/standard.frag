#version 330 core
// This shader uses the Cook-Torrence shading model from http://content.gpwiki.org/D3DBook:%28Lighting%29_Cook-Torrance
// Check out this source for more information about PBR https://docs.google.com/document/d/1Fb9_KgCo0noxROKN4iT8ntTbx913e-t4Wc2nMRWPzNk/edit#
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
uniform mat4 view;
//Lights
uniform int lights;
uniform vec3 directions[20];
uniform vec3 intensities[20];
//Material properties
uniform vec3 albedo;
uniform float roughness;
uniform float gaussian;
uniform float absorption;
uniform float refraction;

float geometric(float ndotl, float ndoth, float vdoth, float ndotv)
{
    float q = 1.0 / vdoth;
    return min(1.0, min((2.0 * ndoth * ndotv) * q, (2.0 * ndoth * ndotl) * q));
}

float gaussian_rough(float ndoth, float gaussian, float roughSquared) {
    float acosSquared = pow(acos(ndoth), 2.0);
    return gaussian * exp(-acosSquared/(roughSquared));
}

float schlick_fresnel(float refraction, float vdoth) {
    return refraction + pow(1.0 - vdoth, 5) * (1.0 - refraction);
}

void main()
{
    //Numerically approximated constants
	const float pi = 3.14159265359;
    vec3 n = normalize(ourNormal);
    vec3 v = normalize(-ourPosition);
    float ndotv = max(0.0, dot(n, v));
	//For each light source
    vec3 fSpecular = vec3(0);
    vec3 fDiffuse = vec3(0);
	vec3 fAmbient = vec3(0);
    for (int i = 0; i < lights; ++i)
	{
		//There's no guarantee that directions are in view-space
        vec3 l = normalize(vec3(view * vec4(directions[i], 0.0))); 
        float ndotl = max(0.0, dot(n, l));
		//No light transfer between surface and light if < 0.0
        if (ndotl > 0.0)
		{
			float roughSquared = pow(roughness, 2.0);
            vec3 h = normalize(v + l);
            float ndoth = max(0.0, dot(n, h));
            float vdoth = max(0.0, dot(v, h));
            //Gaussian distribution on microfacets
			float rough = gaussian_rough(ndoth, gaussian, roughSquared);
            float fresnel = schlick_fresnel(refraction, vdoth);
            float geometry = geometric(ndotl, ndoth, vdoth, ndotv);
            float specular = (fresnel * geometry * rough) / (pi * ndotl * ndotv);
            fSpecular += ndotl * intensities[i] * specular * (1.0 - absorption);
			fDiffuse += ndotl * albedo * absorption;
        }
		//Fake global illumination
		fAmbient += 0.1 * (intensities[i] + albedo);
    }	
	//Average the results
	vec3 final = (fAmbient + fDiffuse + fSpecular) / lights;
	color = vec4(final, 1.0);
}







