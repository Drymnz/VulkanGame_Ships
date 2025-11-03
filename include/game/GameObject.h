#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "graphics/Transform.h"

class GameObject {
public:
    GameObject();

    void update(float deltaTime);

    Transform& getTransform();
    const Transform& getTransform() const;

private:
    Transform transform;
};

#endif // GAME_OBJECT_H