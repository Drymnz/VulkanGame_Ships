#ifndef LIGHT_DATA_H
#define LIGHT_DATA_H

// Estructura que debe coincidir EXACTAMENTE con el uniform en el shader
struct alignas(16) LightUBO {
    alignas(16) float lightDirection[3];  // vec3: 12 bytes + 4 padding = 16
    alignas(4)  float lightIntensity;     // float: 4 bytes
    alignas(16) float lightColor[3];      // vec3: 12 bytes + 4 padding = 16
    alignas(4)  float ambientStrength;    // float: 4 bytes
    
    // Constructor con valores por defecto (sol)
    LightUBO() {
        // Dirección del sol (desde arriba-derecha-adelante)
        lightDirection[0] = 0.5f;   // X
        lightDirection[1] = -1.0f;  // Y (hacia abajo)
        lightDirection[2] = 0.3f;   // Z
        
        lightIntensity = 1.0f;      // Intensidad al 100%
        
        // Color amarillo-blanco del sol
        lightColor[0] = 1.0f;       // R
        lightColor[1] = 0.95f;      // G
        lightColor[2] = 0.8f;       // B
        
        ambientStrength = 0.3f;     // 30% de luz ambiente
    }
};

#endif // LIGHT_DATA_H