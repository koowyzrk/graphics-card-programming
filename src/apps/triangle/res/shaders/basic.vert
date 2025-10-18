#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//additional
// out vec3 ourPosition;
// uniform float xOffset;
uniform mat4 transform;

void main()
{
    gl_Position = transform * projection * view * model * vec4(aPos.x,aPos.y,aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;

    // additional
    // ourPosition = aPos;
    // gl_Position = vec4(aPos.x,aPos.y,aPos.z, 1.0);
    // gl_Position = transform * vec4(aPos.x,aPos.y,aPos.z, 1.0);
}
