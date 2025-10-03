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

struct ModelData
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

	std::unordered_map<std::string, ModelData> mMeshes;
    std::vector<Vertex> mVertices{};
    std::vector<uint32_t> mIndices{};
    std::vector<glm::mat4> mTransforms{};
    // TODO: std::vector<Animation> mAnimations{};
};

class Importer
{
public:
	void load(std::string filepath, Model* model, uint32_t flags)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath, aiProcessPreset_TargetRealtime_MaxQuality /*aiProcess_FlipUVs*/);

		if (!scene) {
			throw std::runtime_error("Could not load model!");
		}

		auto meshes = scene->mMeshes;
		auto rootNode = scene->mRootNode;

        model->mFlags = flags;

        model->mVertexStrideBytes =
            POSITION_LENGTH_BYTES +
            ((flags & HAS_NORMALS)   ? NORMAL_LENGTH_BYTES   : 0) +
            ((flags & HAS_COLORS)    ? COLOR_LENGTH_BYTES    : 0) +
            ((flags & HAS_TEXCOORDS) ? TEXCOORD_LENGTH_BYTES : 0);

		processModel(rootNode, meshes, model, flags);
	}

private:
    void processModel(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags)
    {
        model->mVertices.clear();
        model->mIndices.clear();
        model->mTransforms.clear();

        processNodes(node, meshes, model, flags);
    }

    void processNodes(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            auto mesh = meshes[node->mMeshes[i]];

            if (!mesh->HasPositions())
                continue;

            size_t vertexOffset = model->mVertices.size();
            size_t indexOffset = model->mIndices.size();
            size_t transformOffset = model->mTransforms.size();

            // Extract vertices
            for (unsigned int vIndex = 0; vIndex < mesh->mNumVertices; vIndex++)
            {
                Vertex vert{};
                vert.mPosition = glm::vec3(
                    mesh->mVertices[vIndex].x,
                    mesh->mVertices[vIndex].y,
                    mesh->mVertices[vIndex].z
                );

                if (mesh->HasNormals() && (flags & HAS_NORMALS))
                {
                    vert.mNormal = glm::vec3(
                        mesh->mNormals[vIndex].x,
                        mesh->mNormals[vIndex].y,
                        mesh->mNormals[vIndex].z
                    );
                }

                if (mesh->HasVertexColors(0) && (flags & HAS_COLORS))
                {
                    aiColor4D color = mesh->mColors[0][vIndex];
                    vert.mColor = glm::vec4(
                        color.r,
                        color.g,
                        color.b,
                        color.a
                    );
                }

                if (mesh->HasTextureCoords(0) && (flags & HAS_TEXCOORDS))
                {
                    vert.mTexCoord = glm::vec2(
                        mesh->mTextureCoords[0][vIndex].x,
                        mesh->mTextureCoords[0][vIndex].y
                    );
                }

                model->mVertices.emplace_back(std::move(vert));
            }

            // Extract indices
            for (unsigned int f = 0; f < mesh->mNumFaces; f++)
            {
                const aiFace& face = mesh->mFaces[f];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    uint32_t index = face.mIndices[j] + vertexOffset;
                    model->mIndices.emplace_back(std::move(index));
                }
            }

            uint32_t vertexCount = mesh->mNumVertices;
            uint32_t indexCount = mesh->mNumFaces * 3;

            if (vertexOffset + vertexCount >= UINT32_MAX)
            {
                throw std::runtime_error("Too many vertices for uint16_t indexing.");
            }

            // Transform
            if (flags & HAS_TRANSFORMS)
            {
                auto t = node->mTransformation;

                glm::mat4 transform =
                {
                    { t.a1, t.b1, t.c1, t.d1 },
                    { t.a2, t.b2, t.c2, t.d2 },
                    { t.a3, t.b3, t.c3, t.d3 },
                    { t.a4, t.b4, t.c4, t.d4 },
                };

                model->mTransforms.emplace_back(std::move(transform));
            }

            // Store mesh instance
            std::cout << "Loaded model " << mesh->mName.C_Str() << "\n";
            model->mMeshes[mesh->mName.C_Str()] =
            {
                .mVertexOffset = vertexOffset,
                .mVertexCount = vertexCount,
                .mIndexOffset = indexOffset,
                .mIndexCount = indexCount,
                .mTransformOffset = transformOffset,
            };
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNodes(node->mChildren[i], meshes, model, flags);
        }
    }

};