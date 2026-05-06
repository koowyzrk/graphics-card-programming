#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // normalna musi zostac prostopadla do powierzchni po transformacji
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // pozycja wierzcholka w przestrzeni swiata
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}  


