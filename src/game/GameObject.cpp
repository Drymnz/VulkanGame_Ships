#include "game/GameObject.h"

// Constructor por defecto
// Inicializa el objeto sin ninguna lógica especial.
GameObject::GameObject() {}

// Método update: se llama cada frame.
// Recibe el tiempo transcurrido (deltaTime) para actualizar la lógica del objeto.
// Aquí se podría implementar movimiento, animaciones, IA, etc.
void GameObject::update(float deltaTime) {
    // Lógica específica del objeto (vacía por ahora)
}

// Devuelve una referencia al transform del objeto (no constante).
// Permite modificar la posición, rotación y escala.
Transform& GameObject::getTransform() {
    return transform;
}

// Devuelve una referencia constante al transform.
// Se usa cuando solo se necesita leer la información sin modificarla.
const Transform& GameObject::getTransform() const {
    return transform;
}
