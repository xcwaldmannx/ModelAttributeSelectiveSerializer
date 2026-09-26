#include "AnimationHandler.h"

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace mass;

anim::AnimationSet AnimationHandler::load(const std::string& filename, Configuration& config)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

    if (!scene)
    {
        throw std::runtime_error("could not load scene");
    }

    if (!scene->HasAnimations())
    {
        throw std::runtime_error("file has no animations");
    }

    anim::AnimationSet animationSet;

    const auto& sceneAnimations = scene->mAnimations;

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        anim::Animation animation;

        const auto& sceneAnim = sceneAnimations[i];
        processAnimation(sceneAnim, &animation);

        for (const auto& [name, channel] : animation.mChannels)
        {
            if (!animationSet.mBones.contains(name))
            {
                animationSet.mBones.emplace(name);
            }
        }

        animationSet.mAnimations.emplace(sceneAnim->mName.C_Str(), animation);
    }

    return animationSet;
}

void AnimationHandler::processAnimation(const aiAnimation* sceneAnim, anim::Animation* animation)
{
    animation->mDuration = static_cast<float>(sceneAnim->mDuration);

    animation->mTicksPerSecond = static_cast<float>(sceneAnim->mTicksPerSecond);

    for (unsigned int i = 0; i < sceneAnim->mNumChannels; ++i)
    {
        const aiNodeAnim* nodeAnim = sceneAnim->mChannels[i];

        anim::Channel channel;
        processChannel(nodeAnim, &channel);

        animation->mChannels.emplace(nodeAnim->mNodeName.C_Str(), std::move(channel));
    }
}

void AnimationHandler::processChannel(const aiNodeAnim* nodeAnim, anim::Channel* channel)
{
    channel->mKeyPositions.reserve(nodeAnim->mNumPositionKeys);
    channel->mKeyRotations.reserve(nodeAnim->mNumRotationKeys);
    channel->mKeyScales.reserve(nodeAnim->mNumScalingKeys);

    for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; ++i)
    {
        const auto& key = nodeAnim->mPositionKeys[i];

        anim::PositionKey modelKey;

        modelKey.mPosition.x = key.mValue.x;
        modelKey.mPosition.y = key.mValue.y;
        modelKey.mPosition.z = key.mValue.z;

        modelKey.mTimestamp = static_cast<float>(key.mTime);

        channel->mKeyPositions.push_back(modelKey);
    }

    for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; ++i)
    {
        const auto& key = nodeAnim->mRotationKeys[i];

        anim::RotationKey modelKey;

        modelKey.mRotation.x = key.mValue.x;
        modelKey.mRotation.y = key.mValue.y;
        modelKey.mRotation.z = key.mValue.z;
        modelKey.mRotation.w = key.mValue.w;

        modelKey.mTimestamp = static_cast<float>(key.mTime);

        channel->mKeyRotations.push_back(modelKey);
    }

    for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; ++i)
    {
        const auto& key = nodeAnim->mScalingKeys[i];

        anim::ScaleKey modelKey;

        modelKey.mScale.x = key.mValue.x;
        modelKey.mScale.y = key.mValue.y;
        modelKey.mScale.z = key.mValue.z;

        modelKey.mTimestamp = static_cast<float>(key.mTime);

        channel->mKeyScales.push_back(modelKey);
    }
}
