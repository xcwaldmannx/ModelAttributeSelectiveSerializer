#include "AnimationPlayer.h"

#include <cmath>
#include <ranges>

using namespace mass;

AnimationPlayer::AnimationPlayer(const model::Model& model, const anim::AnimationSet& animationSet) :
    mModel(model), mAnimationSet(animationSet) {}

void AnimationPlayer::setDefaultScene()
{
    mCurrentScene = &mModel.getDefaultScene();

    mBoneTransforms.assign(mCurrentScene->mBones.size(), math::Mat4::identity());

    if (!mCurrentScene->mNodes.empty())
    {
        const auto& root = mCurrentScene->mNodes.begin()->second;
        mInverseRootTransform = root.mTransform.inverse();
    }
    else
    {
        mInverseRootTransform = math::Mat4::identity();
    }
}

void AnimationPlayer::setScene(const std::string& name)
{
    mCurrentScene = &mModel.getScene(name);

    mBoneTransforms.assign(mCurrentScene->mBones.size(), math::Mat4::identity());

    if (!mCurrentScene->mNodes.empty())
    {
        const auto& root = mCurrentScene->mNodes.begin()->second;
        mInverseRootTransform = root.mTransform.inverse();
    }
    else
    {
        mInverseRootTransform = math::Mat4::identity();
    }
}

void AnimationPlayer::play(const std::string& name)
{
    mCurrentAnimation = &mAnimationSet.getAnimation(name);
    mCurrentTime = 0.0f;
}

void AnimationPlayer::update(const float delta)
{
    if (!mCurrentAnimation || !mCurrentScene) return;

    float ticksPerSecond = mCurrentAnimation->mTicksPerSecond;
    if (ticksPerSecond <= 0.0f) ticksPerSecond = 1.0f;

    mCurrentTime += delta * ticksPerSecond;

    if (mCurrentAnimation->mDuration > 0.0f)
    {
        mCurrentTime = std::fmod(mCurrentTime, mCurrentAnimation->mDuration);
    }


    for (const auto& [rootName, rootNode] : mCurrentScene->mNodes)
    {
        updateNode(rootName, rootNode, math::Mat4::identity());
    }
}

void AnimationPlayer::setNodeTransform(const std::string& name, const math::Mat4& transform)
{
    if (mAnimationSet.mBones.contains(name))
    {
        mNodeTransforms[name] = transform;
    }
    else
    {
        throw std::runtime_error("node name does not exist.");
    }
}

const std::vector<math::Mat4>& AnimationPlayer::getBoneTransforms() const
{
    return mBoneTransforms;
}

void AnimationPlayer::updateNode(const std::string& name, const model::Node& node, const math::Mat4& parentTransform)
{
    math::Mat4 localTransform = node.mTransform;

    if (const auto channelIt = mCurrentAnimation->mChannels.find(name); channelIt != mCurrentAnimation->mChannels.end())
    {
        localTransform = sampleChannel(channelIt->second, mCurrentTime);
    }

    if (const auto transformIt = mNodeTransforms.find(name); transformIt != mNodeTransforms.end())
    {
        localTransform = localTransform * transformIt->second;
    }

    const math::Mat4 globalTransform = parentTransform * localTransform;

    if (const auto boneIt = mCurrentScene->mBones.find(name); boneIt != mCurrentScene->mBones.end())
    {
        const model::Bone& bone = boneIt->second;

        if (bone.mBoneId >= 0 && static_cast<size_t>(bone.mBoneId) < mBoneTransforms.size())
        {
            mBoneTransforms[bone.mBoneId] = mInverseRootTransform * globalTransform * bone.mOffsetMatrix;
        }
    }

    for (const auto& [childName, childNode] : node.mChildNodes)
    {
        updateNode(childName, childNode, globalTransform);
    }
}

math::Mat4 AnimationPlayer::sampleChannel(const anim::Channel& channel, const float time)
{
    const math::Vec3 position = samplePosition(channel, time);
    const math::Quat rotation = sampleRotation(channel, time);
    const math::Vec3 scale    = sampleScale(channel, time);

    return math::Mat4::identity().translate(position).rotate(rotation).scale(scale);
}

math::Vec3 AnimationPlayer::samplePosition(const anim::Channel& channel, const float time)
{
    const auto& keys = channel.mKeyPositions;

    if (keys.empty()) return math::Vec3{ 0.0f, 0.0f, 0.0f };

    if (keys.size() == 1)
    {
        return keys.front().mPosition;
    }

    std::size_t nextIndex = 1;

    while (nextIndex < keys.size() && time >= keys[nextIndex].mTimestamp) nextIndex++;

    if (nextIndex == keys.size())
    {
        return keys.back().mPosition;
    }

    const auto& previous = keys[nextIndex - 1];
    const auto& next = keys[nextIndex];

    const float duration = next.mTimestamp - previous.mTimestamp;

    const float factor = duration > 0.0f ? (time - previous.mTimestamp) / duration : 0.0f;

    return math::lerp(previous.mPosition, next.mPosition, factor);
}

math::Quat AnimationPlayer::sampleRotation(const anim::Channel& channel, const float time)
{
    const auto& keys = channel.mKeyRotations;

    if (keys.empty())
    {
        return math::Quat{ 0.0f, 0.0f, 0.0f, 1.0f };
    }

    if (keys.size() == 1)
    {
        return keys.front().mRotation.normalize();
    }

    size_t nextIndex = 1;

    while (nextIndex < keys.size() && time >= keys[nextIndex].mTimestamp) nextIndex++;

    if (nextIndex >= keys.size())
    {
        return keys.back().mRotation.normalize();
    }

    const auto& previous = keys[nextIndex - 1];
    const auto& next = keys[nextIndex];

    const float duration = next.mTimestamp - previous.mTimestamp;

    const float factor = duration > 0.0f ? (time - previous.mTimestamp) / duration : 0.0f;

    return math::slerp(previous.mRotation, next.mRotation, factor);
}

math::Vec3 AnimationPlayer::sampleScale(const anim::Channel& channel, const float time)
{
    const auto& keys = channel.mKeyScales;

    if (keys.empty()) return math::Vec3{ 1.0f, 1.0f, 1.0f };

    if (keys.size() == 1)
    {
        return keys.front().mScale;
    }

    size_t nextIndex = 1;

    while (nextIndex < keys.size() && time >= keys[nextIndex].mTimestamp) nextIndex++;

    if (nextIndex >= keys.size())
    {
        return keys.back().mScale;
    }

    const auto& previous = keys[nextIndex - 1];
    const auto& next = keys[nextIndex];

    const float duration = next.mTimestamp - previous.mTimestamp;

    const float factor = duration > 0.0f ? (time - previous.mTimestamp) / duration : 0.0f;

    return math::lerp(previous.mScale, next.mScale, factor);
}
