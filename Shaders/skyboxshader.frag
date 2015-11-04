#version 330
out vec4 color;
in vec3 texDir;
in vec3 viewPos;
uniform samplerCube skybox;
//uniform vec3 cameraViewPos;
uniform vec3 planetViewPos;
uniform float planetRadius;
uniform float atmosphereRadius;
uniform mat4 view;

vec2 raySphereIntersection(vec3 o, vec3 ray, vec3 c, float rad) 
{
    vec3 dir = o - c;
    float dist = length(dir);
    float angle = dot(ray, dir);
	float y = angle * angle - dist * dist + rad * rad; // >= 0
    float x = sqrt(y);
	return vec2(-angle + x, -angle - x);
}

float furthestPointIntersection(vec2 intersection)
{
	float furthest = intersection.y;
	if (abs(intersection.x) > abs(intersection.y))
	{
		furthest = intersection.x;
	}
	return furthest;
}

float nearestPointIntersection(vec2 intersection)
{
	float nearest = intersection.y;
	if (abs(intersection.x) < abs(intersection.y))
	{
		nearest = intersection.x;
	}
	return nearest;
}

void main(void)
{
	/*vec3 cameraViewPos = view[3].xyz;
	vec3 ray = normalize(viewPos - cameraViewPos);
	vec2 intersectionAtmosphere = raySphereIntersection(cameraViewPos, ray, planetViewPos, atmosphereRadius);
	float t1 = nearestPointIntersection(intersectionAtmosphere);
	float t2 = furthestPointIntersection(intersectionAtmosphere);
	vec3 start = cameraViewPos + t1 * ray;
	vec3 end = cameraViewPos + t2 * ray;

	//Raymarch
	vec3 result = vec3(0, 0, 0);
	int samples = 5;
	for (int n = 0; n < samples; ++n)
	{
		
	}
	*/
	color = texture(skybox, texDir);
	//if (t1 >= 0)
//	{
	//	color += 0.1 * vec4(0,0.5,1.0,1.0);
//	}
}


