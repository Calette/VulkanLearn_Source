#include "Render/Vulkan/VulkanGlobal.h"
#include "Client.h"
#include "Common/Guid.h"
#include "Render/Shader/ConstantBuffer.h"
#include "Common/Time.h"

namespace Palette
{
	Client::Client()
	{
		uuid4_init();

		renderPipeline = new RenderPipeline();

		world = new World();
		world->Initialize();
		Time::Instance()->Initial();
	}

	Client::~Client()
	{
		delete renderPipeline;
		delete world;
		MaterialResource::ReleaseDefaultMat();
		RenderElement::PreRenderShader->ReleaseRef();
		ShaderResource::GetDefaultShader()->ReleaseRef();
		GlobalConstantBuffer::Instance()->ReleaseGlobalConstant();
	}

	void Client::Update_ot()
	{
		Time::Instance()->Update();
		world->Update_ot();
		GlobalConstantBuffer::Instance()->UpdateUniformBuffer();
	}

	void Client::Render_rt()
	{
		renderPipeline->Render_rt();
	}
}