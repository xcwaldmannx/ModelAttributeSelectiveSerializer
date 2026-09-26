#pragma once

#include "../Types.h"

#include <string>
#include <vector>

#include <assimp/scene.h>

namespace mal
{

    class ModelHandler
    {
    public:
        static model::Model load(const std::string& filename, Configuration& config);

    private:
        static void categorize(const aiScene* scene);
        static void processScene(const aiScene* scene, model::Scene* modelScene);
        static void processNode(const aiNode* scene, model::Node* modelNode);
        static void processMesh(const aiMesh* mesh, model::Mesh* modelMesh);

        static std::pair<model::VertexArray, model::IndexArray> getVerticesAndIndices(const aiMesh* mesh);
        static void processBones(const aiMesh* mesh, model::VertexArray& vertices);

    private:
        inline static model::Scene* sActiveModelScene = nullptr;
        inline static model::Model* sActiveModel = nullptr;

        inline static size_t sVertexOffset = 0;
        inline static size_t sIndexOffset = 0;

        inline static aiAnimation** sSceneAnimations = nullptr;
        inline static aiCamera** sSceneCameras = nullptr;
        inline static aiLight** sSceneLights = nullptr;
        inline static aiMaterial** sSceneMaterials = nullptr;
        inline static aiMesh** sSceneMeshes = nullptr;
        inline static aiSkeleton** sSceneSkeletons = nullptr;
        inline static aiTexture** sSceneTextures = nullptr;
    };

}
