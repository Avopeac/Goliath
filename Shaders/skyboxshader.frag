#version 330
out vec4 color;
in vec3 texDir;
uniform samplerCube skybox;
void main(void)
{
    color = texture(skybox, texDir);
}
