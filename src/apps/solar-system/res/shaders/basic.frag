#version 330 core
out vec4 fragcolor;

in vec2 texcoord;
uniform sampler2d texture_diffuse1;

uniform vec3 objectcolor;
uniform bool usetexture;

void main()
{
    if (usetexture) {
        fragcolor = texture(texture_diffuse1, texcoord);
    } else {
        fragcolor = vec4(objectcolor, 1.0);
    }
}
