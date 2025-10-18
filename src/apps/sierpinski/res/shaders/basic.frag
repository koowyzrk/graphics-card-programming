#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 fracColor;

void main()
{
    FragColor = texture(texture1, TexCoord) * vec4(fracColor, 1.0);
}
