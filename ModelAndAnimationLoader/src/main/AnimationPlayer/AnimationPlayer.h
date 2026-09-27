#pragma once

#include "../Types.h"

namespace mal
{

    class AnimationPlayer
    {
    public:
        AnimationPlayer(const model::Model& model, const anim::AnimationSet& animationSet);

        void setDefaultScene();
        void setScene(const std::string& name);

        void play(const std::string& name);

        void update(const float delta);

        void setNodeTransform(const std::string& name, const glm::mat4& transform);

        const std::vector<glm::mat4>& getBoneTransforms() const;

    private:
        void updateNode(const std::string& name, const model::Node& node, const glm::mat4& parentTransform);

        static glm::mat4 sampleChannel(const anim::Channel& channel, const float time);
        static glm::vec3 samplePosition(const anim::Channel& channel, const float time);
        static glm::quat sampleRotation(const anim::Channel& channel, const float time);
        static glm::vec3 sampleScale(const anim::Channel& channel, const float time);

    private:
        const model::Model& mModel;
        const anim::AnimationSet& mAnimationSet;

        const model::Scene* mCurrentScene = nullptr;
        const anim::Animation* mCurrentAnimation = nullptr;
        float mCurrentTime = 0.0f;

        glm::mat4 mInverseRootTransform{};
        std::vector<glm::mat4> mBoneTransforms;
        std::unordered_map<std::string, glm::mat4> mNodeTransforms;
    };

}