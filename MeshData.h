#pragma once
#include "Vertex.h";

struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices; // uint32·Î º¯°æ
    std::string           textureFilename;
};