#version 330 core
out vec4 fragcolor;

in vec2 texcoord;
uniform sampler2d texture_diffuse1;

void main()
{
    fragcolor = texture(texture_diffuse1, texcoord);
}
