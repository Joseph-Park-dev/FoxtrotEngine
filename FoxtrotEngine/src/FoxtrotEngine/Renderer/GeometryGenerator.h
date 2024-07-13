#pragma once
#include "FoxtrotEngine/ResourceSystem/MeshData.h"

#include "spine/spine.h"

class GeometryGenerator {
  public:
    //static std::vector<MeshData> ReadFromFile(std::string basePath,
    //                                     std::string filename);
    static MeshData MakeSpineAnimation(spine::RenderCommand* command);
    static MeshData MakeSquare(float scale);
    static MeshData MakeBox();
    //static MeshData MakeCylinder(const float bottomRadius,
    //                             const float topRadius, float height,
    //                             int numSlices);
    //static MeshData MakeSphere(const float radius, const int numSlices,
    //                           const int numStacks);
    //static MeshData MakeTetrahedron();
    //static MeshData MakeIcosahedron();
    //static MeshData SubdivideToSphere(const float radius, MeshData meshData);
};