#version 330 core

out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
uniform bool directional[4];
uniform vec3 lights[4];
uniform vec3 lightColor[4];
uniform mat4 view;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
void main()
{
    vec3 lightPos = (view * vec4(0,1,2,0)).xyz; //Temporary
    vec3 n = normalize(ourNormal);
    vec3 v = normalize(-ourPosition);
    vec3 l = normalize(lightPos);
    vec3 r = reflect(l,n);
    float ndotl = max(0.0, dot(n,l));
    vec4 diff = clamp(vec4(1.0) * 0.7 * ndotl, 0, 1);
    vec4 spec = vec4(0.0);
    if (ndotl > 0.0) {
        float rdotv = max(0.0, dot(r,v));
        spec = clamp(vec4(1.0) * 0.3 * pow(rdotv, 90.0), 0, 1);
    }
    color = diff + spec;
}

