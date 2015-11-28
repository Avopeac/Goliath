#version 330
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourColor;
uniform sampler2D colorRampTex;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;
uniform mat4 view;

//Noise helper functions
vec3 fade(vec3 t) { return t * t * t * (t * (t * 6 - 15) + 10); }
float perm(float x) { return texture(permutationTex, x / 256.0).r * 256.0; }
float grad(float x, vec3 p) { return dot(2 * texture(gradientTex, x).rgb - 1, p); }
float noise(vec3 p){
	//Get lattice points and position within
	vec3 floorp = floor(p);
	p -= floorp;
	vec3 P = mod(floorp, 256.0);
	//Get hash for lattice gradient 
	float g000 = perm(P.x + perm(P.y + perm(P.z)));
	float g001 = perm(P.x + perm(P.y + perm(P.z + 1)));
	float g010 = perm(P.x + perm(P.y + 1 + perm(P.z)));
	float g011 = perm(P.x + perm(P.y + 1 + perm(P.z + 1)));
	float g100 = perm(P.x + 1 + perm(P.y + perm(P.z)));
	float g101 = perm(P.x + 1 + perm(P.y + perm(P.z + 1)));
	float g110 = perm(P.x + 1 + perm(P.y + 1 + perm(P.z)));
	float g111 = perm(P.x + 1 + perm(P.y + 1 + perm(P.z + 1)));
	//Get lattice gradient
	float n000 = grad(g000, p);
	float n100 = grad(g100, p - vec3(1,0,0));
	float n010 = grad(g010, p - vec3(0,1,0));
	float n110 = grad(g110, p - vec3(1,1,0));
	float n001 = grad(g001, p - vec3(0,0,1));
	float n101 = grad(g101, p - vec3(1,0,1));
	float n011 = grad(g011, p - vec3(0,1,1));
	float n111 = grad(g111, p - vec3(1,1,1));
	//Get quintic curve values
	vec3 f = fade(p);
	//Interpolate along axes	
	float nx00 = mix(n000, n100, f.x);
	float nx01 = mix(n001, n101, f.x);
	float nx10 = mix(n010, n110, f.x);
	float nx11 = mix(n011, n111, f.x);
	float nxy0 = mix(nx00, nx10, f.y);
	float nxy1 = mix(nx01, nx11, f.y);
	return mix(nxy0, nxy1, f.z);
}


float heightFunction(vec3 p, float l, float d, float o, float x) {
    float i, v = 1.0;
	v += (noise(p) + x);
	p *= l;
    for (i = 1; i <= o; ++i) {
        v += (abs(noise(p)) + x) * pow(l, -d * i);
        p *= l;
    }
	return 4.0 + (pow(v, 3) - 1.5) * 0.1;
}

void main()
{
	vec3 lightDir = normalize(vec3(0,1,0));
	vec3 normalDir = normalize(ourNormal);
	//Perturbing normal with noise
	//const float epsilon = 0.05 * -ourPosition.z; //Do some distance function here
	//const float oneOverEpsilon = 1.0 / epsilon;
	//const float scaleFactor = 0.25;
	//const float lacunarity = 2.0;
	//const float dimensionality = 0.9;
	//const float octaves = 24;
	//const float offset = 0.0;
	//float h0 = heightFunction(ourWorldPosition, lacunarity, dimensionality, octaves, offset);
	//float hx = heightFunction(ourWorldPosition + vec3(epsilon,0,0), lacunarity, dimensionality, octaves, offset);
	//float hy = heightFunction(ourWorldPosition + vec3(0,epsilon,0), lacunarity, dimensionality, octaves, offset);
    //float hz = heightFunction(ourWorldPosition + vec3(0,0,epsilon), lacunarity, dimensionality, octaves, offset);
    //vec3 df = vec3(hx - h0, hy - h0, hz - h0) / epsilon;
	//normalDir = normalize(normalDir - df * scaleFactor);

	//Coloring things
	float height = ourColor.r;
	vec3 texColor = texture(colorRampTex, vec2(height, 0)).rgb;

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
		specular = pow(specular, 200);
	}

	//Light colors	
	vec3 sunColor = vec3(1.64,1.27,0.99);
	vec3 skyColor = vec3(0.16,0.20,0.28);
	vec3 reflSunColor = vec3(0.40,0.28,0.20);
    vec3 lighting =  sunColor * ndotl;
	lighting += skyColor * ndots;
    lighting += reflSunColor * ndoti;
    vec3 final = texColor * (lighting + sunColor * specular);
    color = vec4(final, 1.0);
}


 


