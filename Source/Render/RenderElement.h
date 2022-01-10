#pragma once

#include "Resource/RenderMesh.h"
#include "Resource/Material.h"

namespace Palette
{
	struct RenderElement
	{
		static Shader PreRenderShader;
		RenderMesh Mesh;
		Material Mat;
	};

	void DrawMesh(RenderMesh mesh, Material mat);
	void DrawMeshInstance(RenderMesh mesh, Material mat);

	void DrawElement(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element);
	void DrawElementInstance(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element);
}