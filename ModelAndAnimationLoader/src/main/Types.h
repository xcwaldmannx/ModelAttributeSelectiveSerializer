#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace mal
{
    class ModelHandler;
    class AnimationHandler;
    class AnimationPlayer;
    class Loader;

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
        constexpr unsigned int MAX_BONE_WEIGHTS   = 4;

        struct Vertex
        {
            Vertex()
            {
                std::fill_n(&mBoneIds[0], MAX_BONE_INFLUENCE, -1);
                std::fill_n(&mBoneWeights[0], MAX_BONE_WEIGHTS, 0);
            }

            glm::vec3 mPosition{};
            glm::vec3 mNormal{};
            glm::vec3 mTangent{};
            glm::vec3 mBitangent{};
            glm::vec2 mTexCoord{};
            glm::vec4 mColor{};
            int mBoneIds [MAX_BONE_INFLUENCE];
            float mBoneWeights [MAX_BONE_WEIGHTS];
        };

        struct VertexWeight
        {
            unsigned int mVertexId = 0;
            float mWeight = 0;
        };

        struct Bone
        {
            int mBoneId = -1;
            glm::mat4 mOffsetMatrix{};
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
            glm::mat4 mTransform{};
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
            const Scene& getDefaultScene() const
            {
                if (mScenes.contains("Scene")) return mScenes.at("Scene");
                throw std::runtime_error("Scene not found");
            }

            const Scene& getScene(const std::string& scene) const
            {
                if (mScenes.contains(scene)) return mScenes.at(scene);
                throw std::runtime_error("Scene not found");
            }

            const VertexArray& getVertices() const
            {
                return mVertices;
            }

            const IndexArray& getIndices() const
            {
                return mIndices;
            }

            glm::vec3 getBoundsMin() const
            {
                return mBoundsMin;
            }

            glm::vec3 getBoundsMax() const
            {
                return mBoundsMax;
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
            glm::vec3 mBoundsMin;
            glm::vec3 mBoundsMax;

            friend class mal::Loader;
            friend class mal::ModelHandler;
        };
    }

    namespace anim
    {
        struct PositionKey
        {
            glm::vec3 mPosition{};
            float mTimestamp;
        };

        struct RotationKey
        {
            glm::quat mRotation{};
            float mTimestamp;
        };

        struct ScaleKey
        {
            glm::vec3 mScale{};
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
            std::set<std::string> mBones;

            friend class mal::AnimationHandler;
            friend class mal::AnimationPlayer;
        };
    }

}
