#include "input/InputManager.h"

// -------------------------------------------------------
// Constructor: inicializa todos los estados de las teclas y física básica
// -------------------------------------------------------
InputManager::InputManager()
    : keyW(false)        // Estado de la tecla W (avanzar)
    , keyA(false)        // Estado de la tecla A (izquierda)
    , keyS(false)        // Estado de la tecla S (retroceder)
    , keyD(false)        // Estado de la tecla D (derecha)
    , keySpace(false)    // Estado de la tecla ESPACIO (saltar)
    , isJumping(false)   // Indica si actualmente está en el aire
    , velocityDepth(0.0f) // Velocidad en el eje "profundidad" (Z)
    , jumpStrength(-0.5f) // Fuerza inicial del salto (negativa = alejarse de cámara)
    , gravity(0.02f) {}   // Valor de la gravedad (positiva = atrae hacia cámara)

// Destructor vacío — no hay recursos dinámicos
InputManager::~InputManager() {}

// -------------------------------------------------------
// Procesa eventos de teclado de GLFW
// -------------------------------------------------------
void InputManager::handleKeyPress(int key, int action) {
    // Considerar presionada tanto cuando se presiona como cuando se mantiene
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    
    // Movimiento direccional
    if (key == GLFW_KEY_W) keyW = isPressed;
    if (key == GLFW_KEY_S) keyS = isPressed;
    if (key == GLFW_KEY_A) keyA = isPressed;
    if (key == GLFW_KEY_D) keyD = isPressed;
    
    // Lógica de salto: solo activa si se presiona espacio y no está saltando
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && !isJumping) {
        keySpace = true;
        isJumping = true;
        velocityDepth = jumpStrength; // Aplicar impulso inicial (salto)
    }
}

// -------------------------------------------------------
// Actualiza las coordenadas del movimiento del jugador
// -------------------------------------------------------
void InputManager::updateMovement(float& vertical, float& depth, float& horizontal, float speed) {
    // Movimiento horizontal (izquierda/derecha)
    if (keyA) horizontal -= speed;
    if (keyD) horizontal += speed;
    
    // Movimiento vertical (arriba/abajo)
    if (keyW) vertical -= speed;
    if (keyS) vertical += speed;
    
    // Movimiento en profundidad (salto / eje Z)
    if (isJumping) {
        // Aplicar la velocidad al eje de profundidad
        depth += velocityDepth;

        // Actualizar la velocidad con la gravedad
        velocityDepth += gravity;
        
        // Comprobar si el jugador aterriza (profundidad mínima alcanzada)
        if (depth >= -2.0f) {
            depth = -2.0f;         // Restablecer posición base
            velocityDepth = 0.0f;  // Detener movimiento
            isJumping = false;     // Marcar como en tierra
            keySpace = false;      // Restablecer estado de la tecla
        }
    }
}

// -------------------------------------------------------
// Retorna si una tecla específica está presionada
// -------------------------------------------------------
bool InputManager::isKeyPressed(int key) const {
    if (key == GLFW_KEY_W) return keyW;
    if (key == GLFW_KEY_S) return keyS;
    if (key == GLFW_KEY_A) return keyA;
    if (key == GLFW_KEY_D) return keyD;
    if (key == GLFW_KEY_SPACE) return keySpace;
    return false;
}
