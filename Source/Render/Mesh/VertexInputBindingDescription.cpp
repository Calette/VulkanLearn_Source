#include "VertexInputBindingDescription.h"

namespace Palette
{
	void GetVertexInputDescriptions(VertexFormat vertexFormat, VkVertexInputBindingDescription& bindingDescription, std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
	{
		switch (vertexFormat)
		{
		case VertexFormat::VERTEX_P2_C3_T2:
			Vertex_P2_C3_T2::GetBindingDescription(bindingDescription);
			Vertex_P2_C3_T2::GetAttributeDescriptions(attributeDescriptions);
			break;
		default:
			throw std::runtime_error("failed to create Vertex Description!");
			break;
		}
	}

	void Vertex_P2_C3_T2::GetBindingDescription(VkVertexInputBindingDescription& bindingDescription)
	{
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex_P2_C3_T2);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	void Vertex_P2_C3_T2::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
	{
		attributeDescriptions = std::vector<VkVertexInputAttributeDescription>
		{
			VkVertexInputAttributeDescription({ 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_P2_C3_T2, pos) }),
			VkVertexInputAttributeDescription({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_P2_C3_T2, color) }),
			VkVertexInputAttributeDescription({ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_P2_C3_T2, texCoord) })
		};
	}
}