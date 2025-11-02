#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    // Sin matrices por ahora - solo escalar y centrar
    vec3 pos = inPosition * 0.5; // Escalar
    gl_Position = vec4(pos, 1.0);
    
    // Iluminación simple directa
    vec3 lightDir = normalize(vec3(0.5, -1.0, 0.3));
    float diffuse = max(dot(normalize(inNormal), -lightDir), 0.3);
    
    fragColor = inColor * diffuse;
}