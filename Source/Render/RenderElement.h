#pragma once

#include "Resource/RenderMesh.h"
#include "Resource/Material.h"

namespace Palette
{
	struct RenderElement
	{
		static Shader PreRenderShader;
		static void ReleasePreRenderShader();
		RenderMesh Mesh;
		Material Mat;
	};

	void DrawMesh(RenderMesh mesh, Material mat);
	void DrawMeshInstance(RenderMesh mesh, Material mat);

	bool PrepareElement_rt(RenderElement* element, IRenderPass* pass);

	void DrawElement_rt(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element);
	void DrawElementInstance_rt(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element);
}