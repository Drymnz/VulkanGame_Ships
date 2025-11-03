// InputManager.h
#pragma once

#include <GLFW/glfw3.h>

class InputManager {
public:
    InputManager();
    ~InputManager();

    // Maneja eventos de teclado (llamado desde un callback o bucle principal)
    void handleKeyPress(int key, int action);

    // Actualiza la posición en el espacio 3D
    // vertical: eje Y (arriba/abajo)
    // depth: eje Z (profundidad, hacia/desde la cámara)
    // horizontal: eje X (izquierda/derecha)
    void updateMovement(float& vertical, float& depth, float& horizontal, float speed);

    // Verifica si una tecla está actualmente presionada
    bool isKeyPressed(int key) const;

private:
    // Estados de las teclas
    bool keyW;
    bool keyA;
    bool keyS;
    bool keyD;
    bool keySpace;

    // Física del "salto" en profundidad
    bool isJumping;
    float velocityDepth;   // Velocidad de movimiento en profundidad (Z)
    float jumpStrength;    // Impulso inicial (negativo = alejarse de la cámara)
    float gravity;         // Aceleración positiva = volver hacia la cámara
};
