#include "Importer.h"

void Importer::load(std::string filepath, Model* model, uint32_t flags)
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
        ((flags & HAS_NORMALS) ? NORMAL_LENGTH_BYTES : 0) +
        ((flags & HAS_COLORS) ? COLOR_LENGTH_BYTES : 0) +
        ((flags & HAS_TEXCOORDS) ? TEXCOORD_LENGTH_BYTES : 0);

    processModel(rootNode, meshes, model, flags);
}

void Importer::processModel(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags)
{
    model->mVertices.clear();
    model->mIndices.clear();
    model->mTransforms.clear();

    processNodes(node, meshes, model, flags);
}

void Importer::processNodes(aiNode* node, aiMesh** meshes, Model* model, uint32_t flags)
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
