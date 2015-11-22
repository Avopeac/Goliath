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
    for (float i = 0; i <= o; ++i) {
        v += (noise(p) + x) * pow(l, -d * i);
        p *= l;
    }
	return 4.0 + v * 0.1;
}

void main()
{
    float epsilon = clamp(pow(-ourPosition.z - 8, 5), 0.001, 1.0);
    vec3 tangent = normalize(cross(ourNormal, vec3(1,0,0)));
    vec3 bitangent = normalize(cross(ourNormal, tangent));
    float h0 = heightFunction(ourWorldPosition, 2.0, 1.0, 10, 0);
	float hx = heightFunction(ourWorldPosition + vec3(epsilon,0,0), 2.0, 1.0, 10, 0);
    float hy = heightFunction(ourWorldPosition + vec3(0,epsilon,0), 2.0, 1.0, 10, 0);
    float hz = heightFunction(ourWorldPosition + vec3(0,0,epsilon), 2.0, 1.0, 10, 0);
    vec3 df = vec3(hx - h0, hy - h0, hz - h0) / epsilon;
    vec3 normalDir = normalize(ourNormal - df);

    vec3 lightDir = normalize(vec3(0,1,0));
    float ndotl = clamp(dot(normalDir, lightDir), 0.0, 1.0);
    float ndots = clamp(0.5 + 0.5 * dot(lightDir, normalize(ourPosition)), 0.0, 1.0);
    float ndoti = clamp(dot(normalDir, normalize(lightDir * vec3(-1, 0, -1))), 0.0, 1.0);
    vec3 lighting = vec3(1.64,1.27,0.99) * ndotl;
    lighting += vec3(0.16,0.20,0.28) * ndots;
    lighting += vec3(0.40,0.28,0.20) * ndoti;
    vec3 texColor = texture2D(colorRampTex, vec2(ourColor.r, 0)).xyz;
    vec3 final = sqrt(texColor * lighting) * 0.5;
    color = vec4(final, 1.0);
}






 


