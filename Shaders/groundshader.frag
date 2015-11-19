#version 420
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourWorldPosition;
in vec3 ourColor;
uniform sampler2D colorRampTex;
uniform vec3[12] gradients; 
uniform sampler1D permutations;

vec3 fade(vec3 t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float perm(float x) {
	return texture(permutations, x / 256.0).r * 256;
}

float grad(float x, vec3 p) { 
	return dot(gradients[int(x * 256)], p);
}

float noise(vec3 p){
	vec3 P = mod(floor(p), 256.0);
	p -= floor(p);
	vec3 f = fade(p);
	float A = perm(P.x) + P.y;
	float AA = perm(A) + P.z;
	float AB = perm(A + 1) + P.z;
	float B = perm(P.x + 1) + P.y;
	float BA = perm(B) + P.z;
	float BB = perm(B + 1) + P.z;
	return mix(mix(mix(grad(perm(AA), p), grad(perm(BA), p + vec3(-1, 0, 0)), f.x),
         mix(grad(perm(AB), p + vec3(0, -1, 0)), grad(perm(BB), p + vec3(-1, -1, 0)), f.x), f.y),
	mix(mix(grad(perm(AA + 1), p + vec3(0, 0, -1)), grad(perm(BA + 1), p + vec3(-1, 0, -1)), f.x),
	mix(grad(perm(AB + 1), p + vec3(0, -1, -1)), grad(perm(BB + 1), p + vec3(-1, -1, -1)), f.x), f.y), f.z);
}

vec3 applyFog(vec3 rgb, float d) {
    const float b = 0.15;
    float fogAmount = 1.0 - exp(-d * b);
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
}

float heightFunction(vec3 p, float l, float d, float o, float x) {
	float v = 1.0;
	for (float i = 0; i <= o; ++i) {
			v += (noise(p) + x) * pow(l, -d * i);
			p *= l;
	}
	return 1.0 - abs(v);
}

void main()
{
    vec3 normalDir = normalize(ourNormal);
    vec3 lightDir = normalize(vec3(1,1,0));
    float ndotl = clamp(dot(normalDir, lightDir), 0.0, 1.0);
    float ndots = clamp(0.5 + 0.5 * dot(lightDir, normalize(ourWorldPosition)), 0.0, 1.0);
    float ndoti = clamp(dot(normalDir, normalize(lightDir * vec3(-1, 0, -1))), 0.0, 1.0);
    vec3 lighting = vec3(1.64,1.27,0.99) * ndotl;
    lighting += vec3(0.16,0.20,0.28) * ndots;
    lighting += vec3(0.40,0.28,0.20) * ndoti;
    vec3 texColor = texture2D(colorRampTex, vec2(ourColor.r, 0)).xyz;
    vec3 final = sqrt(texColor * lighting);
    color = vec4(final.xyz, 1.0);
}





 


