#version 330 core

out vec4 color;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourWorldPosition;

vec3 applyFog(vec3 rgb, float d) {
    const float b = 0.15;
    float fogAmount = 1.0 - exp(-d * b);
    vec3  fogColor  = vec3(0.5,0.6,0.7);
    return mix( rgb, fogColor, fogAmount );
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
    vec3 final = lighting;
    final = applyFog(final, -ourPosition.z);
    color = vec4(final.xyz, 1.0);
}
