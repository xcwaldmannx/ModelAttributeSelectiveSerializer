#include "../main/Mass.h"

#include <filesystem>
#include <iostream>

#include <string>
#include <vector>

#define LINUX

void test()
{
	mass::Configuration config{};
	config.mSupportedFileExtensions = { "gltf" };

	const std::vector<std::string> files =
	{
		"/home/cwaldmann/Documents/BlenderModels/tentacle/tentacle.glb",
	};

	for (const auto& file : files)
	{
		mass::model::Model model = mass::m::load(file, config);

		mass::anim::AnimationSet animations = mass::a::load(file, config);

		mass::AnimationPlayer player(model, animations);
		mass::math::Mat4 trans = mass::math::Mat4::identity();
		player.setNodeTransform("tentacle_0", trans);
	}
}

int main()
{
	test();

	return 0;
}
