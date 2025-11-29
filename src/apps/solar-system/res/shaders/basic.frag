#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture_diffuse1;

uniform vec3 objectColor;
uniform bool useTexture;

void main()
{
    if (useTexture) {
        FragColor = texture(texture_diffuse1, TexCoord);
    } else {
        FragColor = vec4(objectColor, 1.0);
    }
}
