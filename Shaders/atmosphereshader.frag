#version 330
out vec4 color;
in vec2 ourUv;
uniform sampler2D texUnit;
uniform sampler2D depthUnit;
//Transform information
uniform mat4 inverseViewProj;
uniform vec3 cameraWorldPos;
uniform vec3 planetWorldPos;
uniform float planetRadius;
uniform float atmosphereRadius;
//Scale information
uniform float scaleRadius;
uniform float scaleHeight;
uniform float scaleOverHeight;
//Scattering information
uniform float miePhaseG;
uniform float miePhaseGSqr;
uniform float mieConstant;
uniform float rayleighConstant;
uniform float mieConstant4Pi;
uniform float rayleighConstant4Pi;
//Solar information
uniform vec3 sunDirection;
uniform float sunIntensity;
uniform vec3 sunInverseWavelength;
//Keeps track of intersections
float intersection1 = -1.0;
float intersection2 = -1.0;
bool raySphereIntersection(vec3 o, vec3 ray, vec3 c, float rad) 
{
    vec3 dir = o - c;
    float dist = length(dir);
    float angle = dot(ray, dir);
    float d = angle * angle - dist * dist + rad * rad;
    if (d < 0) { return false; }
    float t = sqrt(d);
    float t0 = -angle + t;
    float t1 = -angle - t;
    //Set t0 to be the nearest true intersection
	float temp = 0;
    if (abs(t0) > abs(t1)) { temp = t0; t0 = t1; t1 = temp; }
	if (t0 < 0.0) { temp = t0; t0 = t1; t1 = temp; }
	if (t0 < 0.0) { return false; }
	//Set "global" variables to hold newest intersections along the given ray
	intersection1 = t0;
    intersection2 = t1;
    return true;
}

float mie(float cosTetha, float cosTethaSqr, float g, float gSqr) {
    float nom = 3.0 * (1.0 - gSqr) * (1.0 +	cosTethaSqr);
    float denom = 2.0 * (2.0 + gSqr) * pow((1.0 + gSqr - 2.0 * g * cosTetha), 1.5);
    return nom * denom;
}

float rayleigh(float cosTethaSqr){ return 0.75 * (1.0 + cosTethaSqr); }

float scale(float cosTetha) {
    float x = 1.0 - cosTetha;
    return 0.25 * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

vec4 atmosphere(float samples, vec3 origin, vec3 ray) {
    vec3 pa, pb;
    //A and B is first and second ray points, X is current step
	float ta0, ta1, tp0, tp1;
    //Distances along ray for each intersection
	bool atmosphereIntersection = raySphereIntersection(origin, ray, planetWorldPos, atmosphereRadius);
    //Find atmosphere intersection
	if (!atmosphereIntersection) { return vec4(0,0,0,1); } //Not hitting atmosphere at all
	ta0 = intersection1;
    ta1 = intersection2;
    bool planetIntersection = raySphereIntersection(origin, ray, planetWorldPos, planetRadius);
    //Find planet intersection
	//if (planetIntersection && intersection2 < 0.0) { return vec4(0,0,0,1); } //Under ground
	tp0 = intersection1;
    tp1 = intersection2;
    //Find pa and pb
	if (!planetIntersection){
        //No planet intersection
		if (ta1 < 0.0) {
            //Hitting atmosphere once
			pa = origin + 0 * ray;
            pb = origin + ta0 * ray;
        } else {
            //Passing through
			pa = origin + ta0 * ray;
            pb = origin + ta1 * ray;
        }
	} else {
        //Intersecting planet surface
		if (ta0 < tp0) {
            //Outside atmosphere
			pa = origin + ta0 * ray;
            pb = origin + tp0 * ray;
        } else {
			pa = origin + tp0 * ray;
			pb = origin + ta0 * ray;
        }
	}
    
    //Raymarching step
	float sampleStep = length(pb - pa) / samples;
    float scaledLength = sampleStep / scaleRadius;
	vec3 px = pa;
	vec3 dp = normalize(pb - pa);
	vec3 color = vec3(0,0,0);
    for (float i = 0.0; i < samples; ++i) {
        float height = length(px);
        float depth = exp(scaleOverHeight * (planetRadius - height));

        float lightAngle = dot(sunDirection, px) / height;
        float cameraAngle = dot(dp, px) / height;

        float scatter = depth * (scale(lightAngle) + scale(cameraAngle));
		vec3 attenuate = exp(-scatter * (sunInverseWavelength * rayleighConstant4Pi + mieConstant4Pi));
        color = color + attenuate * depth * scaledLength;
        px = px + sampleStep * dp;
    }

	vec3 c0 = color * (sunInverseWavelength * rayleighConstant * sunIntensity);
    vec3 c1 = color * mieConstant * sunIntensity;
    float cosTetha = dot(normalize(sunDirection), ray);
    float cosTethaSqr = cosTetha * cosTetha;
    vec4 atmosphere = vec4(rayleigh(cosTethaSqr) * c0 + mie(cosTetha, cosTethaSqr, miePhaseG, miePhaseGSqr) * c1, 1.0);
    atmosphere.a = atmosphere.b;
    return atmosphere;
}



void main(void)
{
	//Compute clip space position of pixel
    vec4 clipSpacePos;
    clipSpacePos.xy = ourUv * 2.0 - 1.0;
    clipSpacePos.z = texture2D(depthUnit, ourUv).r * 2.0 - 1.0;
    clipSpacePos.w = 1.0;
	//Compute view space position of pixel
    vec4 worldSpacePos = inverseViewProj * clipSpacePos;
    worldSpacePos.xyz /= worldSpacePos.w;
	//Compute the atmospheric scattering and add to previous texture
	vec4 scatteredRadiance = atmosphere(5, cameraWorldPos, normalize(worldSpacePos.xyz - cameraWorldPos));
    color = texture2D(texUnit, ourUv) + vec4(scatteredRadiance.rgb, scatteredRadiance.a);
}


