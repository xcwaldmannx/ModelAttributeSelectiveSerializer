#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace mass
{
    class ModelHandler;

    struct Configuration
    {
        std::vector<std::string> mSupportedFileExtensions;
    };

    typedef enum SceneElements
    {
        NONE      = 0,
        ANIMATION = 1,
        CAMERA    = 2,
        LIGHT     = 4,
        MATERIAL  = 8,
        MESH      = 16,
        SKELETON  = 32,
        TEXTURE   = 64,
    } SceneElements;

    namespace model
    {
        struct Vertex;

        using VertexArray = std::vector<Vertex>;
        using IndexArray = std::vector<unsigned int>;

        constexpr unsigned int MAX_BONE_INFLUENCE = 4;
        constexpr unsigned int MAX_BONE_WEIGHTS   = 16;

        struct Vertex
        {
            Vertex()
            {
                std::fill_n(&mBoneIds[0], MAX_BONE_INFLUENCE, -1);
                std::fill_n(&mBoneWeights[0], MAX_BONE_WEIGHTS, 0);
            }

            float mPosition    [3] = {};
            float mNormal      [3] = {};
            float mTangent     [3] = {};
            float mBitangent   [3] = {};
            float mTexCoord    [2] = {};
            float mColor       [4] = {};
            int   mBoneIds     [MAX_BONE_INFLUENCE];
            float mBoneWeights [MAX_BONE_WEIGHTS];
        };

        struct VertexWeight
        {
            unsigned int mVertexId = 0;
            float mWeight = 0;
        };

        struct Bone
        {
            // VertexWeight mVertexWeights[MAX_BONE_INFLUENCE];
            int mBoneId = -1;
            float mOffsetMatrix[4][4] = {};
        };

        struct Mesh
        {
            unsigned int mVertexOffset = 0;
            unsigned int mVertexCount = 0;
            unsigned int mIndexOffset = 0;
            unsigned int mIndexCount = 0;
        };

        struct Node
        {
            float mTransform[4][4] = {};
            std::unordered_map<std::string, Mesh> mMeshes;
            std::unordered_map<std::string, Node> mChildNodes;
        };

        struct Scene
        {
            std::unordered_map<std::string, Node> mNodes;
            std::unordered_map<std::string, Bone> mBones;
        };

        class Model
        {
        public:
            const VertexArray& getVertices() const
            {
                return mVertices;
            }

            const IndexArray& getIndices() const
            {
                return mIndices;
            }

            bool hasAnimations() const
            {
                return mSceneElements & ANIMATION;
            }

            bool hasCameras() const
            {
                return mSceneElements & CAMERA;
            }

            bool hasLights() const
            {
                return mSceneElements & LIGHT;
            }

            bool hasMaterials() const
            {
                return mSceneElements & MATERIAL;
            }

            bool hasMeshes() const
            {
                return mSceneElements & MESH;
            }

            bool hasSkeletons() const
            {
                return mSceneElements & SKELETON;
            }

            bool hasTextures() const
            {
                return mSceneElements & TEXTURE;
            }

        private:
            char mSceneElements = 0;

            std::unordered_map<std::string, Scene> mScenes;

            VertexArray mVertices;
            IndexArray mIndices;

            friend class mass::ModelHandler;
        };
    }

    namespace anim
    {
        struct PositionKey
        {
            float mPosition[3] = {};
            float mTimestamp;
        };

        struct RotationKey
        {
            float mRotation[4] = {};
            float mTimestamp;
        };

        struct ScaleKey
        {
            float mScale[3] = {};
            float mTimestamp;
        };

        struct Channel
        {
            std::vector<PositionKey> mKeyPositions;
            std::vector<RotationKey> mKeyRotations;
            std::vector<ScaleKey> mKeyScales;
        };

        struct Animation
        {
            float mDuration = 0;
            float mTicksPerSecond = 0;
            std::unordered_map<std::string, Channel> mChannels;
        };

        class AnimationSet
        {
        public:
            const Animation& getAnimation(const std::string& name) const
            {
                return mAnimations.at(name);
            }

        private:
            std::unordered_map<std::string, Animation> mAnimations;
        };
    }

}
