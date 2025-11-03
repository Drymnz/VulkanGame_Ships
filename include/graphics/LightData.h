#ifndef LIGHT_DATA_H
#define LIGHT_DATA_H

#include <cstring>

// Estructura que debe coincidir EXACTAMENTE con el uniform en el shader
// Layout estándar std140 de GLSL
struct LightUBO {
    // vec3 + padding = 16 bytes
    float lightDirection[3];  // 12 bytes
    float _pad1;              // 4 bytes padding
    
    float lightIntensity;     // 4 bytes
    float _pad2[3];           // 12 bytes padding para alinear a 16
    
    // vec3 + padding = 16 bytes
    float lightColor[3];      // 12 bytes
    float _pad3;              // 4 bytes padding
    
    float ambientStrength;    // 4 bytes
    float _pad4[3];           // 12 bytes padding para alinear a 16
    
    // Total: 64 bytes
    
    // Constructor con valores por defecto (sol)
    LightUBO() {
        // Dirección del sol (desde arriba-derecha-adelante)
        lightDirection[0] = 0.5f;   // X
        lightDirection[1] = -1.0f;  // Y (hacia abajo)
        lightDirection[2] = 0.3f;   // Z
        _pad1 = 0.0f;
        
        lightIntensity = 2.0f;      // Intensidad muy alta
        _pad2[0] = _pad2[1] = _pad2[2] = 0.0f;
        
        // Color blanco brillante
        lightColor[0] = 1.0f;       // R
        lightColor[1] = 1.0f;       // G
        lightColor[2] = 1.0f;       // B
        _pad3 = 0.0f;
        
        ambientStrength = 0.8f;     // 80% de luz ambiente (muy brillante)
        _pad4[0] = _pad4[1] = _pad4[2] = 0.0f;
    }
};

// Verificar que el tamaño sea correcto (64 bytes)
static_assert(sizeof(LightUBO) == 64, "LightUBO debe ser exactamente 64 bytes");

#endif // LIGHT_DATA_H