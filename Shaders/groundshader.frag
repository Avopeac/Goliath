#version 420
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourWorldPosition;
in vec3 ourColor;
uniform sampler2D colorRampTex;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;

//Noise generator
vec3 fade(vec3 t) { return t * t * t * (t * (t * 6 - 15) + 10); }
float perm(float x) { return texture(permutationTex, x / 256.0).r * 256.0; }
float grad(float x, vec3 p) { return dot(2 * texture(gradientTex, x).rgb - 1, p); }

float noise(vec3 p){
	vec3 floorp = floor(p);
	vec3 P = mod(floorp, 256.0);
    p -= floorp;
    vec3 f = fade(p);
	float A = perm(P.x) + P.y;
	float AA = perm(A) + P.z;
	float AB = perm(A + 1) + P.z;
	float B = perm(P.x + 1) + P.y;
	float BA = perm(B) + P.z;
	float BB = perm(B + 1) + P.z;

	return mix(
		mix(mix(grad(perm(AA), p),
			grad(perm(BA), p + vec3(-1, 0, 0)), f.x),
        mix(grad(perm(AB), p + vec3(0, -1, 0)),
			grad(perm(BB), p + vec3(-1, -1, 0)), f.x), f.y),
	    mix(mix(grad(perm(AA + 1), p + vec3(0, 0, -1)),
			grad(perm(BA + 1), p + vec3(-1, 0, -1)), f.x),
			mix(grad(perm(AB + 1), p + vec3(0, -1, -1)),
            grad(perm(BB + 1), p + vec3(-1, -1, -1)), f.x), f.y), f.z);
}


float heightFunction(vec3 p, float l, float d, float o, float x) {
    float v = 1.0;
	float i;
	v += (noise(p) + x);
	p *= l;

    for (i = 1; i <= o; ++i) {
        v += (abs(noise(p)) + x) * pow(l, -d * i);
        p *= l;
    }
	return 4.0 + (pow(v, 2) - 1.0) * 0.15;
}

void main()
{
	//Find tangent space basis
	vec3 normalDir = normalize(ourNormal);
	vec3 sphereDir = normalize(ourWorldPosition);
	//Perturbing normal with noise
	const float epsilon = 0.01 * -ourPosition.z; //Do some distance function here
	const float oneOverEpsilon = 1.0 / epsilon;
	const float scaleFactor = 0.1;
	const float lacunarity = 2.0;
	const float dimensionality = 0.9;
	const float octaves = 12.0;
	const float offset = 0.0;
	float h0 = heightFunction(ourWorldPosition, lacunarity, dimensionality, octaves, offset);
	float hx = heightFunction(ourWorldPosition + vec3(epsilon,0,0), lacunarity, dimensionality, octaves, offset);
    float hy = heightFunction(ourWorldPosition + vec3(0,epsilon,0), lacunarity, dimensionality, octaves, offset);
    float hz = heightFunction(ourWorldPosition + vec3(0,0,epsilon), lacunarity, dimensionality, octaves, offset);
    vec3 df = vec3(hx - h0, hy - h0, hz - h0) * scaleFactor * oneOverEpsilon;
	normalDir = normalize(sphereDir - df);

	//Coloring things
	float height = length(ourWorldPosition) - 4.0;
	float slope = (1.0 + dot(sphereDir, normalDir)) * 0.5;
	float equatorial = abs(sphereDir.y);
	vec3 texColor = slope * texture(colorRampTex, vec2(h0, 0)).rgb;

	//Lighting
    vec3 lightDir = normalize(vec3(0,1,0));
    float ndotl = clamp(dot(normalDir, lightDir), 0.0, 1.0);
    float ndots = clamp(0.5 + 0.5 * dot(lightDir, normalize(ourPosition)), 0.0, 1.0);
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
    vec3 final = (texColor * lighting + 0.5 * sunColor * specular);
    color = vec4(final, 1.0);
}


 


