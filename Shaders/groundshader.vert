#version 420
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 color;
out vec3 ourPosition;
out vec3 ourWorldPosition;
out vec3 ourNormal;
out vec2 ourUv;
out vec3 ourColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

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
	
	vec4 modelPos = model * vec4(position, 1);
	if (color.b < 1.0){
		vec3 normalizedPos = normalize(vec3(modelPos));
		modelPos = vec4(heightFunction(normalizedPos, 2.0, 0.9, 14, 0) * normalizedPos, 1);  //Just trying offsetting vertex positions in shader, comment out if you need to test CPU code.
	}

    vec4 viewPos = view * modelPos;
	ourPosition = viewPos.xyz;
	ourWorldPosition = modelPos.xyz;
	ourNormal = normalize(normal);
    ourUv = uv;
	ourColor = color;
	//float far =	1000.0;
	//float c = 1.0;
	vec4 clipPos = proj * viewPos;
	//clipPos.z = (2.0 * log(c * clipPos.w + 1.0) / log(c * far +  1) - 1) * clipPos.w;
    gl_Position = clipPos;
}
