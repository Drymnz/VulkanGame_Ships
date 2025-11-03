#version 450

// Inputs del vertex shader
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragWorldPos;

// Uniform buffer para la iluminación
layout(binding = 0) uniform LightUBO {
    vec3 lightDirection;
    float lightIntensity;
    vec3 lightColor;
    float ambientStrength;
} light;

layout(location = 0) out vec4 outColor;

void main() {
    // Normalizar la normal
    vec3 normal = normalize(fragNormal);
    
    // Normalizar la dirección de la luz
    vec3 lightDir = normalize(-light.lightDirection);
    
    // ============================================
    // 1. LUZ AMBIENTE (más alta para escenas claras)
    // ============================================
    vec3 ambient = light.ambientStrength * light.lightColor;
    
    // ============================================
    // 2. LUZ DIFUSA
    // ============================================
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.lightIntensity * light.lightColor;
    
    // ============================================
    // 3. LUZ ESPECULAR
    // ============================================
    vec3 viewPos = vec3(0.0, 0.0, 5.0);
    vec3 viewDir = normalize(viewPos - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = spec * 0.8 * light.lightColor;
    
    // ============================================
    // COMBINACIÓN FINAL - MÁS BRILLANTE
    // ============================================
    vec3 lighting = ambient + diffuse + specular;
    
    // 🔥 BOOST: Aclarar el color base del modelo
    vec3 brightenedColor = fragColor * 1.5; // Multiplicar por 1.5 para aclarar
    
    vec3 finalColor = lighting * brightenedColor;
    
    // 🔥 BOOST FINAL: Asegurar que no sea demasiado oscuro
    finalColor = max(finalColor, vec3(0.2)); // Mínimo 20% de brillo
    
    outColor = vec4(finalColor, 1.0);
}