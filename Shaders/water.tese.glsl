#version 430

layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];
out vec3 tePosition;
out float teDisplacement;
out vec3 teNormal;
out vec3 tePatchDistance;

// Atmosphere variables
in vec3 tc_c0[];
in vec3 tc_c1[];
in float tc_t0[];
out vec3 te_c0;
out vec3 te_c1;
out float te_t0;

uniform vec3 wCameraPos;
uniform float globTime;
uniform float near;
uniform float far;
uniform float waveHeight;
uniform float waveFreq;
uniform int maxOctets;
uniform float quadtree_level;
uniform float maxTessLevel;
uniform int maxLODLevel;
uniform float detailCutoff;

#define M_PI 3.1415926535897932384626433832795

//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

float height(vec3 pos, float time, int octets) {
	const float angle_offset = M_PI / 1.7; // Just to introduce some randomness in animation
	const float anim_speed = 0.04;
	const float alpha = 0.25;
	const float freq_scale = waveFreq;
	float sum = 0;
	float angle = 0;

	for (int oct = 1; oct <= octets; ++oct) {
		angle += angle_offset;
        vec3 anim_vec = anim_speed * vec3(cos(angle), sin(angle), tan(angle));
        pos.x += M_PI / 2.0;
		sum += pow(alpha, oct) * (snoise(pow(2, oct) * (freq_scale * pos + time * anim_vec)) + 1.0) / 2.0;
	}

	return sum;
}

void main()
{
	te_c0 = gl_TessCoord.x * tc_c0[0] + gl_TessCoord.y * tc_c0[1] + gl_TessCoord.z * tc_c0[2]; 
	te_c1 = gl_TessCoord.x * tc_c1[0] + gl_TessCoord.y * tc_c1[1] + gl_TessCoord.z * tc_c1[2]; 
    te_t0 = gl_TessCoord.x * tc_t0[0] + gl_TessCoord.y * tc_t0[1] + gl_TessCoord.z * tc_t0[2]; 

    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];

	// Same length on all vertices
	float waterLevel = length(tcPosition[0]);
	// Total quadtree + tessellation level
	float detail = gl_TessLevelInner[0] * pow(2.0, quadtree_level);
	// Guaranteed max, but might overshoot in practice
	float detail_max = maxTessLevel * pow(2.0, maxLODLevel);
	float detail_cutoff = detailCutoff;
	float detail_norm = pow(detail / detail_max, detail_cutoff);

    vec3 flatPosition = normalize(p0 + p1 + p2);
    teNormal = normalize(n0 + n1 + n2);
    tePatchDistance = gl_TessCoord;

	int octets = int(ceil(detail_norm * float(maxOctets)));

	// Displacement should be [0,1], applied to normalized position
	teDisplacement = height(flatPosition, globTime, octets);
	tePosition = flatPosition + waveHeight * teDisplacement * teNormal;

	// Compute derivatives and adjust normal, makes sense if eps depends on detail
	// level in some way
	float eps = 1.0 / detail;
	float xDer, yDer, zDer;
	xDer = height(vec3(flatPosition.x + eps, flatPosition.y, flatPosition.z), globTime, octets);
	yDer = height(vec3(flatPosition.x, flatPosition.y + eps, flatPosition.z), globTime, octets);
	zDer = height(vec3(flatPosition.x, flatPosition.y, flatPosition.z + eps), globTime, octets);
	// XXX: I'm not sure about the scaling here. waveHeight scales the normalized position 
	// so should work fine with normal adjustment, but looks awful when scaling gradient.
	vec3 gradient = 1.0 / eps * vec3(xDer - teDisplacement, yDer - teDisplacement, zDer - teDisplacement);
	// So let's do a hack on the hack and see if it hacks. Less normal change with distance for crude LOD.
	gradient *= waveHeight * detail_norm;
	teNormal = normalize(teNormal - gradient);
}
