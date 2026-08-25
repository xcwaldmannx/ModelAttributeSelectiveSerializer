#pragma once

#include "../Types.h"

#include <assimp/scene.h>

namespace mass
{

    class AnimationHandler
    {
    public:
        static std::vector<anim::Animation> load(const std::string& filename, Configuration& config);

    private:
        static anim::Animation processAnimation(aiAnimation* sceneAnim);
    };

}