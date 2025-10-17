#pragma once

#include <iostream>
#include <stdexcept>
#include <stdint.h>

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

static uint8_t POSITION_LENGTH_BYTES = 12;
static uint8_t NORMAL_LENGTH_BYTES   = 12;
static uint8_t COLOR_LENGTH_BYTES    = 16;
static uint8_t TEXCOORD_LENGTH_BYTES = 8;

enum ModelLoaderFlags
{
    HAS_NORMALS    =  1,
    HAS_COLORS     =  2,
    HAS_TEXCOORDS  =  4,
    HAS_TRANSFORMS =  8,
    HAS_ANIMATIONS = 16
};

struct Vertex
{
    glm::vec3 mPosition{};
    glm::vec3 mNormal{};
    glm::vec4 mColor{};
    glm::vec2 mTexCoord{};
};

struct Mesh
{
    size_t mVertexOffset = 0;
	size_t mVertexCount = 0;

    size_t mIndexOffset = 0;
	size_t mIndexCount = 0;

    size_t mTransformOffset = 0;
};

struct Model
{
    uint32_t mFlags;
    uint8_t mVertexStrideBytes = 0;

	std::unordered_map<std::string, Mesh> mMeshes;
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};
    std::vector<glm::mat4> mTransforms{};
    // TODO: std::vector<Animation> mAnimations{};
};

class Importer
{
public:
    void load(std::string filepath, Model* model, uint32_t flags);

private:
    void processModel(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags);
    void processNodes(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags);

};