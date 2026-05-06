#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    // pozycja wierzchołka(wektor kierunkowy) staje się wspolrzedna tekstury
    TexCoords = aPos;
    mat4 staticView = mat4(mat3(view)); //usuwanie translacji - tylko rotacja
    vec4 pos = projection * staticView * vec4(aPos, 1.0);
    //.xyww ustawia z = w, co po perspective divide daje z/w = 1.0 - maks głębokość
    gl_Position = pos.xyww; 
}
