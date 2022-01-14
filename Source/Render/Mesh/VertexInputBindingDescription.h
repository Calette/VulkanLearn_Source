#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>
#include <stdexcept>

namespace Palette
{
	enum class VertexFormat
	{
		VERTEX_P2_C3_T2
	};

	struct Vertex
	{

	};

	struct Vertex_P2_C3_T2 : public Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
		glm::vec2 texCoord;

		Vertex_P2_C3_T2(glm::vec2 p, glm::vec3 c, glm::vec2 t) : pos(p), color(c), texCoord(t) {}

		static void GetBindingDescription(VkVertexInputBindingDescription& bindingDescription);

		static void GetAttributeDescriptions(std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
	};

	void GetVertexInputDescriptions(VertexFormat vertexFormat, VkVertexInputBindingDescription& bindingDescription, std::vector<VkVertexInputAttributeDescription>& attributeDescriptions);
}