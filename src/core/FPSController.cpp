#include "core/FPSController.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

FPSController::FPSController()
    : lastTime(0.0)
    , frameStartTime(0.0)
    , frameCount(0)
    , currentFPS(0.0)
    , targetFPS(60)
    , frameTime(1.0 / 60.0)
    , accumulator(0.0) {
    lastTime = glfwGetTime();
}

FPSController::FPSController(int targetFps) 
    : lastTime(0.0)
    , frameStartTime(0.0)
    , frameCount(0)
    , currentFPS(0.0)
    , targetFPS(targetFps)
    , frameTime(1.0 / targetFps)
    , accumulator(0.0) {
    lastTime = glfwGetTime();
}

void FPSController::setTargetFPS(int fps) {
    if (fps <= 0) fps = 60;
    targetFPS = fps;
    frameTime = 1.0 / targetFPS;
}

void FPSController::begin() {
    frameStartTime = glfwGetTime();
}

void FPSController::end() {
    frameCount++;
    updateFPSCounter();
}

void FPSController::limit(int fps) {
    double currentTime = glfwGetTime();
    double elapsed = currentTime - frameStartTime;
    double targetFrameTime = 1.0 / fps;
    
    if (elapsed < targetFrameTime) {
        double sleepTime = targetFrameTime - elapsed;
        std::this_thread::sleep_for(
            std::chrono::duration<double>(sleepTime)
        );
    }
}

bool FPSController::shouldRender() {
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    
    accumulator += deltaTime;
    lastTime = currentTime;
    
    if (accumulator >= frameTime) {
        accumulator -= frameTime;
        return true;
    }
    
    return false;
}

void FPSController::update() {
    frameCount++;
}

void FPSController::updateFPSCounter() {
    double currentTime = glfwGetTime();
    
    if (currentTime - lastTime >= 1.0) {
        currentFPS = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
}