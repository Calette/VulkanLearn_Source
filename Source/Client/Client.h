#pragma once
#include "Render/RenderPipeline.h"
#include "World/World.h"

namespace Palette
{
	class Client
	{
	public:
		Client();
		~Client();

		void Update_ot();
		void Render_rt();

		World* GetWorld() { return world; };
		RenderPipeline* GetRenderPipeline() { return renderPipeline; };

	protected:
		RenderPipeline* renderPipeline;
		World* world;
	};
}