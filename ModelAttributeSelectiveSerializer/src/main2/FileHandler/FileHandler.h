#pragma once

#include "../Types.h"

#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/scene.h>

namespace mass
{

    class FileHandler
    {
    public:
        static void load(const std::string& filename, Configuration& config, model::Model* model);

    private:
        static void processScene(const aiScene* scene, model::Scene* modelScene);
        static void processNode(const aiNode* scene, model::Node* modelNode);
        static void processMesh(const aiMesh* mesh, model::Mesh* modelMesh);

        static std::vector<model::Vertex> getVertices(const aiMesh* mesh);
        static void processBones(const aiMesh* mesh, std::vector<model::Vertex>& vertices);

    private:
        inline static model::Scene* sActiveModelScene = nullptr;
        inline static aiAnimation** sSceneAnimations = nullptr;
        inline static aiCamera** sSceneCameras = nullptr;
        inline static aiLight** sSceneLights = nullptr;
        inline static aiMaterial** sSceneMaterials = nullptr;
        inline static aiMesh** sSceneMeshes = nullptr;
        inline static aiSkeleton** sSceneSkeletons = nullptr;
        inline static aiTexture** sSceneTextures = nullptr;
    };

}
