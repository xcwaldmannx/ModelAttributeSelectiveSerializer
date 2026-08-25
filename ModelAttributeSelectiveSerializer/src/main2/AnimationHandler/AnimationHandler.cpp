#include "AnimationHandler.h"

#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

using namespace mass;

std::vector<anim::Animation> AnimationHandler::load(const std::string& filename, Configuration& config)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);

    if (!scene)
    {
        throw std::runtime_error("could not load scene");
    }

    if (!scene->HasAnimations()) return;

    std::vector<anim::Animation> animations;

    const auto& sceneAnimations = scene->mAnimations;

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        const auto& sceneAnim = sceneAnimations[i];
        processAnimation(sceneAnim);
    }

    return animations;
}

anim::Animation AnimationHandler::processAnimation(aiAnimation* sceneAnim)
{
    anim::Animation animation;

    return animation;
}