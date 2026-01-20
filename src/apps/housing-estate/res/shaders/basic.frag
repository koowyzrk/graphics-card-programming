#version 330 core
out vec4 FragColor;

struct Light {
    bool enabled;
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float range;
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 10
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

uniform bool isLightSource;
uniform vec3 lightVisualColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

vec3 calculateLight(Light light, vec3 normal, vec3 viewDir, vec3 texColor) {
    if (!light.enabled) return vec3(0.0);

    vec3 lightDir;
    float attenuation = 1.0;
    float intensity = 1.0;

    if (light.type == 0) {
        // wektor do światła
        lightDir = normalize(-light.direction);
    } else {
        lightDir = normalize(light.position - FragPos);
        
        // obliczanie prostszego dystansu liniowo nie jako kwadrat odległości 
        // uzywamy tlumienia zeby swiatlo zniklo po przekroczeniu okreslonego dystansu
        float distance = length(light.position - FragPos);
        attenuation = clamp(1.0 - distance / light.range, 0.0, 1.0);
        
        if (light.type == 2) {
            // cosinus kąta między kierunkiem latarki a kierunkiem do fragmentu.
            float theta = dot(lightDir, normalize(-light.direction));
            // różnica między cosinusami kątów wewnętrznego i zewnętrznego.
            float epsilon = light.cutOff - light.outerCutOff;
            // wygladzanie na brzegach
            intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.color * diff * texColor;

    // (Blinn-Phong)
    // główna różnica między modelem Phonga a Blinn-Phonga. 
    // zamiast liczyć wektor odbicia 
    // liczymy wektor dokładnie w połowie drogi między kierunkiem do światła a kierunkiem do kamery
    vec3 halfwayDir = normalize(lightDir + viewDir);
    // pow - mnoznik odblasku - wieksza wartosc ostry punkt
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = light.color * spec * 0.5; 

    return (diffuse + specular) * attenuation * intensity;
}

void main() {
    if (isLightSource) {
        FragColor = vec4(lightVisualColor * 2.0, 1.0); 
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 texColor = texture(texture_diffuse1, TexCoords).rgb;
    
    // ambient
    vec3 result = vec3(0.1) * texColor; 

    for(int i = 0; i < lightCount; i++) {
        result += calculateLight(lights[i], norm, viewDir, texColor);
    }

    FragColor = vec4(result, 1.0);
}
