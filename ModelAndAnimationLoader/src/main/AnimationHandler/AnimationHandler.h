#pragma once

#include "../Types.h"

#include <assimp/scene.h>

namespace mal
{

    class AnimationHandler
    {
    public:
        static anim::AnimationSet load(const std::string& filename, Configuration& config);

    private:
        static void processAnimation(const aiAnimation* sceneAnim, anim::Animation* animation);
        static void processChannel(const aiNodeAnim* nodeAnim, anim::Channel* channel);
    };

}