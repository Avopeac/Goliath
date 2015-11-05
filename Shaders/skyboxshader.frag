#version 330
out vec4 color;
in vec3 texDir;
in vec3 viewPos;
uniform samplerCube skybox;
uniform vec3 planetViewPos;
uniform float planetRadius;
uniform float atmosphereRadius;
uniform float scaleHeight;
uniform mat4 view;

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

vec3 atmosphericRadiance(int samples, vec3 origin, vec3 ray) {
	float sampleStep = 1.0 / samples;
	vec3 radiance = texture(skybox, texDir).rgb;
	bool planetIntersection = false;
	bool atmosphereIntersection = false;
	float l0, l1, l2, dl, h;
	planetIntersection = raySphereIntersection(origin, ray, planetViewPos, planetRadius);
	if (planetIntersection && (intersection1 > 0.0) && (intersection2 < 0.0)) { return radiance; }
	l0 = intersection1;
	atmosphereIntersection = raySphereIntersection(origin, ray, planetViewPos, atmosphereRadius);
	l1 = intersection1;
	l2 = intersection2;

	//Set up initial ray for marching
	vec3 p0 = origin;
	vec3 dp = ray;
	vec3 p1;
	if (!planetIntersection){
		//Missed everything
		if (!atmosphereIntersection) { return radiance; }
		//Inside to boundary
		if (l2 <= 0.0) {
			l1 = l1;
		} else { 
			p0 = p0 + (l1 + dp);
			l0 = l2 - l1;
		}
		//Add up lightsources TODO: add actual lightsources
		for (int i = 0; i < 2; ++i) {
			if (dot(vec3(0,1,0), normalize(dp)) >= 0.0) {
				radiance += vec3(0.01,0.01,0.01);
			} 
		}
	} else {
		if (l0 < l1) { atmosphereIntersection = false; }	
		if (!atmosphereIntersection) {
			l0 = l0; //Useless?
		} else {
			p0 = p0 + (l1 + dp);
			l0 = l0 - l1;
		}
	}

	dp *= l0;
	p1 = p0 + dp;
	dp *= sampleStep;

	float qqq = dot(normalize(p1), normalize(vec3(0,1,0)));
	const float viewDepth = 5.0f;
	const float atmosHeight = 4.0;
	const vec4 scattering = vec4(0.198141888310295, 0.465578010163675, 0.862540960504986, 0.0000000000000000000000025);
	vec3 b;
	vec3 p = p1;
	dl = l0 * sampleStep/viewDepth;
	for (int i = 0; i < samples; ++i){
		p-=dp;
		raySphereIntersection(p, normalize(p), planetViewPos, atmosphereRadius);
		h = exp(intersection1 / atmosHeight) / 2.78;
		b = scattering.rgb * h * dl;
		radiance.r *= 1.0 - b.r;
		radiance.g *= 1.0 - b.g;
		radiance.b *= 1.0 - b.b;
		radiance += b * qqq;
	}

	radiance = max(vec3(0.0), radiance);
	h = 0.0;
	if (h < radiance.r) { h = radiance.r; }
	if (h < radiance.g) { h = radiance.g; }
	if (h < radiance.b) { h = radiance.b; }
	if (h > 1.0) {
		h = 1.0 / h;
		radiance.r *= h;
		radiance.g *= h;
		radiance.b *= h;
	}

	return radiance;
}


void main(void)
{
    vec3 cameraViewPos = view[3].xyz;
    vec3 ray = normalize(viewPos - cameraViewPos);
	vec3 radiance = atmosphericRadiance(5, cameraViewPos, ray);
	color = vec4(radiance, 1.0);
}


