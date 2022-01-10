#include "RenderPipeline.h"
#include "SceneCuller.h"
#include "Render/Vulkan/VulkanGlobal.h"
#include "IRenderPass.h"

namespace Palette
{
	RenderPipeline::RenderPipeline()
	{
		m_FrameGraph = new FrameGraph();
		mainScene = new RenderScene(Camera::MainCamera);
	}

	RenderPipeline::~RenderPipeline()
	{
		delete m_FrameGraph;
		delete mainScene;
	}

	void RenderPipeline::Update_rt()
	{
		RenderElement::PreRenderShader = nullptr;
		mainScene->Reset();
		simplePass.Update_rt();
	}

	void RenderPipeline::SetFinalPass(IRenderPass* pass)
	{
		pass->SetFinalOutput();
		if (finalPass != pass)
		{
			if (finalPass)
				PaletteGlobal::vulkanDevice->FreeFramebuffers();
			PaletteGlobal::vulkanDevice->CreateFramebuffers(pass->GetRenderPass());
			pass->SetFramebuffer(PaletteGlobal::vulkanDevice->GetSwapChainFramebuffer());
			pass->SetExtent(PaletteGlobal::vulkanDevice->GetExtent2D());
			finalPass = pass;
		}
	}

	IRenderPass* RenderPipeline::Setup_rt()
	{
		IRenderPass* output = nullptr;

		output = simplePass.Connect(output, mainScene);

		SetFinalPass(output);
		return output;
	}

	void RenderPipeline::Render_rt()
	{
		if (Camera::MainCamera == nullptr)
		{
			// todo
			//return;
		}
		Update_rt();

		mainScene->SetCamera(Camera::MainCamera);
		SceneCuller::Instance()->Culling(Camera::MainCamera, mainScene);

		IRenderPass* pass = Setup_rt();

		m_FrameGraph->Setup_rt(pass);
		m_FrameGraph->Execute_rt();
	}
}