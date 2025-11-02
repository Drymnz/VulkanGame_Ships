#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>

class InputManager {
public:
    InputManager();
    ~InputManager();

    void handleKeyPress(int key, int action);
    void updateMovement(float& x, float& y, float speed);

    bool isKeyPressed(int key) const;

private:
    bool keyW;
    bool keyA;
    bool keyS;
    bool keyD;
};

#endif