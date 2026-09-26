#include "../main/Mal.h"

#include <filesystem>
#include <iostream>

#include <string>
#include <vector>

#define LINUX

void test()
{
	mal::Configuration config{};
	config.mSupportedFileExtensions = { "gltf" };

	const std::vector<std::string> files =
	{
		"/home/cwaldmann/Documents/BlenderModels/tentacle/tentacle.glb",
	};

	for (const auto& file : files)
	{
		mal::model::Model model = mal::m::load(file, config);

		mal::anim::AnimationSet animations = mal::a::load(file, config);

		mal::AnimationPlayer player(model, animations);
		mal::math::Mat4 trans = mal::math::Mat4::identity();
		player.setNodeTransform("tentacle_0", trans);
	}
}

int main()
{
	test();

	return 0;
}
