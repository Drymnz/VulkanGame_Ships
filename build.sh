#!/bin/bash

echo "=== Compilando proyecto Vulkan Game ==="

# Compilar shaders
echo ""
echo "[1/3] Compilando shaders..."
cd shaders
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv

if [ $? -ne 0 ]; then
    echo "✗ Error al compilar shaders"
    exit 1
fi
echo "✓ Shaders compilados"

# Copiar shaders a build
if [ ! -d "../build" ]; then
    mkdir ../build
fi
cp vert.spv ../build/
cp frag.spv ../build/
echo "✓ Shaders copiados a build/"

# Compilar proyecto
echo ""
echo "[2/3] Compilando código C++..."
cd ../build
cmake .. > /dev/null 2>&1
make

if [ $? -ne 0 ]; then
    echo "✗ Error al compilar código"
    exit 1
fi
echo "✓ Código compilado"

# Verificar que el modelo existe
echo ""
echo "[3/3] Verificando recursos..."
if [ -f "ship.glb" ]; then
    echo "✓ Modelo ship.glb encontrado"
else
    echo "⚠ Advertencia: ship.glb no encontrado en build/"
fi

echo ""
echo "=== ✓ Compilación completa ==="
echo ""
echo "Para ejecutar: cd build && ./juego"
