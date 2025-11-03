#include "input/InputManager.h"

InputManager::InputManager()
    : keyW(false)
    , keyA(false)
    , keyS(false)
    , keyD(false)
    , keySpace(false)
    , isJumping(false)
    , velocityDepth(0.0f)
    , jumpStrength(-0.5f)  // Negativo = alejarse (saltar hacia la cámara)
    , gravity(0.02f) {}    // Positivo = volver

InputManager::~InputManager() {}

void InputManager::handleKeyPress(int key, int action) {
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    
    if (key == GLFW_KEY_W) keyW = isPressed;
    if (key == GLFW_KEY_S) keyS = isPressed;
    if (key == GLFW_KEY_A) keyA = isPressed;
    if (key == GLFW_KEY_D) keyD = isPressed;
    
    // Salto - solo al presionar
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS && !isJumping) {
        keySpace = true;
        isJumping = true;
        velocityDepth = jumpStrength;
    }
}

void InputManager::updateMovement(float& vertical, float& depth, float& horizontal, float speed) {
    // A/D = Movimiento horizontal (izquierda/derecha)
    if (keyA) horizontal -= speed;
    if (keyD) horizontal += speed;
    
    // W/S = Movimiento vertical (arriba/abajo)
    if (keyW) vertical -= speed;
    if (keyS) vertical += speed;
    
    // ESPACIO = Salto en profundidad (acercarse/alejarse de la cámara)
    if (isJumping) {
        depth += velocityDepth;
        velocityDepth += gravity;
        
        // Aterrizaje - vuelve a la profundidad inicial
        if (depth >= -2.0f) {
            depth = -2.0f;
            velocityDepth = 0.0f;
            isJumping = false;
            keySpace = false;
        }
    }
}

bool InputManager::isKeyPressed(int key) const {
    if (key == GLFW_KEY_W) return keyW;
    if (key == GLFW_KEY_S) return keyS;
    if (key == GLFW_KEY_A) return keyA;
    if (key == GLFW_KEY_D) return keyD;
    if (key == GLFW_KEY_SPACE) return keySpace;
    return false;
}