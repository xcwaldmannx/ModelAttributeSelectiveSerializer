#include "../main/Mal.h"

#include <filesystem>

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
		mal::model::Model model = mal::ModelHandler::load(file, config);

		mal::anim::AnimationSet animations = mal::AnimationHandler::load(file, config);

		mal::AnimationPlayer player(model, animations);

		player.setDefaultScene();
		player.play("attack");

		glm::mat4 trans = glm::mat4(1.0);
		player.setNodeTransform("tentacle_0", trans);

		player.update(0);
	}
}

int main()
{
	test();

	return 0;
}
