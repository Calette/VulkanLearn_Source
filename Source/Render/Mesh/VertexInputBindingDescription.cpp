#include "VertexInputBindingDescription.h"

namespace Palette
{
	void GetVertexInputDescriptions(VertexFormat vertexFormat, VkVertexInputBindingDescription& bindingDescription, std::array<VkVertexInputAttributeDescription, 2>& attributeDescriptions)
	{
		switch (vertexFormat)
		{
		case VertexFormat::VERTEX_P2_C3:
			bindingDescription = Vertex_P2_C3::GetBindingDescription();
			attributeDescriptions = Vertex_P2_C3::GetAttributeDescriptions();
			break;
		default:
			throw std::runtime_error("failed to create Vertex Description!");
			break;
		}
	}
}