#include "RenderElement.h"
#include "IRenderPass.h"

namespace Palette
{
	Shader RenderElement::PreRenderShader = nullptr;

	void DrawMesh(RenderMesh mesh, Material mat)
	{
		// todo
		// 直接加到各个passd的m_Elements
	}

	void DrawMeshInstance(RenderMesh mesh, Material mat)
	{
		// todo
		// 直接加到各个passd的m_Elements
	}

	void DrawElement(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element)
	{
		for (auto shader : element->Mat->GetShaders())
		{
			if ((uint32_t)shader->GetPassType() & (uint32_t)pass->GetPassType())
			{
				if (shader != RenderElement::PreRenderShader)
				{
					if (!shader->HasPipeline())
					{
						CreatePipeline(pass, shader);
						shader->FinishCreatePipeline();
					}
					vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());
				}

				VkBuffer vertexBuffers[] = { element->Mesh->GetVertexBuffer() };
				VkBuffer indexBuffer = element->Mesh->GetIndexBuffer();
				VkDeviceSize offsets[] = { 0 };

				vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

				//uint32_t vertexDataSize = static_cast<uint32_t>(mesh->GetVertexData().size());
				//vkCmdDraw(commandBuffers[i], vertexDataSize, 1, 0, 0);
				uint32_t indexDataSize = static_cast<uint32_t>(element->Mesh->GetIndexData().size());
				vkCmdDrawIndexed(cmd, static_cast<uint32_t>(indexDataSize), 1, 0, 0, 0);

				RenderElement::PreRenderShader = shader;
			}
		}
	}

	void DrawElementInstance(VkCommandBuffer& cmd, RenderElement* element)
	{

	}
}