#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    // kierunek patrzenia widza od fragmentu
    vec3 I = normalize(Position - cameraPos);
    // obliczony wektor odbicia
    vec3 R = reflect(I, normalize(Normal));
    // otrzymany wektor R jest następnie używany jako wektor kierunku do indeksowania/próbkowania cube mapy
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
