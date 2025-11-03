#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inColor;

layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;

void main() {
    // Transformar posición
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    
    // Proyección perspectiva simple
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
    
    // Vista rotada: Cámara mirando desde +X hacia -X
    // Y = derecha/izquierda, Z = arriba/abajo, X = profundidad
    mat4 view = mat4(
        0.0, 0.0, -1.0, 0.0,   // X de cámara apunta a -Z de mundo
        -1.0, 0.0, 0.0, 0.0,   // Y de cámara apunta a -X de mundo
        0.0, 1.0, 0.0, 0.0,    // Z de cámara apunta a +Y de mundo
        5.0, 0.0, 0.0, 1.0     // Cámara en X=5 mirando hacia X=0
    );
    
    gl_Position = projection * view * worldPos;
    
    fragNormal = mat3(push.model) * inNormal;
    fragColor = inColor;
}