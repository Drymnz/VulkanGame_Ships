#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GLFW/glfw3.h>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    void create(int width, int height, const char* title);
    void setUserPointer(void* pointer);
    void setKeyCallback(GLFWkeyfun callback);
    GLFWwindow* getWindow() const;
    void destroy();

private:
    GLFWwindow* window;
};

#endif // WINDOW_MANAGER_H