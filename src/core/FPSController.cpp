#include "core/FPSController.h"
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

// Constructor por defecto: inicializa el controlador a 60 FPS por defecto
FPSController::FPSController()
    : lastTime(0.0)
    , frameStartTime(0.0)
    , frameCount(0)
    , currentFPS(0.0)
    , targetFPS(60)
    , frameTime(1.0 / 60.0)  // Duración de cada frame (1/60 segundos)
    , accumulator(0.0)
{
    // Registra el tiempo inicial (segundos desde inicio del programa)
    lastTime = glfwGetTime();
}

// Constructor que permite definir manualmente los FPS objetivo
FPSController::FPSController(int targetFps) 
    : lastTime(0.0)
    , frameStartTime(0.0)
    , frameCount(0)
    , currentFPS(0.0)
    , targetFPS(targetFps)
    , frameTime(1.0 / targetFps)
    , accumulator(0.0)
{
    lastTime = glfwGetTime();
}

// Cambia los FPS objetivo dinámicamente
void FPSController::setTargetFPS(int fps) {
    if (fps <= 0) fps = 60;  // Valor mínimo de seguridad
    targetFPS = fps;
    frameTime = 1.0 / targetFPS;  // Actualiza la duración del frame
}

// Marca el inicio de un frame (para calcular cuánto tarda)
void FPSController::begin() {
    frameStartTime = glfwGetTime();
}

// Marca el final del frame y actualiza el contador de FPS
void FPSController::end() {
    frameCount++;
    updateFPSCounter();
}

// Limita la velocidad de fotogramas (sleep si el frame fue muy rápido)
void FPSController::limit(int fps) {
    double currentTime = glfwGetTime();
    double elapsed = currentTime - frameStartTime;  // Tiempo que tomó el frame
    double targetFrameTime = 1.0 / fps;             // Tiempo deseado por frame
    
    if (elapsed < targetFrameTime) {
        double sleepTime = targetFrameTime - elapsed;
        // Suspende el hilo por el tiempo necesario para mantener los FPS constantes
        std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
    }
}

// Determina si es momento de renderizar un nuevo frame
// (usado cuando se quiere interpolar física/render en tiempos fijos)
bool FPSController::shouldRender() {
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;  // Tiempo desde el último frame
    
    accumulator += deltaTime;  // Acumula tiempo real transcurrido
    lastTime = currentTime;
    
    // Si ya pasó el tiempo de un frame, se debe renderizar
    if (accumulator >= frameTime) {
        accumulator -= frameTime;
        return true;
    }
    
    return false;
}

// Incrementa contador de frames sin recalcular FPS
void FPSController::update() {
    frameCount++;
}

// Calcula los FPS actuales cada segundo
void FPSController::updateFPSCounter() {
    double currentTime = glfwGetTime();
    
    // Cada segundo, calcula FPS promedio
    if (currentTime - lastTime >= 1.0) {
        currentFPS = frameCount / (currentTime - lastTime);
        frameCount = 0;
        lastTime = currentTime;
    }
}
