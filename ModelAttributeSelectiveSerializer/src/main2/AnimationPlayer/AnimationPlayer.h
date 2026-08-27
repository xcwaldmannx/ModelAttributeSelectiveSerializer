#pragma once

#include "../Types.h"

namespace mass
{

    class AnimationPlayer
    {
    public:
        AnimationPlayer(const model::Model& model, const anim::AnimationSet& animationSet);

        void setScene(const std::string& name);

        void play(const std::string& name);

        void update(const float delta);

        void setNodeTransform(const std::string& name, const math::Mat4& transform);

        const std::vector<math::Mat4>& getBoneTransforms() const;

    private:
        void updateNode(const std::string& name, const model::Node& node, const math::Mat4& parentTransform);

        static math::Mat4 sampleChannel(const anim::Channel& channel, const float time);
        static math::Vec3 samplePosition(const anim::Channel& channel, const float time);
        static math::Quat sampleRotation(const anim::Channel& channel, const float time);
        static math::Vec3 sampleScale(const anim::Channel& channel, const float time);

    private:
        const model::Model& mModel;
        const anim::AnimationSet& mAnimationSet;

        const model::Scene* mCurrentScene = nullptr;
        const anim::Animation* mCurrentAnimation = nullptr;
        float mCurrentTime = 0.0f;

        math::Mat4 mInverseRootTransform;
        std::vector<math::Mat4> mBoneTransforms;
        std::unordered_map<std::string, math::Mat4> mNodeTransforms;
    };

}