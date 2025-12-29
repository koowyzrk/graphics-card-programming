#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in mat4 instanceMatrix;

out vec2 TexCoord;

uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;
uniform bool isInstanced;

void main()
{
    TexCoord = aTexCoords;
    mat4 finalModel = isInstanced ? instanceMatrix : model;
    gl_Position = projection * view * finalModel * vec4(aPos, 1.0);
    // gl_Position = projection * view * model * vec4(aPos, 1.0);
}
