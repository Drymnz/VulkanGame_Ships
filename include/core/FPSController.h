#ifndef FPS_CONTROLLER_H
#define FPS_CONTROLLER_H

class FPSController
{
public:
    FPSController();
    FPSController(int targetFps);

    void setTargetFPS(int fps);
    int getCurrentFPS() const { return static_cast<int>(currentFPS); }
    
    void begin();
    void end();
    void limit(int fps);
    
    bool shouldRender();
    void update();
    void updateFPSCounter();

private:
    double lastTime;
    double frameStartTime;
    int frameCount;
    double currentFPS;
    int targetFPS;
    double frameTime;
    double accumulator;
};

#endif // FPS_CONTROLLER_H