#include "game/GameObject.h"

GameObject::GameObject() {}

void GameObject::update(float deltaTime) {
    // Lógica específica del objeto
}

Transform& GameObject::getTransform() {
    return transform;
}

const Transform& GameObject::getTransform() const {
    return transform;
}