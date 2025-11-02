#pragma once
#include <string>
#include <vector>
#include "BufferManager.h"
#include "Vertex.h"  

class ModelLoader {
public:
    ModelLoader();
    ~ModelLoader();

    bool loadModel(const std::string& filepath);
    
    const std::vector<Vertex3D>& getVertices() const { return vertices; }
    const std::vector<uint32_t>& getIndices() const { return indices; }
    bool isLoaded() const { return loaded; }

private:
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;
    bool loaded;
};