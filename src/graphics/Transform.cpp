#include "graphics/Transform.h"
#include <cstring>

Transform::Transform()
    : position{0.0f, -2.0f, 0.0f}
    , rotation{0.0f, 0.0f, 0.0f}
    , scale{1.0f, 1.0f, 1.0f} {}

void Transform::setPosition(float x, float y, float z) {
    position[0] = x;
    position[1] = y;
    position[2] = z;
}

void Transform::setRotation(float x, float y, float z) {
    rotation[0] = x;
    rotation[1] = y;
    rotation[2] = z;
}

void Transform::setScale(float x, float y, float z) {
    scale[0] = x;
    scale[1] = y;
    scale[2] = z;
}

void Transform::getModelMatrix(float* matrix) const {
    // Matriz identidad
    memset(matrix, 0, 16 * sizeof(float));
    
    // Escala
    matrix[0] = scale[0];
    matrix[5] = scale[1];
    matrix[10] = scale[2];
    matrix[15] = 1.0f;
    
    // Posición
    matrix[12] = position[0];
    matrix[13] = position[1];
    matrix[14] = position[2];
    
    // TODO: Agregar rotación cuando sea necesario
}