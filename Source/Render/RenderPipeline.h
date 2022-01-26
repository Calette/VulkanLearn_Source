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

	protected:
		FrameGraph* frameGraph;
		RenderScene* mainScene;

		SimplePass simplePass;
	};
}