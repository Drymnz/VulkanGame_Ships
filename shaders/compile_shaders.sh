#!/bin/bash

echo "Compilando shaders..."

# Compilar shaders
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv

# Verificar que la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "✓ Shaders compilados exitosamente"
    
    # Copiar a la carpeta build
    if [ -d "../build" ]; then
        cp vert.spv ../build/
        cp frag.spv ../build/
        echo "✓ Shaders copiados a build/"
    else
        echo "⚠ Carpeta build no encontrada"
    fi
else
    echo "✗ Error al compilar shaders"
    exit 1
fi

echo "Listo!"
