#version 330
out vec4 color;
in vec3 texDir;
uniform mat4 view;
uniform samplerCube skybox;

void main(void)
{
	color = texture(skybox, texDir);
}


