#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

// Push constants para la matriz del modelo
layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

// Uniform buffer para la iluminación
layout(binding = 0) uniform LightUBO {
    vec3 lightDirection;   // Dirección de la luz (sol)
    float lightIntensity;  // Intensidad de la luz
    vec3 lightColor;       // Color de la luz
    float ambientStrength; // Luz ambiente
} light;

// Outputs al fragment shader
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragWorldPos;

void main() {
    // Transformar posición a espacio mundial
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    
    // Proyección perspectiva
    float fov = 1.0;
    float aspect = 800.0 / 600.0;
    float near = 0.1;
    float far = 100.0;
    
    float f = 1.0 / tan(fov / 2.0);
    mat4 projection = mat4(
        f/aspect, 0.0, 0.0, 0.0,
        0.0, f, 0.0, 0.0,
        0.0, 0.0, (far+near)/(near-far), -1.0,
        0.0, 0.0, (2.0*far*near)/(near-far), 0.0
    );
    
    // Vista (cámara en Z=5 mirando hacia -Z)
    mat4 view = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, -5.0, 1.0
    );
    
    gl_Position = projection * view * worldPos;
    
    // Transformar la normal al espacio mundial
    fragNormal = mat3(push.model) * inNormal;
    fragWorldPos = worldPos.xyz;
    fragColor = inColor;
}