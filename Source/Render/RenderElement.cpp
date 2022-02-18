#include "RenderElement.h"
#include "IRenderPass.h"
#include "Vulkan/VulkanGlobal.h"
#include "Render/Shader/ConstantBuffer.h"

namespace Palette
{
	Shader RenderElement::PreRenderShader = nullptr;

	void RenderElement::ReleasePreRenderShader()
	{
		PreRenderShader = nullptr;
	}

	void DrawMesh(RenderMesh mesh, Material mat)
	{
		// todo
		// 直接加到各个passd的m_Elements
		for (Shader shader : mat->GetShaders())
		{
			switch (shader->GetPassType())
			{
			case SIMPLE_PASS:
				//PaletteGlobal::client->GetRenderPipeline()
				break;
			default:
				break;
			}
		}
	}

	void DrawMeshInstance(RenderMesh mesh, Material mat)
	{
		// todo
		// 直接加到各个passd的m_Elements
	}

	bool PrepareElement_rt(RenderElement* element, IRenderPass* pass)
	{
		for (auto shader : element->Mat->GetShaders())
		{
			if (!shader->HasPipeline())
			{
				CreatePipeline(pass, shader);
				shader->FinishCreatePipeline();
			}

			auto& elementDescriptorSetList = shader->GetShaderModule()->GetConstantBuffers();

			for (auto& iter : elementDescriptorSetList)
			{
				for (auto buffer : iter.second)
				{
					if (buffer->GetType() == CUSTOM_CONSTANT)
						buffer->UpdateUniformBuffer();
				}
			}
		}
		return true;
	}

	void DrawElement_rt(VkCommandBuffer& cmd, IRenderPass* pass, RenderElement* element)
	{
		for (auto shader : element->Mat->GetShaders())
		{
			if ((uint32_t)shader->GetPassType() & (uint32_t)pass->GetPassType())
			{
				if (shader != RenderElement::PreRenderShader)
				{
					vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, shader->GetPipeline());
				}

				VkBuffer vertexBuffers[] = { element->Mesh->GetVertexBuffer() };
				VkBuffer indexBuffer = element->Mesh->GetIndexBuffer();
				VkDeviceSize offsets[] = { 0 };

				vkCmdBindVertexBuffers(cmd, 0, 1, vertexBuffers, offsets);
				vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

				std::vector<VkDescriptorSet> descriptorSetList{};
				shader->GetShaderModule()->GetDescriptorSets(descriptorSetList);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pass->GetPipelineLayout(), 0, descriptorSetList.size(), descriptorSetList.data(), 0, nullptr);

				uint32_t indexDataSize = static_cast<uint32_t>(element->Mesh->GetIndexData().size());
				vkCmdDrawIndexed(cmd, indexDataSize, 1, 0, 0, 0);

				RenderElement::PreRenderShader = shader;
			}
		}
	}

	void DrawElementInstance_rt(VkCommandBuffer& cmd, RenderElement* element)
	{

	}
}