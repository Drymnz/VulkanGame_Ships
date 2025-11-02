#include "input/InputManager.h"

InputManager::InputManager()
    : keyW(false)
    , keyA(false)
    , keyS(false)
    , keyD(false) {}

InputManager::~InputManager() {}

void InputManager::handleKeyPress(int key, int action) {
    bool isPressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    
    if (key == GLFW_KEY_W) keyW = isPressed;
    if (key == GLFW_KEY_S) keyS = isPressed;
    if (key == GLFW_KEY_A) keyA = isPressed;
    if (key == GLFW_KEY_D) keyD = isPressed;
}

void InputManager::updateMovement(float& x, float& y, float speed) {
    if (keyW) y -= speed;
    if (keyS) y += speed;
    if (keyA) x -= speed;
    if (keyD) x += speed;
}

bool InputManager::isKeyPressed(int key) const {
    if (key == GLFW_KEY_W) return keyW;
    if (key == GLFW_KEY_S) return keyS;
    if (key == GLFW_KEY_A) return keyA;
    if (key == GLFW_KEY_D) return keyD;
    return false;
}