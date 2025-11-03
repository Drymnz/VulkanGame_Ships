// IMPORTANTE: los defines de implementación van *solo* en este archivo .cpp
// Esto indica a tinygltf que genere las implementaciones de sus funciones.
// Si los defines estuvieran en un .h, causarían redefiniciones en otros archivos.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

#include "graphics/ModelLoader.h"
#include <iostream>

// Constructor: inicializa bandera de carga
ModelLoader::ModelLoader() : loaded(false) {}

// Destructor: (vacío, pero se podría limpiar buffers si fuera necesario)
ModelLoader::~ModelLoader() {}

bool ModelLoader::loadModel(const std::string& filepath) {
    // Objetos base del cargador tinygltf
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    
    // Cargar modelo glTF binario (.glb)
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filepath);
    
    // Mostrar advertencias si las hay
    if (!warn.empty()) {
        std::cout << "Warning al cargar modelo: " << warn << std::endl;
    }
    
    // Mostrar errores y detener si algo falló
    if (!err.empty()) {
        std::cerr << "Error al cargar modelo: " << err << std::endl;
        return false;
    }
    
    // Si falló la carga
    if (!ret) {
        std::cerr << "Failed to parse glTF" << std::endl;
        return false;
    }
    
    // Información básica del modelo
    std::cout << "Modelo cargado: " << filepath << std::endl;
    std::cout << "Meshes: " << model.meshes.size() << std::endl;
    
    // Procesar cada mesh dentro del archivo glTF
    for (size_t i = 0; i < model.meshes.size(); i++) {
        const tinygltf::Mesh& mesh = model.meshes[i];
        
        // Cada mesh puede tener múltiples primitivas (partes separadas)
        for (size_t j = 0; j < mesh.primitives.size(); j++) {
            const tinygltf::Primitive& primitive = mesh.primitives[j];
            
            // === POSICIONES ===
            // Se obtiene el accessor (índice a los datos) de las posiciones
            const tinygltf::Accessor& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView& posView = model.bufferViews[posAccessor.bufferView];
            const tinygltf::Buffer& posBuffer = model.buffers[posView.buffer];
            // Apuntamos al arreglo de floats con las posiciones
            const float* positions = reinterpret_cast<const float*>(&posBuffer.data[posView.byteOffset + posAccessor.byteOffset]);
            
            // === NORMALES (opcional) ===
            const float* normals = nullptr;
            if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                const tinygltf::Accessor& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                const tinygltf::BufferView& normView = model.bufferViews[normAccessor.bufferView];
                const tinygltf::Buffer& normBuffer = model.buffers[normView.buffer];
                normals = reinterpret_cast<const float*>(&normBuffer.data[normView.byteOffset + normAccessor.byteOffset]);
            }
            
            // === UVs (opcional) ===
            const float* texCoords = nullptr;
            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                const tinygltf::Accessor& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                const tinygltf::BufferView& uvView = model.bufferViews[uvAccessor.bufferView];
                const tinygltf::Buffer& uvBuffer = model.buffers[uvView.buffer];
                texCoords = reinterpret_cast<const float*>(&uvBuffer.data[uvView.byteOffset + uvAccessor.byteOffset]);
            }
            
            // === CREACIÓN DE VÉRTICES ===
            size_t vertexStart = vertices.size();
            for (size_t v = 0; v < posAccessor.count; v++) {
                Vertex3D vertex{};
                
                // Posición
                vertex.pos[0] = positions[v * 3 + 0];
                vertex.pos[1] = positions[v * 3 + 1];
                vertex.pos[2] = positions[v * 3 + 2];
                
                // Normal (si no existe, asigna una hacia arriba)
                if (normals) {
                    vertex.normal[0] = normals[v * 3 + 0];
                    vertex.normal[1] = normals[v * 3 + 1];
                    vertex.normal[2] = normals[v * 3 + 2];
                } else {
                    vertex.normal[0] = 0.0f;
                    vertex.normal[1] = 1.0f;
                    vertex.normal[2] = 0.0f;
                }
                
                // Coordenadas de textura (si no existen, poner 0)
                if (texCoords) {
                    vertex.texCoord[0] = texCoords[v * 2 + 0];
                    vertex.texCoord[1] = texCoords[v * 2 + 1];
                } else {
                    vertex.texCoord[0] = 0.0f;
                    vertex.texCoord[1] = 0.0f;
                }
                
                // Color por defecto (blanco)
                vertex.color[0] = 1.0f;
                vertex.color[1] = 1.0f;
                vertex.color[2] = 1.0f;
                
                vertices.push_back(vertex);
            }
            
            // === ÍNDICES ===
            if (primitive.indices >= 0) {
                const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
                const tinygltf::BufferView& indexView = model.bufferViews[indexAccessor.bufferView];
                const tinygltf::Buffer& indexBuffer = model.buffers[indexView.buffer];
                
                const void* dataPtr = &indexBuffer.data[indexView.byteOffset + indexAccessor.byteOffset];
                
                // Se leen los índices según el tipo de dato (byte, short, int)
                for (size_t idx = 0; idx < indexAccessor.count; idx++) {
                    uint32_t index = 0;
                    
                    switch (indexAccessor.componentType) {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            const uint16_t* buf = static_cast<const uint16_t*>(dataPtr);
                            index = buf[idx];
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                            const uint32_t* buf = static_cast<const uint32_t*>(dataPtr);
                            index = buf[idx];
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                            const uint8_t* buf = static_cast<const uint8_t*>(dataPtr);
                            index = buf[idx];
                            break;
                        }
                    }
                    
                    // Los índices se ajustan al desplazamiento actual de vértices
                    indices.push_back(vertexStart + index);
                }
            }
        }
    }
    
    // Información final
    std::cout << "Vértices cargados: " << vertices.size() << std::endl;
    std::cout << "Índices cargados: " << indices.size() << std::endl;
    
    loaded = true;
    return true;
}
