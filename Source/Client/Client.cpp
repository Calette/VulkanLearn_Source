#include "Render/Vulkan/VulkanGlobal.h"
#include "Client.h"
#include "Common/Guid.h"
#include "Render/Shader/ConstantBuffer.h"

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
		GlobalConstantBuffer::Instance()->ReleaseGlobalConstantBuffer();
		delete renderPipeline;
		delete world;
	}

	void Client::Update_ot()
	{
		world->Update_ot();
		GlobalConstantBuffer::Instance()->UpdateUniformBuffer();
	}

	void Client::Render_rt()
	{
		renderPipeline->Render_rt();
	}
}