#pragma once
#include <string>

#include "ResourceSystem/Vertex.h"

struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices; // uint32�� ����

    bool IsEmpty() const {
        return vertices.empty() || indices.empty();
    }
};