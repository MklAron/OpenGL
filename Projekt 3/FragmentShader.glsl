#version 330 core
in vec3 FragPosition;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform bool lightingEnabled;
uniform bool isLightSource;

void main()
{
    if(isLightSource) {
        FragColor = vec4(lightColor, 1.0);
    }
    else if (lightingEnabled) {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
        
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPosition - FragPosition);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        vec3 result = (ambient + diffuse) * vec3(1.0);
        FragColor = vec4(result, 1.0);
    } else {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}