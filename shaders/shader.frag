#version 450

// Inputs del vertex shader
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

// Uniform buffer para la iluminación
layout(binding = 0) uniform LightUBO {
    vec3 lightDirection;   // Dirección de la luz (sol)
    float lightIntensity;  // Intensidad de la luz
    vec3 lightColor;       // Color de la luz
    float ambientStrength; // Luz ambiente
} light;

layout(location = 0) out vec4 outColor;

void main() {
    // Normalizar la normal
    vec3 normal = normalize(fragNormal);
    
    // Normalizar la dirección de la luz (invertida porque apunta HACIA la luz)
    vec3 lightDir = normalize(-light.lightDirection);
    
    // ============================================
    // 1. LUZ AMBIENTE (iluminación base mínima)
    // ============================================
    vec3 ambient = light.ambientStrength * light.lightColor;
    
    // ============================================
    // 2. LUZ DIFUSA (Lambert - depende del ángulo)
    // ============================================
    // Calcular cuánta luz recibe la superficie basado en el ángulo
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.lightIntensity * light.lightColor;
    
    // ============================================
    // 3. LUZ ESPECULAR (Phong - brillos)
    // ============================================
    // Posición de la cámara (en Z=5 según el vertex shader)
    vec3 viewPos = vec3(0.0, 0.0, 5.0);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    
    // Reflexión de la luz
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // Calcular el brillo especular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // 32 = shininess
    vec3 specular = spec * 0.5 * light.lightColor; // 0.5 = fuerza especular
    
    // ============================================
    // COMBINACIÓN FINAL
    // ============================================
    vec3 lighting = ambient + diffuse + specular;
    vec3 finalColor = lighting * fragColor;
    
    outColor = vec4(finalColor, 1.0);
}