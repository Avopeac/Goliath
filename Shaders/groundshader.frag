#version 330
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourColor;
in vec3 ourScaledPosition;

in vec3 c0;
in vec3 c1;
in float t0;

uniform float cameraHeight;
uniform sampler2D colorRampTex;
uniform sampler1D permutationTex;
uniform sampler1D gradientTex;
uniform sampler2D groundNormalTex;
uniform sampler2D grassNormalTex;
uniform sampler2D rockNormalTex;
uniform mat4 view;
uniform vec3 lightDir;

//Noise helper functions
/*vec3 fade(vec3 t) { return t * t * t * (t * (t * 6 - 15) + 10); }
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
}*/

void main()
{
	vec3 normalDir = normalize(ourNormal);
	vec3 groundNormals = 2.0 * texture(groundNormalTex, ourUv).rgb - 1.0;
	vec3 grassNormals = 2.0 * texture(grassNormalTex, ourUv).rgb - 1.0;
	vec3 rockNormals = 2.0 * texture(rockNormalTex, ourUv).rgb - 1.0;

	vec3 blendWeights;
	blendWeights.x = 1.0 - (ourColor.r - 1);
	blendWeights.y = 1.0 - (ourColor.r - 0.5);
	blendWeights.z = 1.0 - (ourColor.r - 0.0);

	vec3 normalTexDir = normalize(blendWeights.x * groundNormals + 
	blendWeights.y * grassNormals + 
	blendWeights.z * rockNormals);

	normalDir = normalize(normalDir - 0.2 * normalTexDir);
	float slope = 1.0 - abs(dot(normalDir, ourScaledPosition)) + 0.6;

	vec3 texColor = texture(colorRampTex, vec2(ourColor.r * slope, 0)).rgb;

	//Lighting
    float ndotl = clamp(dot(normalDir, lightDir), 0.0, 1.0);
	float ndots = clamp(0.5 + 0.5 * ndotl, 0.0, 1.0);
	float specular = 0.0;
	if (ndotl > 0.0){
		vec3 viewDir = normalize(-ourPosition);
		vec3 reflDir = normalize(reflect(-lightDir, normalDir));
		vec3 halfDir = normalize(normalDir + reflDir);
		specular = clamp(-dot(halfDir, viewDir), 0.0, 1.0);
		specular = pow(specular, 200);
	}

	//Light colors	
	vec3 sunColor = vec3(1.64,1.27,0.99);
	//vec3 skyColor = vec3(0.16,0.20,0.28);
	vec3 lighting =  sunColor * ndotl;
	//lighting += skyColor * ndots;
	vec3 final = 0.5 * (texColor * lighting + 5.0 * lighting * specular);
	vec3 atmosphereColor = c0 + 0.25 * c1;

	float falloff = pow(t0, 0.19);

	atmosphereColor = 1.0 - exp(atmosphereColor * -0.9);
	final *= 1.0 + atmosphereColor.b;
	color = vec4(final + falloff * atmosphereColor, 1.0);
}


 


