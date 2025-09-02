#pragma once

#include "ModelLoader.h"

// vcpkg install assimp:x64-windows
// Preprocessor definitions에 NOMINMAX 추가
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <iostream>
#include <vector>

#include "FTMeshData.h"
#include "Vertex.h"

#include "Static/FTString.h"

class ModelLoader {
  public:
      void Load(const FTDS::String& resPath);
      void Load(const FTDS::String& basePath, const FTDS::String& filename);

    void ProcessNode(aiNode *node, const aiScene *scene,
                     DirectX::SimpleMath::Matrix tr);

    FTMeshData ProcessMesh(aiMesh *mesh, const aiScene *scene);

  public:
    FTDS::String basePath;
    std::vector<FTMeshData> meshes;
};