#version 330 core
out vec4 FragColor;

in vec3 ourColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;

// additional
// in vec3 ourPosition;
// uniform vec3 ourColor;
uniform sampler2D addTexture;

void main()
{
    // FragColor = texture(ourTexture,TexCoord) * vec4(ourColor, 1.0);
    // additional
    // FragColor = ourColor;
    // FragColor = vec4(ourColor,1.0); 
    // FragColor = mix(texture(ourTexture,TexCoord),vec4(ourColor, 1.0),2);
    FragColor = mix(texture(ourTexture,TexCoord),texture(addTexture,TexCoord),0.2);
}
