#version 330
out vec4 color;
in vec2 ourUv;
uniform sampler2D texUnit;
uniform bool horizontal;
uniform int size;
void main(void)
{
	//Find texel size
	vec2 offsets = 1.0 / textureSize(texUnit, 0);
	vec3 results = texture(texUnit, ourUv).rgb;
	//Horizontal or vertical convolution kernel
	if (horizontal)
	{
		for(int i = 0; i < size; ++i)
		{
			results += texture(texUnit, ourUv + vec2(offsets.x * i, 0)).rgb;
			results += texture(texUnit, ourUv - vec2(offsets.x * i, 0)).rgb;
		}
	} else 
	{
		for(int i = 0; i < size; ++i)
		{
			results += texture(texUnit, ourUv + vec2(0, offsets.y * i)).rgb;
			results += texture(texUnit, ourUv - vec2(0, offsets.y * i)).rgb;
		}
		
	}
	
	//Average the results
	color = vec4(results / vec3(2.0 * size + 1), 1.0);
}
