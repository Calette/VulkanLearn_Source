#pragma once
#include "RenderPass/SimplePass.h"
#include "FrameGraph/FrameGraph.h"

namespace Palette
{
	class RenderPipeline
	{
	public:
		RenderPipeline();
		~RenderPipeline();

		void Update_rt();
		void Render_rt();
		IRenderPass* Setup_rt();
		void SetFinalPass(IRenderPass* pass);

	protected:
		FrameGraph* m_FrameGraph;

		RenderScene* mainScene;

		IRenderPass* finalPass = nullptr;

		SimplePass simplePass;
	};
}