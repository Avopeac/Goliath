#version 330
out vec4 color;
in vec3 viewPos;
in vec3 texDir;

//Lights
uniform int lights;
uniform vec3 directions[20];
uniform vec3 intensities[20];
uniform vec3 planetViewPos;
uniform float planetRadius;
uniform float atmosphereRadius;
uniform float viewDepth;
uniform vec4 scattering;
uniform mat4 view;
uniform samplerCube skybox;

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


vec3 atmosphericRadiance(int samples, vec3 origin, vec3 ray)
{
	int i; //Loop variable
	float sampleStep = 1.0 / samples;
	vec3 radiance = texture(skybox, texDir).xyz;
	bool atmosphereIntersection = raySphereIntersection(origin, ray, planetViewPos, atmosphereRadius);
	if (!atmosphereIntersection) { return radiance; } //Miss everything
	float l1 = intersection1, l2 = intersection2; //Atmosphere hits
	bool planetIntersection = raySphereIntersection(origin, ray, planetViewPos, planetRadius);
	float l0 = intersection1; //Nearest planet hit
	if (planetIntersection && intersection2 < 0.0) { return radiance; } //Under ground
	vec3 dp = ray;
	vec3 p0 = origin;

	//Set l0 to the current view depth (along ray)
	if (!planetIntersection){ //Ray outside planet surface
		if (l2 < 0.0) { //Ray starts inside atmosphere to boundary
			l0 = l1;
		} else { //Ray starts goes boundary to boundary
			p0 += dp * l1;
			l0 = l2 - l1;
		}
	} else { //Ray hits plantery surface
		if (l1 < l0){ //Ray goes from boundary to planet surface
			p0 += dp * l1;
			l0 -= l1;
		} else { //Ray goes from inside atmosphere to planet surface
			l0 = l0;
		}
	}

	//p1 is point on end of view depth and dp the raymarching step size
	vec3 p1 = p0 + dp * l0;
	dp = p1 - p0;
	dp *= sampleStep;
	float dl = l0 * sampleStep / viewDepth;
	l1 = scattering.a; 
	
	vec3 p,b;
	float h;
	for (p = p1, i = 0; i < samples; p-=dp, ++i){
		b = normalize(p) * planetRadius;
		h = length(p - b); //Height above planet surface
		h = exp(h / atmosphereRadius);
		b = scattering.rgb * h * dl;
		radiance *= b;
		radiance += b;
	}

	if (radiance.r < 0.0) { radiance.r = 0.0; }
	if (radiance.g < 0.0) { radiance.g = 0.0; }
	if (radiance.b < 0.0) { radiance.b = 0.0; }
	return radiance;
}

void main(void)
{
    vec3 cameraViewPos = view[3].xyz;
    vec3 ray = normalize(viewPos - cameraViewPos);
	vec3 radiance = atmosphericRadiance(5, cameraViewPos, ray);
	color = vec4(radiance, 1.0);
}


