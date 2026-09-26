#include "ModelHandler.h"

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace fs = std::filesystem;

using namespace mal;

model::Model ModelHandler::load(const std::string& filename, Configuration& config)
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

    sActiveModel = new model::Model();

    if (scene->HasAnimations()) sActiveModel->mSceneElements |= ANIMATION;
    if (scene->HasCameras())    sActiveModel->mSceneElements |= CAMERA;
    if (scene->HasLights())     sActiveModel->mSceneElements |= LIGHT;
    if (scene->HasMaterials())  sActiveModel->mSceneElements |= MATERIAL;
    if (scene->HasMeshes())     sActiveModel->mSceneElements |= MESH;
    if (scene->HasSkeletons())  sActiveModel->mSceneElements |= SKELETON;
    if (scene->HasTextures())   sActiveModel->mSceneElements |= TEXTURE;

    sVertexOffset = 0;
    sIndexOffset = 0;

    sSceneAnimations = scene->mAnimations;
    sSceneCameras    = scene->mCameras;
    sSceneLights     = scene->mLights;
    sSceneMaterials  = scene->mMaterials;
    sSceneMeshes     = scene->mMeshes;
    sSceneSkeletons  = scene->mSkeletons;
    sSceneTextures   = scene->mTextures;

    model::Scene modelScene;
    auto sceneName = scene->mName.C_Str();

    sActiveModel->mScenes.emplace(sceneName, modelScene);
    sActiveModelScene = &sActiveModel->mScenes.at(sceneName);

    processScene(scene, sActiveModelScene);

    model::Model resultModel = *sActiveModel;

    sActiveModelScene = nullptr;
    sSceneTextures    = nullptr;
    sSceneSkeletons   = nullptr;
    sSceneMeshes      = nullptr;
    sSceneMaterials   = nullptr;
    sSceneLights      = nullptr;
    sSceneCameras     = nullptr;
    sSceneAnimations  = nullptr;

    sIndexOffset = 0;
    sVertexOffset = 0;

    delete sActiveModel;
    sActiveModel = nullptr;

    return resultModel;
}

void ModelHandler::categorize(const aiScene* scene)
{
    if (scene->HasAnimations()) sActiveModel->mSceneElements |= ANIMATION;
    if (scene->HasCameras())    sActiveModel->mSceneElements |= CAMERA;
    if (scene->HasLights())     sActiveModel->mSceneElements |= LIGHT;
    if (scene->HasMaterials())  sActiveModel->mSceneElements |= MATERIAL;
    if (scene->HasMeshes())     sActiveModel->mSceneElements |= MESH;
    if (scene->HasSkeletons())  sActiveModel->mSceneElements |= SKELETON;
    if (scene->HasTextures())   sActiveModel->mSceneElements |= TEXTURE;
}

void ModelHandler::processScene(const aiScene* scene, model::Scene* modelScene)
{
    const auto& node = scene->mRootNode;
    model::Node modelNode;

    processNode(node, &modelNode);
    modelScene->mNodes.emplace(node->mName.C_Str(), std::move(modelNode));

}

void ModelHandler::processNode(const aiNode* node, model::Node* modelNode)
{
    const auto meshCount = node->mNumMeshes;
    const auto childNodeCount = node->mNumChildren;

    const auto& nodeTrans = node->mTransformation;
    auto& modelNodeTrans = modelNode->mTransform;

    modelNodeTrans.a1 = nodeTrans.a1; modelNodeTrans.b1 = nodeTrans.b1;
    modelNodeTrans.a2 = nodeTrans.a2; modelNodeTrans.b2 = nodeTrans.b2;
    modelNodeTrans.a3 = nodeTrans.a3; modelNodeTrans.b3 = nodeTrans.b3;
    modelNodeTrans.a4 = nodeTrans.a4; modelNodeTrans.b4 = nodeTrans.b4;

    modelNodeTrans.c1 = nodeTrans.c1; modelNodeTrans.d1 = nodeTrans.d1;
    modelNodeTrans.c2 = nodeTrans.c2; modelNodeTrans.d2 = nodeTrans.d2;
    modelNodeTrans.c3 = nodeTrans.c3; modelNodeTrans.d3 = nodeTrans.d3;
    modelNodeTrans.c4 = nodeTrans.c4; modelNodeTrans.d4 = nodeTrans.d4;

    for (unsigned int i = 0; i < meshCount; i++)
    {
        const auto& mesh = sSceneMeshes[node->mMeshes[i]];
        model::Mesh modelMesh;

        processMesh(mesh, &modelMesh);

        modelNode->mMeshes.emplace(mesh->mName.C_Str(), modelMesh);
    }

    for (unsigned int i = 0; i < childNodeCount; i++)
    {
        const auto& childNode = node->mChildren[i];
        model::Node modelChildNode;

        processNode(childNode, &modelChildNode);

        modelNode->mChildNodes.emplace(childNode->mName.C_Str(), std::move(modelChildNode));
    }
}

void ModelHandler::processMesh(const aiMesh* mesh, model::Mesh* modelMesh)
{
    if (!mesh->HasPositions()) return;

    model::VertexArray vertices;
    model::IndexArray indices;
    getVertexData(mesh, vertices, indices, sActiveModel->mBoundsMin, sActiveModel->mBoundsMax);

    modelMesh->mVertexOffset = sVertexOffset;
    modelMesh->mVertexCount = vertices.size();

    modelMesh->mIndexOffset = sIndexOffset;
    modelMesh->mIndexCount = indices.size();

    if (mesh->HasBones())
    {
        processBones(mesh, vertices);
    }

    sActiveModel->mVertices.insert(sActiveModel->mVertices.end(), vertices.begin(), vertices.end());
    sActiveModel->mIndices.insert(sActiveModel->mIndices.end(), indices.begin(), indices.end());

    sVertexOffset += vertices.size();
    sIndexOffset += indices.size();
}

void ModelHandler::getVertexData(
        const aiMesh* mesh,
        model::VertexArray& vertices,
        model::IndexArray& indices,
        math::Vec3& boundsMin,
        math::Vec3& boundsMax)
{
    const unsigned int vertexCount = mesh->mNumVertices;
    const unsigned int indexCount = mesh->mNumFaces * 3;

    vertices.resize(vertexCount);
    indices.reserve(indexCount);

    for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++)
    {
        model::Vertex modelVertex;

        const auto& vertex = mesh->mVertices[vertexIndex];
        modelVertex.mPosition.x = vertex.x;
        modelVertex.mPosition.y = vertex.y;
        modelVertex.mPosition.z = vertex.z;

        if (vertex.x < boundsMin.x) boundsMin.x = vertex.x;
        if (vertex.y < boundsMin.y) boundsMin.y = vertex.y;
        if (vertex.z < boundsMin.z) boundsMin.z = vertex.z;

        if (vertex.x > boundsMax.x) boundsMax.x = vertex.x;
        if (vertex.y > boundsMax.y) boundsMax.y = vertex.y;
        if (vertex.z > boundsMax.z) boundsMax.z = vertex.z;

        if (mesh->HasNormals())
        {
            const auto& normal = mesh->mNormals[vertexIndex];
            modelVertex.mNormal.x = normal.x;
            modelVertex.mNormal.y = normal.y;
            modelVertex.mNormal.z = normal.z;
        }

        if (mesh->HasTangentsAndBitangents())
        {
            const auto& tangent = mesh->mTangents[vertexIndex];
            modelVertex.mTangent.x = tangent.x;
            modelVertex.mTangent.y = tangent.y;
            modelVertex.mTangent.z = tangent.z;

            const auto& bitangent = mesh->mBitangents[vertexIndex];
            modelVertex.mBitangent.x = bitangent.x;
            modelVertex.mBitangent.y = bitangent.y;
            modelVertex.mBitangent.z = bitangent.z;
        }

        const unsigned int texCoordSet = 0;

        if (mesh->HasTextureCoords(texCoordSet))
        {
            const auto& texCoords = mesh->mTextureCoords[texCoordSet][vertexIndex];
            modelVertex.mTexCoord.x = texCoords.x;
            modelVertex.mTexCoord.y = texCoords.y;
        }

        const unsigned int colorSet = 0;

        if (mesh->HasVertexColors(colorSet))
        {
            const auto& color = mesh->mColors[colorSet][vertexIndex];
            modelVertex.mColor.x = color.r;
            modelVertex.mColor.y = color.g;
            modelVertex.mColor.z = color.b;
            modelVertex.mColor.w = color.a;
        }

        vertices[vertexIndex] = modelVertex;
    }

    for (unsigned int f = 0; f < mesh->mNumFaces; f++)
    {
        const aiFace& face = mesh->mFaces[f];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
}

void ModelHandler::processBones(const aiMesh* mesh, model::VertexArray& vertices)
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

            ModelBoneTrans.a1 = boneTrans.a1; ModelBoneTrans.b1 = boneTrans.b1;
            ModelBoneTrans.a2 = boneTrans.a2; ModelBoneTrans.b2 = boneTrans.b2;
            ModelBoneTrans.a3 = boneTrans.a3; ModelBoneTrans.b3 = boneTrans.b3;
            ModelBoneTrans.a4 = boneTrans.a4; ModelBoneTrans.b4 = boneTrans.b4;

            ModelBoneTrans.c1 = boneTrans.c1; ModelBoneTrans.d1 = boneTrans.d1;
            ModelBoneTrans.c2 = boneTrans.c2; ModelBoneTrans.d2 = boneTrans.d2;
            ModelBoneTrans.c3 = boneTrans.c3; ModelBoneTrans.d3 = boneTrans.d3;
            ModelBoneTrans.c4 = boneTrans.c4; ModelBoneTrans.d4 = boneTrans.d4;

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
