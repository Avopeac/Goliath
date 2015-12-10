#version 330
out vec4 color;
in vec3 c0;
in vec3 c1;
in vec3 t0;
uniform float g;
uniform float g2;
uniform vec3 lightDir;

// Calculates the Mie phase function
float getMiePhase(float fCos, float fCos2, float g, float g2)
{
    return 1.5 * ((1.0 - g2) / (2.0 + g2)) * (1.0 + fCos2) / pow(1.0 + g2 - 2.0 * g * fCos, 1.5);
}

// Calculates the Rayleigh phase function
float getRayleighPhase(float fCos2)
{
    return 0.75 + 0.75 * fCos2;
}

void main(void)
{
	float fCos = dot(lightDir, t0) / length(t0);
	float fCos2 = fCos * fCos;
	vec3 col = getRayleighPhase(fCos2) * c0 + getMiePhase(fCos, fCos2, g, g2) * c1;
	const float hdrExposure = 0.9;
	col = 1.0 - exp(col * -hdrExposure);
    color = vec4(col, col.b);
}



