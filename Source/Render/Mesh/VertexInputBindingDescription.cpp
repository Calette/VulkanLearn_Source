#include "VertexInputBindingDescription.h"

namespace Palette
{
	void GetVertexInputDescriptions(VertexFormat vertexFormat, VkVertexInputBindingDescription& bindingDescription, std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
	{
		if ((int)vertexFormat <= (int)VertexFormat::VERTEX_COMMON)
		{
			Vertex_Common::GetBindingDescription(bindingDescription);
			Vertex_Common::GetAttributeDescriptions(attributeDescriptions, vertexFormat);
			return;
		}

		switch (vertexFormat)
		{
		case VertexFormat::VERTEX_P3_C3_U2:
			Vertex_P3_C3_T2::GetBindingDescription(bindingDescription);
			Vertex_P3_C3_T2::GetAttributeDescriptions(attributeDescriptions);
			break;
		default:
			throw std::runtime_error("failed to create Vertex Description!");
			break;
		}
	}

	void Vertex_Common::GetBindingDescription(VkVertexInputBindingDescription& bindingDescription)
	{
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex_Common);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	void Vertex_Common::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions, VertexFormat vertexFormat)
	{
		attributeDescriptions.clear();

		uint32_t location = 0;
		if ((int)vertexFormat & (int)VertexAtterbute::Position)
		{
			attributeDescriptions.push_back(VkVertexInputAttributeDescription({ location, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Common, pos) }));
			location++;
		}
		if ((int)vertexFormat & (int)VertexAtterbute::Normal)
		{
			attributeDescriptions.push_back(VkVertexInputAttributeDescription({ location, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Common, normal) }));
			location++;
		}
		if ((int)vertexFormat & (int)VertexAtterbute::UV)
		{
			attributeDescriptions.push_back(VkVertexInputAttributeDescription({ location, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_Common, uv) }));
			location++;
		}
		if ((int)vertexFormat & (int)VertexAtterbute::Color)
		{
			// todo VK_FORMAT_R8G8B8A8_UNORM
			attributeDescriptions.push_back(VkVertexInputAttributeDescription({ location, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex_Common, color) }));
			location++;
		}
		if ((int)vertexFormat & (int)VertexAtterbute::Tangent)
		{
			attributeDescriptions.push_back(VkVertexInputAttributeDescription({ location, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_Common, tangent) }));
			location++;
		}
	}

	void Vertex_P3_C3_T2::GetBindingDescription(VkVertexInputBindingDescription& bindingDescription)
	{
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex_P3_C3_T2);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	}

	void Vertex_P3_C3_T2::GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions)
	{
		attributeDescriptions = std::vector<VkVertexInputAttributeDescription>
		{
			VkVertexInputAttributeDescription({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_P3_C3_T2, pos) }),
			VkVertexInputAttributeDescription({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex_P3_C3_T2, color) }),
			VkVertexInputAttributeDescription({ 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex_P3_C3_T2, uv) })
		};
	}
}