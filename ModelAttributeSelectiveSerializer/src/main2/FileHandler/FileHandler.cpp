#include "FileHandler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace fs = std::filesystem;

using namespace mass;

void FileHandler::load(const std::string& filename, Configuration& config, model::Model* model)
{
    if (!fs::exists(fs::path(filename)))
    {
        throw std::runtime_error("file does not exist");
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality /*aiProcess_FlipUVs*/);

    if (!scene)
    {
        throw std::runtime_error("could not load scene");
    }

    if (scene->HasAnimations()) model->mSceneElements |= ANIMATION;
    if (scene->HasCameras())    model->mSceneElements |= CAMERA;
    if (scene->HasLights())     model->mSceneElements |= LIGHT;
    if (scene->HasMaterials())  model->mSceneElements |= MATERIAL;
    if (scene->HasMeshes())     model->mSceneElements |= MESH;
    if (scene->HasSkeletons())  model->mSceneElements |= SKELETON;
    if (scene->HasTextures())   model->mSceneElements |= TEXTURE;

    sSceneAnimations = scene->mAnimations;
    sSceneCameras    = scene->mCameras;
    sSceneLights     = scene->mLights;
    sSceneMaterials  = scene->mMaterials;
    sSceneMeshes     = scene->mMeshes;
    sSceneSkeletons  = scene->mSkeletons;
    sSceneTextures   = scene->mTextures;

    model::Scene modelScene;
    const auto& sceneName = scene->mName.C_Str();
    model->mScenes.emplace(sceneName, modelScene);

    sActiveModelScene = &model->mScenes.at(sceneName);

    processScene(scene, &modelScene);


    sActiveModelScene = nullptr;
    sSceneTextures    = nullptr;
    sSceneSkeletons   = nullptr;
    sSceneMeshes      = nullptr;
    sSceneMaterials   = nullptr;
    sSceneLights      = nullptr;
    sSceneCameras     = nullptr;
    sSceneAnimations  = nullptr;
}

void FileHandler::processScene(const aiScene* scene, model::Scene* modelScene)
{
    const auto& node = scene->mRootNode;
    model::Node modelNode;

    processNode(node, &modelNode);
    modelScene->mNodes.emplace(node->mName.C_Str(), std::move(modelNode));

}

void FileHandler::processNode(const aiNode* node, model::Node* modelNode)
{
    const auto meshCount = node->mNumMeshes;
    const auto childNodeCount = node->mNumChildren;

    for (unsigned int i = 0; i < meshCount; i++)
    {
        const auto& mesh = sSceneMeshes[node->mMeshes[i]];
        model::Mesh modelMesh;

        processMesh(mesh, &modelMesh);

        modelNode->mMeshes.emplace(mesh->mName.C_Str(), std::move(modelMesh));
    }

    for (unsigned int i = 0; i < childNodeCount; i++)
    {
        const auto& childNode = node->mChildren[i];
        model::Node modelChildNode;

        processNode(childNode, &modelChildNode);

        modelNode->mChildNodes.emplace(childNode->mName.C_Str(), std::move(modelChildNode));
    }
}

void FileHandler::processMesh(const aiMesh* mesh, model::Mesh* modelMesh)
{
    if (!mesh->HasPositions()) return;

    modelMesh->mVertices = getVertices(mesh);

    if (mesh->HasBones())
    {
        processBones(mesh, modelMesh->mVertices);
    }
}

std::vector<model::Vertex> FileHandler::getVertices(const aiMesh* mesh)
{
    const unsigned int vertexCount = mesh->mNumVertices;
    const unsigned int indexCount = mesh->mNumFaces * 3;

    std::vector<model::Vertex> vertices;
    vertices.resize(vertexCount);

    for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
    {
        model::Vertex modelVertex;

        const auto& vertex = mesh->mVertices[vertexIndex];
        modelVertex.mPosition[0] = vertex.x;
        modelVertex.mPosition[1] = vertex.y;
        modelVertex.mPosition[2] = vertex.z;

        if (mesh->HasNormals())
        {
            const auto& normal = mesh->mNormals[vertexIndex];
            modelVertex.mNormal[0] = normal.x;
            modelVertex.mNormal[1] = normal.y;
            modelVertex.mNormal[2] = normal.z;
        }

        if (mesh->HasTangentsAndBitangents())
        {
            const auto& tangent = mesh->mTangents[vertexIndex];
            modelVertex.mTangent[0] = tangent.x;
            modelVertex.mTangent[1] = tangent.y;
            modelVertex.mTangent[2] = tangent.z;

            const auto& bitangent = mesh->mBitangents[vertexIndex];
            modelVertex.mBitangent[0] = bitangent.x;
            modelVertex.mBitangent[1] = bitangent.y;
            modelVertex.mBitangent[2] = bitangent.z;
        }

        const unsigned int texCoordSet = 0;

        if (mesh->HasTextureCoords(texCoordSet))
        {
            const auto& texCoords = mesh->mTextureCoords[texCoordSet][vertexIndex];
            modelVertex.mTexCoord[0] = texCoords.x;
            modelVertex.mTexCoord[1] = texCoords.y;
        }

        const unsigned int colorSet = 0;

        if (mesh->HasVertexColors(colorSet))
        {
            const auto& color = mesh->mColors[colorSet][vertexIndex];
            modelVertex.mColor[0] = color.r;
            modelVertex.mColor[1] = color.g;
            modelVertex.mColor[2] = color.b;
            modelVertex.mColor[3] = color.a;
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++)
        {
            const aiFace& face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                unsigned int index = face.mIndices[j] + vertexOffset;
                modelVertex.mIndices.push_back(index);
            }
        }

        vertices[vertexIndex] = modelVertex;
    }

    return vertices;
}

void FileHandler::processBones(const aiMesh* mesh, std::vector<model::Vertex>& vertices)
{
    const unsigned int boneCount = mesh->mNumBones;

    for (unsigned int boneIndex = 0; boneIndex < boneCount; boneIndex++)
    {
        int boneId = -1;

        const auto& bone = mesh->mBones[boneIndex];
        const auto& boneName = bone->mName.C_Str();

        auto modelBoneIt = sActiveModelScene->mBones.find(boneName);

        if (modelBoneIt == sActiveModelScene->mBones.end())
        {
            model::Bone modelBone;
            modelBone.mBoneId = static_cast<int>(sActiveModelScene->mBones.size());

            const auto& boneTrans = bone->mOffsetMatrix;
            auto& ModelBoneTrans = modelBone.mOffsetMatrix;

            ModelBoneTrans[0][0] = boneTrans.a1;
            ModelBoneTrans[0][1] = boneTrans.a2;
            ModelBoneTrans[0][2] = boneTrans.a3;
            ModelBoneTrans[0][3] = boneTrans.a4;

            ModelBoneTrans[1][0] = boneTrans.b1;
            ModelBoneTrans[1][1] = boneTrans.b2;
            ModelBoneTrans[1][2] = boneTrans.b3;
            ModelBoneTrans[1][3] = boneTrans.b4;

            ModelBoneTrans[2][0] = boneTrans.c1;
            ModelBoneTrans[2][1] = boneTrans.c2;
            ModelBoneTrans[2][2] = boneTrans.c3;
            ModelBoneTrans[2][3] = boneTrans.c4;

            ModelBoneTrans[3][0] = boneTrans.d1;
            ModelBoneTrans[3][1] = boneTrans.d2;
            ModelBoneTrans[3][2] = boneTrans.d3;
            ModelBoneTrans[3][3] = boneTrans.d4;

            boneId = modelBone.mBoneId;
            sActiveModelScene->mBones[boneName] = modelBone;
        }
        else
        {
            boneId = modelBoneIt->second.mBoneId;
        }

        for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; weightIndex++)
        {
            const aiVertexWeight& weight = bone->mWeights[weightIndex];

            auto& vertex = vertices[weight.mVertexId];

            for (int influenceIndex = 0; influenceIndex < model::MAX_BONE_INFLUENCE; influenceIndex++)
            {
                if (vertex.mBoneIds[influenceIndex] == -1)
                {
                    vertex.mBoneIds[influenceIndex] = boneId;
                    vertex.mBoneWeights[influenceIndex] = weight.mWeight;

                    break;
                }
            }
        }
    }
}
