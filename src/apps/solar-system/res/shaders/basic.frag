#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
// in vec3 ourColor;

uniform sampler2D texture1;
uniform vec3 fracColor;

// additional
uniform float saturation;
uniform vec2 TexOffset;

void main()
{
    // used when we have defined color atributes in vertex array (rainbow tetra)
    // FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
    
    // used when we set the color of tetra using color edit + saturation 
    //FragColor = mix(texture(texture1, TexCoord),vec4(fracColor, 1.0),saturation);

    FragColor = mix(texture(texture1, TexCoord + TexOffset),vec4(fracColor, 1.0),saturation);
    
    // used when we have defined color atributes in vertex array (rainbow tetra) + saturation
    // FragColor = mix(texture(texture1, TexCoord),vec4(ourColor, 1.0),saturation);
}
