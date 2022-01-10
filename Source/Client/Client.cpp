#include "Render/Vulkan/VulkanGlobal.h"
#include "Client.h"
#include "Common/Guid.h"

namespace Palette
{
	Client::Client()
	{
		uuid4_init();

		renderPipeline = new RenderPipeline();

		world = new World();
		world->Initialize();
	}

	Client::~Client()
	{
		delete renderPipeline;
		delete world;
	}

	void Client::Update_ot()
	{
		world->Update_ot();
	}

	void Client::Render_rt()
	{
		renderPipeline->Render_rt();
	}
}