#version 330 core
out vec4 FragColor;
// uniform vec4 ourColor; // we set this variable in the OpenGL code.
in vec3 ourColor;
in vec3 ourPosition;

void main()
{
    // FragColor = ourColor;
    FragColor = vec4(ourColor,1.0);
    // FragColor = vec4(ourPosition,1.0);
}
