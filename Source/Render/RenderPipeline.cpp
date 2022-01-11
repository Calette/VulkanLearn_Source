#include "RenderPipeline.h"
#include "SceneCuller.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "IRenderPass.h"

namespace Palette
{
	RenderPipeline::RenderPipeline()
	{
		frameGraph = new FrameGraph();
		mainScene = new RenderScene(Camera::MainCamera);
	}

	RenderPipeline::~RenderPipeline()
	{
		delete frameGraph;
		delete mainScene;
	}

	void RenderPipeline::Pass_Update_rt()
	{
		RenderElement::PreRenderShader = nullptr;
		mainScene->Reset();
		simplePass.Update_rt();
	}

	IRenderPass* RenderPipeline::Setup_rt()
	{
		IRenderPass* output = nullptr;

		output = simplePass.Connect(output, mainScene);

		return output;
	}

	void RenderPipeline::Render_rt()
	{
		if (Camera::MainCamera == nullptr)
		{
			// todo
			//return;
		}

		Pass_Update_rt();

		mainScene->SetCamera(Camera::MainCamera);
		SceneCuller::Instance()->Culling(Camera::MainCamera, mainScene);

		IRenderPass* pass = Setup_rt();

		frameGraph->Setup_rt(pass);
		frameGraph->Execute_rt();
	}
}