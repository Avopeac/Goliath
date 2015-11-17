#version 330 core
out vec4 color;
in vec2 ourUv;
in vec3 ourNormal;
in vec3 ourPosition;
in vec3 ourColor;
uniform sampler2D colorRampTex;
void main()
{
	vec3 normalDir = normalize(ourNormal);
	vec3 lightDir = normalize(vec3(1,1,0));
	float ndotl = dot(normalDir, lightDir);
	float diffuse = max(0.0, ndotl);

	vec4 c = texture(colorRampTex, vec2(ourColor.r, 0.5));
	color = vec4(c.xyz * diffuse, 1.0);
}




 


