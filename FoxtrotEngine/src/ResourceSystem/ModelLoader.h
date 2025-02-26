#pragma once

#include "ModelLoader.h"

// vcpkg install assimp:x64-windows
// Preprocessor definitions에 NOMINMAX 추가
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <iostream>
#include <string>
#include <vector>

#include "FTMeshData.h"
#include "Vertex.h"

class ModelLoader {
  public:
    void Load(std::string basePath, std::string filename);

    void ProcessNode(aiNode *node, const aiScene *scene,
                     DirectX::SimpleMath::Matrix tr);

    FTMeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);

  public:
    std::string basePath;
    std::vector<FTMeshData> meshes;
};